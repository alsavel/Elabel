#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "lvgl.h"
#include "portmacro.h"
// Распиновка LilyGo T-Display-S3 (i8080 8-bit)
#define LCD_PIN_POWER   15
#define LCD_PIN_BL      38
#define LCD_PIN_RD      9
#define LCD_PIN_WR      8
#define LCD_PIN_RS      7
#define LCD_PIN_CS      6
#define LCD_PIN_RST     5

#define LCD_PIN_D0      39
#define LCD_PIN_D1      40
#define LCD_PIN_D2      41
#define LCD_PIN_D3      42
#define LCD_PIN_D4      45
#define LCD_PIN_D5      46
#define LCD_PIN_D6      47
#define LCD_PIN_D7      48
#define BTN_CLEAR_PIN   GPIO_NUM_14   // Нижняя кнопка (KEY) на LilyGo T-Display-S3

#define LCD_H_RES       320
#define LCD_V_RES       170

// Настройки текстовой консоли
#define LOG_QUEUE_LEN       16      // Глубина очереди сообщений
#define LOG_LINE_MAX_LEN    48      // Макс. длина одной строки (с учетом '\0')
#define LOG_MAX_LINES       10      // Макс. кол-во строк на экране (под высоту 170px)
typedef struct {
    const char *prefix;
    uint32_t delay_ms;
} worker_params_t;

static const char *TAG = "EPROO_LVGL";
static esp_lcd_panel_handle_t panel_handle = NULL;
static QueueHandle_t logQueue = NULL;
static lv_obj_t *scrLabel = NULL;
// Единый статический буфер экрана и счетчик строк в нем
static char scrLogTextBuf[LOG_MAX_LINES * LOG_LINE_MAX_LEN] = "";
static int current_lines = 0;

// Добавление одной строки в плоский буфер экрана со сдвигом при переполнении
static void append_line_to_screen_buf(const char *line)
{
    if (current_lines >= LOG_MAX_LINES) {
        char *first_nl = strchr(scrLogTextBuf, '\n');
        if (first_nl != NULL) {
            size_t tail_len = strlen(first_nl + 1);
            memmove(scrLogTextBuf, first_nl + 1, tail_len + 1);
            current_lines--;
        } else {
            *scrLogTextBuf = '\0';
            current_lines = 0;
        }
    }
    if (current_lines > 0) {
        strcat(scrLogTextBuf, "\n");
    }
    strcat(scrLogTextBuf, line);
    current_lines++;
}

// Колбэк завершения передачи DMA -> сообщаем LVGL, что буфер свободен
static bool on_color_trans_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
    lv_display_t *disp = (lv_display_t *)user_ctx;
    lv_display_flush_ready(disp);
    return false;
}

// Колбэк отрисовки области экрана из LVGL в контроллер ST7789
static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    uint32_t len = lv_area_get_size(area);
    lv_draw_sw_rgb565_swap(px_map, len);
    printf("[%lld] .\n", esp_timer_get_time() / 1000);
    esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, px_map);
}

// Колбэк системного времени (мс) для внутренних расчетов LVGL
static uint32_t esp_lv_tick_get_cb(void)
{
    return (uint32_t)(esp_timer_get_time() / 1000ULL);
}


// Потокобезопасная неблокирующая отправка строки в очередь консоли
void send_msg_to_log_queue(const char *fmt, ...)
{
    if (logQueue == NULL) {        return;    }
    char msg[LOG_LINE_MAX_LEN];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);
    // Убираем завершающий '\n', если он был передан в строке
    size_t len = strlen(msg);
    if (len > 0 && msg[len - 1] == '\n') {
        msg[len - 1] = '\0';
    }
    xQueueSend(logQueue, msg, 0);
}


// Тестовая задача одна функция для всех, задержка указана в pvParam
static void worker_task_core(void *pvParameters)
{
    const worker_params_t *params = (const worker_params_t*)pvParameters;
    uint32_t step = 0;
    while (1) {
        size_t free_heap = esp_get_free_heap_size() / 1024;
		send_msg_to_log_queue("+[%s] (%d) %s, step-%lu Heap%zuKB", pcTaskGetName(NULL), xPortGetCoreID(),
				params->prefix, step++, free_heap);
        vTaskDelay(pdMS_TO_TICKS(params->delay_ms));
    }
}

// Диагностика свободной памяти ESP32-S3
static void print_memory_info(void)
{
    printf("\n======21-11========== ESP32-S3 MEMORY INFO ================\n");
    printf("Internal RAM free:     %6u KB (min: %u KB)\n",
           heap_caps_get_free_size(MALLOC_CAP_INTERNAL) / 1024,
           heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL) / 1024);

    printf("Internal DMA free:     %6u KB (largest block: %u KB)\n",
           heap_caps_get_free_size(MALLOC_CAP_DMA) / 1024,
           heap_caps_get_largest_free_block(MALLOC_CAP_DMA) / 1024);

    size_t psram_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
    if (psram_total > 0) {
        printf("External PSRAM free:   %6u KB / %u KB\n",
               heap_caps_get_free_size(MALLOC_CAP_SPIRAM) / 1024,
               psram_total / 1024);
    } else {
        printf("External PSRAM:        Not enabled / Not detected\n");
    }
    printf("======================================================\n\n");
}

// Вывод списка всех задач FreeRTOS в Serial-консоль
static void print_tasks_to_serial(void)
{
#if (configUSE_TRACE_FACILITY == 1)
    UBaseType_t task_count = uxTaskGetNumberOfTasks();
    TaskStatus_t *status_array = malloc(task_count * sizeof(TaskStatus_t));
    if (status_array == NULL) {
        ESP_LOGE(TAG, "Не хватило памяти для списка задач");
        return;
    }
    task_count = uxTaskGetSystemState(status_array, task_count, NULL);
    printf("\n========== FREERTOS TASKS (%u) ===========\n", (unsigned int)task_count);
    printf("%-16s | %-4s | %-7s | %-4s | %-8s\n", "Name", "Core", "State", "Prio", "MinStack");
    printf("-----------------------------------------------------\n");
    for (UBaseType_t i = 0; i < task_count; i++) {
        const char *state_str = "Unknown";
        switch (status_array[i].eCurrentState) {
            case eRunning:   state_str = "Running"; break;
            case eReady:     state_str = "Ready";   break;
            case eBlocked:   state_str = "Blocked"; break;
            case eSuspended: state_str = "Suspend"; break;
            case eDeleted:   state_str = "Deleted"; break;
            default:         break;
        }
        char core_str[5] = "Any";
        BaseType_t core_id = xTaskGetCoreID(status_array[i].xHandle);
        if (core_id == 0) {
            strcpy(core_str, "0");
        } else if (core_id == 1) {
            strcpy(core_str, "1");
        }

        printf("%-16s | %-4s | %-7s | %-4u | %-8u\n",
               status_array[i].pcTaskName,
               core_str,
               state_str,
               (unsigned int)status_array[i].uxCurrentPriority,
               (unsigned int)status_array[i].usStackHighWaterMark);
    }
    printf("=====================================================\n\n");
    free(status_array);
#endif
}

// Обработчик прерывания нижней кнопки (GPIO 14) -> шлет команду очистки в очередь
static void IRAM_ATTR clear_btn_isr_handler(void *arg)
{
    char cmd[LOG_LINE_MAX_LEN] = "\x01CLEAR";
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(logQueue, cmd, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

static void init_clear_button(void)
{
    gpio_config_t btn_conf = {
        .pin_bit_mask = (1ULL << BTN_CLEAR_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };
    gpio_config(&btn_conf);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BTN_CLEAR_PIN, clear_btn_isr_handler, NULL);
}
//---------------------
void app_main(void)
{
    print_memory_info();

    logQueue = xQueueCreate(LOG_QUEUE_LEN, LOG_LINE_MAX_LEN);
    assert(logQueue != NULL);

    ESP_LOGI(TAG, "1. Включаем питание дисплея и подсветку...");
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << LCD_PIN_POWER) | (1ULL << LCD_PIN_BL) | (1ULL << LCD_PIN_RD),
    };
    gpio_config(&io_conf);
    gpio_set_level(LCD_PIN_POWER, 1);
    gpio_set_level(LCD_PIN_RD, 1);
    gpio_set_level(LCD_PIN_BL, 1);

    ESP_LOGI(TAG, "2. Инициализируем шину i8080 и контроллер ST7789...");
    esp_lcd_i80_bus_handle_t i80_bus = NULL;
    esp_lcd_i80_bus_config_t bus_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .dc_gpio_num = LCD_PIN_RS,
        .wr_gpio_num = LCD_PIN_WR,
        .data_gpio_nums = {
            LCD_PIN_D0, LCD_PIN_D1, LCD_PIN_D2, LCD_PIN_D3,
            LCD_PIN_D4, LCD_PIN_D5, LCD_PIN_D6, LCD_PIN_D7,
        },
        .bus_width = 8,
        .max_transfer_bytes = LCD_H_RES * LCD_V_RES * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(esp_lcd_new_i80_bus(&bus_config, &i80_bus));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i80_config_t io_config = {
        .cs_gpio_num = LCD_PIN_CS,
        .pclk_hz = 10 * 1000 * 1000,
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 0,
            .dc_data_level = 1,
        },
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i80(i80_bus, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LCD_PIN_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // Настройка ориентации и смещения для матрицы 170x320
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, true));
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(panel_handle, 0, 35));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    ESP_LOGI(TAG, "3. Инициализируем LVGL 9...");
    lv_init();
    lv_tick_set_cb(esp_lv_tick_get_cb);

    lv_display_t *disp = lv_display_create(LCD_H_RES, LCD_V_RES);
    lv_display_set_flush_cb(disp, lvgl_flush_cb);

    const esp_lcd_panel_io_callbacks_t cbs = {
        .on_color_trans_done = on_color_trans_done,
    };
    ESP_ERROR_CHECK(esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, disp));

    // Выделяем ОДИН полноэкранный буфер в DMA-памяти (106.25 КБ)
    size_t buf_size = LCD_H_RES * LCD_V_RES * sizeof(uint16_t);
    void *buf1 = heap_caps_malloc(buf_size, MALLOC_CAP_DMA);
    assert(buf1);
    lv_display_set_buffers(disp, buf1, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

    ESP_LOGI(TAG, "4. Настраиваем виджет консоли (одиночный статический Label)...");
    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    scrLabel = lv_label_create(scr);
    lv_obj_set_width(scrLabel, LCD_H_RES - 10);
    lv_label_set_long_mode(scrLabel, LV_LABEL_LONG_MODE_WRAP);
    lv_obj_set_style_text_color(scrLabel, lv_color_hex(0x00FF66), LV_PART_MAIN);
    lv_obj_set_style_text_align(scrLabel, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(scrLabel, LV_ALIGN_TOP_LEFT, 5, 4);

    // Привязываем статический буфер без выделения памяти в куче LVGL
    lv_label_set_text_static(scrLabel, scrLogTextBuf);

    send_msg_to_log_queue("LVGL 9.5 ready on Core %d", xPortGetCoreID());
    init_clear_button();

    // Запускаем две рабочие задачи на разных ядрах ESP32-S3
    static const worker_params_t par_c0 = {   .prefix = "tASK0-",   .delay_ms = 2300    };
    static const worker_params_t par_c1 = {   .prefix = "tASK1",   .delay_ms = 1300    };
    xTaskCreatePinnedToCore(worker_task_core, "taska-0", 3072, (void*)&par_c0, 7, NULL, 0);
    xTaskCreatePinnedToCore(worker_task_core, "taska-1", 3072, (void*)&par_c1, 6, NULL, 1);

    int64_t last_tasks_dump_us = esp_timer_get_time();
    char buff_for_data_read[LOG_LINE_MAX_LEN];

    while (1) {
        // Задача СПИТ (0% CPU), пока не придет лог, нажатие кнопки или не пройдет 8 сек
        if (xQueueReceive(logQueue, buff_for_data_read, pdMS_TO_TICKS(8000)) == pdTRUE) {
            do {
                if (*buff_for_data_read == '\x01') {
                    // Команда очистки от кнопки GPIO 14
                    xQueueReset(logQueue);
                    snprintf(scrLogTextBuf, sizeof(scrLogTextBuf),
                             "buffer cleared by GPIO %d", BTN_CLEAR_PIN);
                    current_lines = 1;
                } else {
                    append_line_to_screen_buf(buff_for_data_read);
                }
            } while (xQueueReceive(logQueue, buff_for_data_read, 0) == pdTRUE);

            // Сообщаем LVGL об изменении статического буфера и сразу рисуем кадр
            lv_label_set_text_static(scrLabel, scrLogTextBuf);
            lv_refr_now(NULL);
            //lv_timer_handler() needed anyway??
        }

        // Печать таблицы задач раз в 8 секунд
        int64_t now_us = esp_timer_get_time();
        if (now_us - last_tasks_dump_us >= 8000000LL) {
            last_tasks_dump_us = now_us;
            print_tasks_to_serial();
        }
    }
}


