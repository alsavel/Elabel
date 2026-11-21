/**
 * @file lvgl_private.h
 * Minimal private headers localized in src/
 */

#ifndef LVGL_PRIVATE_H
#define LVGL_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

#include "core/lv_obj_private.h"
#include "core/lv_obj_class_private.h"
#include "core/lv_obj_draw_private.h"
#include "core/lv_obj_event_private.h"
#include "core/lv_obj_pos.h"
#include "core/lv_obj_scroll_private.h"
#include "core/lv_obj_style_private.h"
#include "core/lv_group_private.h"
#include "core/lv_refr_private.h"
#include "core/lv_global.h"

#include "display/lv_display_private.h"
#include "indev/lv_indev_private.h"

#include "draw/lv_draw_private.h"
#include "draw/lv_draw_buf_private.h"
#include "draw/lv_draw_label_private.h"
#include "draw/lv_draw_rect_private.h"
#include "draw/sw/lv_draw_sw_private.h"
#include "draw/sw/lv_draw_sw_mask_private.h"

#include "font/fmt_txt/lv_font_fmt_txt_private.h"

#include "misc/lv_area_private.h"
#include "misc/lv_event_private.h"
#include "misc/lv_fs_private.h"
#include "misc/lv_rb_private.h"
#include "misc/lv_style_private.h"
#include "misc/lv_text_private.h"
#include "misc/lv_timer_private.h"
#include "misc/cache/lv_cache_private.h"
#include "misc/cache/lv_cache_entry_private.h"

#include "osal/lv_os_private.h"
#include "stdlib/lv_mem_private.h"
#include "themes/lv_theme_private.h"
#include "tick/lv_tick_private.h"

#include "widgets/label/lv_label_private.h"

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRIVATE_H*/

