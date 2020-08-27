AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -W  -Wall -g -std=c99

# SUBDIRS =  lib/ .


EXTRA_DIST = README.md LICENSE ./*.h widgets/*.h lib/lvgl.h lib/liblv/* config.ini

# bin
bin_PROGRAMS = antfx
# lib source files
antfx_SOURCES = antfxapp.c  antfx.c
antfx_CPPFLAGS=-I$(srcdir)/lib

if USE_SDL2
antfx_SOURCES += backends/sdl2_engine.c
else
antfx_SOURCES += backends/fb_engine.c
endif

antfx_SOURCES +=    gui.c \
                    conf.c
                    
antfx_SOURCES +=    widgets/image.c \
                    widgets/default_wp.c \
                    widgets/icons/radio.c \
                    widgets/icons/alarm.c \
                    widgets/icons/calendar.c \
                    widgets/icons/camera.c \
                    widgets/fonts/roboto_bold_50.c \
                    widgets/icons/w01d.c \
                    widgets/icons/w01n.c \
                    widgets/icons/w02d.c \
                    widgets/icons/w02n.c \
                    widgets/icons/w03d.c \
                    widgets/icons/w04d.c \
                    widgets/icons/w09d.c \
                    widgets/icons/w10d.c \
                    widgets/icons/w10n.c \
                    widgets/icons/w11d.c \
                    widgets/icons/w13d.c \
                    widgets/icons/w50d.c

antfx_SOURCES += lib/liblv/lv_core/lv_group.c
antfx_SOURCES += lib/liblv/lv_core/lv_indev.c
antfx_SOURCES += lib/liblv/lv_core/lv_disp.c
antfx_SOURCES += lib/liblv/lv_core/lv_obj.c
antfx_SOURCES += lib/liblv/lv_core/lv_refr.c
antfx_SOURCES += lib/liblv/lv_core/lv_style.c
antfx_SOURCES += lib/liblv/lv_core/lv_debug.c

antfx_SOURCES += lib/liblv/lv_draw/lv_draw_basic.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_rect.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_label.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_line.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_img.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_arc.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_triangle.c
antfx_SOURCES += lib/liblv/lv_draw/lv_img_decoder.c
antfx_SOURCES += lib/liblv/lv_draw/lv_img_cache.c

antfx_SOURCES += lib/liblv/lv_font/lv_font.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_fmt_txt.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_roboto_12.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_roboto_16.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_roboto_22.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_roboto_28.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_unscii_8.c

antfx_SOURCES += lib/liblv/lv_hal/lv_hal_disp.c
antfx_SOURCES += lib/liblv/lv_hal/lv_hal_indev.c
antfx_SOURCES += lib/liblv/lv_hal/lv_hal_tick.c


antfx_SOURCES += lib/liblv/lv_misc/lv_circ.c
antfx_SOURCES += lib/liblv/lv_misc/lv_area.c
antfx_SOURCES += lib/liblv/lv_misc/lv_task.c
antfx_SOURCES += lib/liblv/lv_misc/lv_fs.c
antfx_SOURCES += lib/liblv/lv_misc/lv_anim.c
antfx_SOURCES += lib/liblv/lv_misc/lv_mem.c
antfx_SOURCES += lib/liblv/lv_misc/lv_ll.c
antfx_SOURCES += lib/liblv/lv_misc/lv_color.c
antfx_SOURCES += lib/liblv/lv_misc/lv_txt.c
antfx_SOURCES += lib/liblv/lv_misc/lv_math.c
antfx_SOURCES += lib/liblv/lv_misc/lv_log.c
antfx_SOURCES += lib/liblv/lv_misc/lv_gc.c
antfx_SOURCES += lib/liblv/lv_misc/lv_utils.c
antfx_SOURCES += lib/liblv/lv_misc/lv_async.c
antfx_SOURCES += lib/liblv/lv_misc/lv_printf.c
antfx_SOURCES += lib/liblv/lv_misc/lv_bidi.c

antfx_SOURCES += lib/liblv/lv_objx/lv_arc.c
antfx_SOURCES += lib/liblv/lv_objx/lv_bar.c
antfx_SOURCES += lib/liblv/lv_objx/lv_cb.c
antfx_SOURCES += lib/liblv/lv_objx/lv_cpicker.c
antfx_SOURCES += lib/liblv/lv_objx/lv_ddlist.c
antfx_SOURCES += lib/liblv/lv_objx/lv_kb.c
antfx_SOURCES += lib/liblv/lv_objx/lv_line.c
antfx_SOURCES += lib/liblv/lv_objx/lv_mbox.c
antfx_SOURCES += lib/liblv/lv_objx/lv_preload.c
antfx_SOURCES += lib/liblv/lv_objx/lv_roller.c
antfx_SOURCES += lib/liblv/lv_objx/lv_table.c
antfx_SOURCES += lib/liblv/lv_objx/lv_tabview.c
antfx_SOURCES += lib/liblv/lv_objx/lv_tileview.c
antfx_SOURCES += lib/liblv/lv_objx/lv_btn.c
antfx_SOURCES += lib/liblv/lv_objx/lv_calendar.c
antfx_SOURCES += lib/liblv/lv_objx/lv_chart.c
antfx_SOURCES += lib/liblv/lv_objx/lv_canvas.c
antfx_SOURCES += lib/liblv/lv_objx/lv_gauge.c
antfx_SOURCES += lib/liblv/lv_objx/lv_label.c
antfx_SOURCES += lib/liblv/lv_objx/lv_list.c
antfx_SOURCES += lib/liblv/lv_objx/lv_slider.c
antfx_SOURCES += lib/liblv/lv_objx/lv_ta.c
antfx_SOURCES += lib/liblv/lv_objx/lv_spinbox.c
antfx_SOURCES += lib/liblv/lv_objx/lv_btnm.c
antfx_SOURCES += lib/liblv/lv_objx/lv_cont.c
antfx_SOURCES += lib/liblv/lv_objx/lv_img.c
antfx_SOURCES += lib/liblv/lv_objx/lv_imgbtn.c
antfx_SOURCES += lib/liblv/lv_objx/lv_led.c
antfx_SOURCES += lib/liblv/lv_objx/lv_lmeter.c
antfx_SOURCES += lib/liblv/lv_objx/lv_page.c
antfx_SOURCES += lib/liblv/lv_objx/lv_sw.c
antfx_SOURCES += lib/liblv/lv_objx/lv_win.c

antfx_SOURCES += lib/liblv/lv_themes/lv_theme_alien.c
antfx_SOURCES += lib/liblv/lv_themes/lv_theme.c
antfx_SOURCES += lib/liblv/lv_themes/lv_theme_default.c
antfx_SOURCES += lib/liblv/lv_themes/lv_theme_night.c
antfx_SOURCES += lib/liblv/lv_themes/lv_theme_templ.c
antfx_SOURCES += lib/liblv/lv_themes/lv_theme_zen.c
antfx_SOURCES += lib/liblv/lv_themes/lv_theme_material.c
antfx_SOURCES += lib/liblv/lv_themes/lv_theme_nemo.c
antfx_SOURCES += lib/liblv/lv_themes/lv_theme_mono.c