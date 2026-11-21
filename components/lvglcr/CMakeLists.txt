set(LVGL_CORE_SOURCES
    # База и ОС
    "src/lv_init.c"
    "src/lv_stubs.c"
    "src/osal/lv_os.c"
    "src/osal/lv_os_none.c"
    "src/tick/lv_tick.c"

    # (не?)cтандартная библиотека (память, строки)
#    "src/stdlib/lv_string.c"
#    "src/stdlib/lv_sprintf.c"
#    "src/stdlib/builtin/lv_mem_core_builtin.c"
#    "src/stdlib/builtin/lv_sprintf_builtin.c"
#    "src/stdlib/builtin/lv_string_builtin.c"
#    "src/stdlib/builtin/lv_tlsf.c"

    # Стандартная библиотека (используем встроенную libc из ESP-IDF)
    "src/stdlib/lv_mem.c"
    "src/stdlib/clib/lv_mem_core_clib.c"
    "src/stdlib/clib/lv_sprintf_clib.c"
    "src/stdlib/clib/lv_string_clib.c"

    # Ядро объектов и цикл рендера
    "src/core/lv_obj.c"
    "src/core/lv_obj_class.c"
    "src/core/lv_obj_draw.c"
    "src/core/lv_obj_event.c"
    "src/core/lv_obj_id_builtin.c"
    "src/core/lv_obj_pos.c"
    "src/core/lv_obj_style.c"
    "src/core/lv_obj_style_gen.c"
    "src/core/lv_obj_tree.c"
    "src/core/lv_refr.c"
    "src/display/lv_display.c"

    # Утилиты ядра (без анимаций, файловой системы и профилировщика)
    "src/misc/lv_area.c"
    "src/misc/lv_array.c"
    "src/misc/lv_async.c"
    "src/misc/lv_bidi.c"
    "src/misc/lv_circle_buf.c"
    "src/misc/lv_color.c"
    "src/misc/lv_color_op.c"
    "src/misc/lv_event.c"
    "src/misc/lv_grad.c"
    "src/misc/lv_iter.c"
    "src/misc/lv_ll.c"
    "src/misc/lv_log.c"
    "src/misc/lv_lru.c"
    "src/misc/lv_math.c"
    "src/misc/lv_matrix.c"
    "src/misc/lv_palette.c"
    "src/misc/lv_pending.c"
    "src/misc/lv_rb.c"
    "src/misc/lv_style.c"
    "src/misc/lv_style_gen.c"
    "src/misc/lv_templ.c"
    "src/misc/lv_text.c"
    "src/misc/lv_text_ap.c"
    "src/misc/lv_timer.c"
    "src/misc/lv_tree.c"
    "src/misc/lv_utils.c"
    "src/misc/cache/lv_cache.c"
    "src/misc/cache/lv_cache_entry.c"

    # Шрифты (оставляем только один шрифт, например 14px)
    "src/font/lv_font.c"
    "src/font/fmt_txt/lv_font_fmt_txt.c"
    "src/font/lv_font_montserrat_14.c"

    # Программный рендер (только заливка прямоугольников и букв)
    "src/draw/lv_draw.c"
    "src/draw/lv_draw_buf.c"
    "src/draw/lv_draw_label.c"
    "src/draw/lv_draw_rect.c"
    "src/draw/sw/lv_draw_sw.c"
    "src/draw/sw/lv_draw_sw_fill.c"
    "src/draw/sw/lv_draw_sw_letter.c"
    "src/draw/sw/lv_draw_sw_utils.c"


    # Единственный виджет
    "src/widgets/label/lv_label.c"
)

# Добавляем файлы смешивания пикселей (blend)
file(GLOB_RECURSE LVGL_BLEND_SOURCES "src/draw/sw/blend/*.c")

idf_component_register(
    SRCS ${LVGL_CORE_SOURCES} ${LVGL_BLEND_SOURCES}
    INCLUDE_DIRS "src"
    REQUIRES esp_timer
)

target_compile_definitions(${COMPONENT_LIB} PUBLIC LV_CONF_INCLUDE_SIMPLE)

