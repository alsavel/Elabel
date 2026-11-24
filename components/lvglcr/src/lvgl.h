/**
 * @file lvgl.h
 * Minimal LVGL 9 header localized in src/
 */

#ifndef LVGL_H
#define LVGL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lv_version.h"
#include "lv_conf_internal.h"

#include "lv_init.h"

#include "stdlib/lv_mem.h"
#include "stdlib/lv_string.h"
#include "stdlib/lv_sprintf.h"

#include "misc/lv_log.h"
#include "misc/lv_timer.h"
#include "misc/lv_math.h"
#include "misc/lv_array.h"
#include "misc/lv_async.h"
#include "misc/lv_rb.h"
#include "misc/lv_utils.h"
//#include "misc/lv_iter.h"
//#include "misc/lv_circle_buf.h"
#include "misc/lv_tree.h"

#include "osal/lv_os.h"
#include "tick/lv_tick.h"

#include "core/lv_obj.h"
#include "core/lv_group.h"
#include "core/lv_refr.h"
//#include "indev/lv_indev.h"
#include "display/lv_display.h"

#include "font/lv_font.h"
#include "font/fmt_txt/lv_font_fmt_txt.h"

#include "widgets/label/lv_label.h"

//#include "layouts/lv_layout.h"

#include "draw/lv_draw_buf.h"
#include "draw/sw/lv_draw_sw_utils.h"

//#include "themes/lv_theme.h"

#if LV_USE_PRIVATE_API
#include "lvgl_private.h"
#endif

#define LV_VERSION_CHECK(x,y,z) (x == LVGL_VERSION_MAJOR && (y < LVGL_VERSION_MINOR || (y == LVGL_VERSION_MINOR && z <= LVGL_VERSION_PATCH)))

static inline int lv_version_major(void) { return LVGL_VERSION_MAJOR; }
static inline int lv_version_minor(void) { return LVGL_VERSION_MINOR; }
static inline int lv_version_patch(void) { return LVGL_VERSION_PATCH; }
static inline const char * lv_version_info(void) { return LVGL_VERSION_INFO; }

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_H*/

