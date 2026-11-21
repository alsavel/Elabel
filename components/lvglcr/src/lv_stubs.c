#include "lvgl_private.h"

/* 1. Инициализация и деинициализация (lv_init.c) */
void lv_fs_init(void) {}
void lv_fs_deinit(void) {}
void lv_layout_init(void) {}
void lv_layout_deinit(void) {}
void lv_anim_core_init(void) {}
void lv_anim_core_deinit(void) {}
void lv_group_init(void) {}
void lv_group_deinit(void) {}
void lv_image_decoder_init(void) {}
void lv_image_decoder_deinit(void) {}
void lv_indev_deinit(void) {}
void lv_theme_deinit(void) {}

/* 2. Темы (lv_display.c, lv_obj_class.c) */
bool lv_theme_simple_is_inited(void) { return true; }
lv_theme_t * lv_theme_simple_init(lv_display_t * disp) { return NULL; }
lv_theme_t * lv_theme_simple_get(void) { return NULL; }
void lv_theme_apply(lv_obj_t * obj) {}

/* 3. Группы и устройства ввода (lv_obj.c, lv_obj_class.c, lv_obj_event.c) */
lv_group_t * lv_group_get_default(void) { return NULL; }
void lv_group_add_obj(lv_group_t * group, lv_obj_t * obj) {}
void lv_group_remove_obj(lv_obj_t * obj) {}
bool lv_group_get_editing(const lv_group_t * group) { return false; }

lv_indev_t * lv_indev_active(void) { return NULL; }
lv_indev_t * lv_indev_get_next(lv_indev_t * indev) { return NULL; }
lv_indev_type_t lv_indev_get_type(const lv_indev_t * indev) { return LV_INDEV_TYPE_NONE; }
//lv_obj_t * lv_indev_get_scroll_obj(lv_indev_t * indev) { return NULL; }
lv_obj_t * lv_indev_get_scroll_obj(const lv_indev_t * indev) { return NULL; }

void lv_indev_read_timer_cb(lv_timer_t * timer) {}

/* 4. Лейауты (lv_obj_pos.c) */
//int32_t lv_layout_get_min_size(lv_obj_t * obj, bool width) { return 0; }
bool lv_layout_get_min_size(lv_obj_t * obj, int32_t * size, bool width) { return false; }

void lv_layout_apply(lv_obj_t * obj) {}
//bool lv_obj_is_layout_positioned(const lv_obj_t * obj) { return false; }

/* 5. Скролл и скроллбары (lv_obj.c, lv_obj_pos.c, lv_display.c, lv_label.c) */
int32_t lv_obj_get_scroll_x(const lv_obj_t * obj) { return 0; }
int32_t lv_obj_get_scroll_y(const lv_obj_t * obj) { return 0; }
int32_t lv_obj_get_scroll_top(const lv_obj_t * obj) { return 0; }
int32_t lv_obj_get_scroll_bottom(const lv_obj_t * obj) { return 0; }
int32_t lv_obj_get_scroll_left(const lv_obj_t * obj) { return 0; }
int32_t lv_obj_get_scroll_right(const lv_obj_t * obj) { return 0; }
lv_dir_t lv_obj_get_scroll_dir(const lv_obj_t * obj) { return LV_DIR_NONE; }
lv_scrollbar_mode_t lv_obj_get_scrollbar_mode(const lv_obj_t * obj) { return LV_SCROLLBAR_MODE_OFF; }
void lv_obj_set_scrollbar_mode(lv_obj_t * obj, lv_scrollbar_mode_t mode) {}
void lv_obj_get_scrollbar_area(lv_obj_t * obj, lv_area_t * hor, lv_area_t * ver)
{
    lv_area_set(hor, 0, 0, -1, -1);
    lv_area_set(ver, 0, 0, -1, -1);
}
void lv_obj_scrollbar_invalidate(lv_obj_t * obj) {}
void lv_obj_readjust_scroll(lv_obj_t * obj, lv_anim_enable_t anim_en) {}
void lv_obj_scroll_to(lv_obj_t * obj, int32_t x, int32_t y, lv_anim_enable_t anim_en) {}
void lv_obj_scroll_to_x(lv_obj_t * obj, int32_t x, lv_anim_enable_t anim_en) {}
void lv_obj_scroll_to_y(lv_obj_t * obj, int32_t y, lv_anim_enable_t anim_en) {}
void lv_obj_scroll_by(lv_obj_t * obj, int32_t x, int32_t y, lv_anim_enable_t anim_en) {}
void lv_obj_scroll_to_view_recursive(lv_obj_t * obj, lv_anim_enable_t anim_en) {}
void lv_obj_update_snap(lv_obj_t * obj, lv_anim_enable_t anim_en) {}

/* 6. Анимации (lv_obj.c, lv_obj_style.c, lv_label.c, lv_refr.c) */
void lv_anim_refr_now(void) {}
//uint32_t lv_anim_count_running(void) { return 0; }
uint16_t lv_anim_count_running(void) { return 0; }

void lv_anim_init(lv_anim_t * a) { lv_memzero(a, sizeof(lv_anim_t)); }
lv_anim_t * lv_anim_start(const lv_anim_t * a) { return NULL; }
lv_anim_t * lv_anim_get(void * var, lv_anim_exec_xcb_t exec_cb) { return NULL; }
bool lv_anim_delete(void * var, lv_anim_exec_xcb_t exec_cb) { return false; }
uint32_t lv_anim_speed_clamped(uint32_t speed, uint32_t min_time, uint32_t max_time) { return 0; }
uint32_t lv_anim_resolve_speed(uint32_t speed_or_time, int32_t start, int32_t end) { return 0; }
void lv_anim_set_var(lv_anim_t * a, void * var) {}
void lv_anim_set_exec_cb(lv_anim_t * a, lv_anim_exec_xcb_t exec_cb) {}
void lv_anim_set_start_cb(lv_anim_t * a, lv_anim_start_cb_t start_cb) {}
void lv_anim_set_completed_cb(lv_anim_t * a, lv_anim_completed_cb_t completed_cb) {}
void lv_anim_set_values(lv_anim_t * a, int32_t start, int32_t end) {}
void lv_anim_set_duration(lv_anim_t * a, uint32_t duration) {}
void lv_anim_set_delay(lv_anim_t * a, uint32_t delay) {}
void lv_anim_set_path_cb(lv_anim_t * a, lv_anim_path_cb_t path_cb) {}
void lv_anim_set_early_apply(lv_anim_t * a, bool en) {}
void lv_anim_set_user_data(lv_anim_t * a, void * user_data) {}
void lv_anim_set_repeat_count(lv_anim_t * a, uint32_t cnt) {}
void lv_anim_set_reverse_delay(lv_anim_t * a, uint32_t delay) {}
void lv_anim_set_repeat_delay(lv_anim_t * a, uint32_t delay) {}
void lv_anim_set_reverse_duration(lv_anim_t * a, uint32_t duration) {}

/* 7. Изображения, маски, слои и примитивы (lv_draw*.c, lv_refr.c, lv_obj.c) */
lv_image_src_t lv_image_src_get_type(const void * src) { return LV_IMAGE_SRC_UNKNOWN; }
lv_result_t lv_image_decoder_get_info(const void * src, lv_image_header_t * header) { return LV_RESULT_INVALID; }
void lv_draw_image_dsc_init(lv_draw_image_dsc_t * dsc) { lv_memzero(dsc, sizeof(lv_draw_image_dsc_t)); }
void lv_draw_layer(lv_layer_t * layer, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords) {}
void lv_draw_mask_rect_dsc_init(lv_draw_mask_rect_dsc_t * dsc) {}
void lv_draw_mask_rect(lv_layer_t * layer, const lv_draw_mask_rect_dsc_t * dsc) {}
void lv_draw_blur_dsc_init(lv_draw_blur_dsc_t * dsc) {}
void lv_draw_blur(lv_layer_t * layer, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords) {}
//void lv_draw_line_iterate(lv_layer_t * layer, const lv_draw_line_dsc_t * dsc, void (*point_cb)(int32_t x, int32_t y, void * user_data), void * user_data) {}
void lv_draw_line_iterate(lv_draw_task_t * t, lv_draw_line_dsc_t * dsc, void (*draw_line_cb)(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc)) {}

/* 8. Программные примитивы (lv_draw_sw.c, lv_draw_sw_letter.c) */
void lv_draw_sw_border(lv_draw_task_t * t, const lv_draw_border_dsc_t * dsc, const lv_area_t * coords) {}
void lv_draw_sw_box_shadow(lv_draw_task_t * t, const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords) {}
void lv_draw_sw_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * coords) {}
void lv_draw_sw_arc(lv_draw_task_t * t, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords) {}
void lv_draw_sw_line(lv_draw_task_t * t, const lv_draw_line_dsc_t * dsc) {}
void lv_draw_sw_triangle(lv_draw_task_t * t, const lv_draw_triangle_dsc_t * dsc) {}
void lv_draw_sw_mask_rect(lv_draw_task_t * t, const lv_draw_mask_rect_dsc_t * dsc) {}
void lv_draw_sw_vector(lv_draw_task_t * t, const lv_draw_vector_dsc_t * dsc) {}
void lv_draw_sw_blur(lv_draw_task_t * t, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords) {}
void lv_draw_sw_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords) {}

