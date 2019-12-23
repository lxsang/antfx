/**
 * @file lvgl.h
 * Include all LittleV GL related headers
 */

#ifndef LVGL_H
#define LVGL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "liblv/lv_version.h"

#include "liblv/lv_misc/lv_log.h"
#include "liblv/lv_misc/lv_task.h"
#include "liblv/lv_misc/lv_math.h"
#include "liblv/lv_misc/lv_async.h"

#include "liblv/lv_hal/lv_hal.h"

#include "liblv/lv_core/lv_obj.h"
#include "liblv/lv_core/lv_group.h"
#include "liblv/lv_core/lv_indev.h"

#include "liblv/lv_core/lv_refr.h"
#include "liblv/lv_core/lv_disp.h"
#include "liblv/lv_core/lv_debug.h"

#include "liblv/lv_themes/lv_theme.h"

#include "liblv/lv_font/lv_font.h"
#include "liblv/lv_font/lv_font_fmt_txt.h"
#include "liblv/lv_misc/lv_bidi.h"
#include "liblv/lv_misc/lv_printf.h"

#include "liblv/lv_objx/lv_btn.h"
#include "liblv/lv_objx/lv_imgbtn.h"
#include "liblv/lv_objx/lv_img.h"
#include "liblv/lv_objx/lv_label.h"
#include "liblv/lv_objx/lv_line.h"
#include "liblv/lv_objx/lv_page.h"
#include "liblv/lv_objx/lv_cont.h"
#include "liblv/lv_objx/lv_list.h"
#include "liblv/lv_objx/lv_chart.h"
#include "liblv/lv_objx/lv_table.h"
#include "liblv/lv_objx/lv_cb.h"
#include "liblv/lv_objx/lv_cpicker.h"
#include "liblv/lv_objx/lv_bar.h"
#include "liblv/lv_objx/lv_slider.h"
#include "liblv/lv_objx/lv_led.h"
#include "liblv/lv_objx/lv_btnm.h"
#include "liblv/lv_objx/lv_kb.h"
#include "liblv/lv_objx/lv_ddlist.h"
#include "liblv/lv_objx/lv_roller.h"
#include "liblv/lv_objx/lv_ta.h"
#include "liblv/lv_objx/lv_canvas.h"
#include "liblv/lv_objx/lv_win.h"
#include "liblv/lv_objx/lv_tabview.h"
#include "liblv/lv_objx/lv_tileview.h"
#include "liblv/lv_objx/lv_mbox.h"
#include "liblv/lv_objx/lv_gauge.h"
#include "liblv/lv_objx/lv_lmeter.h"
#include "liblv/lv_objx/lv_sw.h"
#include "liblv/lv_objx/lv_kb.h"
#include "liblv/lv_objx/lv_arc.h"
#include "liblv/lv_objx/lv_preload.h"
#include "liblv/lv_objx/lv_calendar.h"
#include "liblv/lv_objx/lv_spinbox.h"

#include "liblv/lv_draw/lv_img_cache.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
}
#endif

#endif /*LVGL_H*/
