AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -W  -Wall -g -std=c99
# SUBDIRS =  lib/ .


EXTRA_DIST =    README.md \
                LICENSE \
                ./*.h \
                widgets/*.h \
                lib/lvgl.h \
                lib/liblv/* \
                config.ini \
                lib/ini/ini.h

# bin
bin_PROGRAMS = antfx

# lib source files
antfx_SOURCES = antfxapp.c  antfx.c
antfx_CPPFLAGS=-I$(srcdir)/lib -W  -Wall -g -std=c99

if USE_SDL2
antfx_SOURCES += backends/sdl2_engine.c
else
antfx_SOURCES += backends/fb_engine.c
endif

antfx_SOURCES +=    gui.c \
                    conf.c \
                    db.c \
                    utils.c \
                    hw.c \
                    media.c \
                    audio.c \
                    weather.c \
                    bst.c \
                    lib/ini/ini.c

antfx_SOURCES +=    widgets/image.c \
                    widgets/default_wp.c \
                    widgets/icons/radio.c \
                    widgets/icons/alarm.c \
                    widgets/icons/calendar.c \
                    widgets/icons/camera.c \
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

antfx_SOURCES += antfx_SOURCES += lib/liblv/lv_themes/lv_theme_template.c
antfx_SOURCES += lib/liblv/lv_themes/lv_theme_mono.c
antfx_SOURCES += lib/liblv/lv_themes/lv_theme.c
antfx_SOURCES += lib/liblv/lv_themes/lv_theme_empty.c
antfx_SOURCES += lib/liblv/lv_themes/lv_theme_material.c
antfx_SOURCES += lib/liblv/lv_core/lv_disp.c
antfx_SOURCES += lib/liblv/lv_core/lv_refr.c
antfx_SOURCES += lib/liblv/lv_core/lv_indev.c
antfx_SOURCES += lib/liblv/lv_core/lv_style.c
antfx_SOURCES += lib/liblv/lv_core/lv_obj.c
antfx_SOURCES += lib/liblv/lv_core/lv_group.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_arc.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_page.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_win.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_objmask.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_label.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_canvas.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_cont.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_msgbox.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_led.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_line.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_roller.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_switch.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_linemeter.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_tileview.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_spinbox.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_objx_templ.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_calendar.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_img.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_textarea.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_bar.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_cpicker.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_chart.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_checkbox.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_gauge.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_imgbtn.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_dropdown.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_spinner.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_btn.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_list.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_slider.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_tabview.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_table.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_keyboard.c
antfx_SOURCES += lib/liblv/lv_widgets/lv_btnmatrix.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_18.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_24.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_16.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_36.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_30.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_12_subpx.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_unscii_8.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_loader.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_28.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_22.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_40.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_48.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_34.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_32.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_28_compressed.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_20.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_46.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_14.c
antfx_SOURCES += lib/liblv/lv_font/lv_font.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_42.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_simsun_16_cjk.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_dejavu_16_persian_hebrew.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_12.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_44.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_38.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_montserrat_26.c
antfx_SOURCES += lib/liblv/lv_font/lv_font_fmt_txt.c
antfx_SOURCES += lib/liblv/lv_misc/lv_txt_ap.c
antfx_SOURCES += lib/liblv/lv_misc/lv_task.c
antfx_SOURCES += lib/liblv/lv_misc/lv_bidi.c
antfx_SOURCES += lib/liblv/lv_misc/lv_math.c
antfx_SOURCES += lib/liblv/lv_misc/lv_anim.c
antfx_SOURCES += lib/liblv/lv_misc/lv_async.c
antfx_SOURCES += lib/liblv/lv_misc/lv_mem.c
antfx_SOURCES += lib/liblv/lv_misc/lv_templ.c
antfx_SOURCES += lib/liblv/lv_misc/lv_printf.c
antfx_SOURCES += lib/liblv/lv_misc/lv_gc.c
antfx_SOURCES += lib/liblv/lv_misc/lv_fs.c
antfx_SOURCES += lib/liblv/lv_misc/lv_debug.c
antfx_SOURCES += lib/liblv/lv_misc/lv_txt.c
antfx_SOURCES += lib/liblv/lv_misc/lv_area.c
antfx_SOURCES += lib/liblv/lv_misc/lv_color.c
antfx_SOURCES += lib/liblv/lv_misc/lv_utils.c
antfx_SOURCES += lib/liblv/lv_misc/lv_log.c
antfx_SOURCES += lib/liblv/lv_misc/lv_ll.c
antfx_SOURCES += lib/liblv/lv_draw/lv_img_buf.c
antfx_SOURCES += lib/liblv/lv_draw/lv_img_decoder.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_triangle.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_blend.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_img.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_rect.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_line.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_label.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_mask.c
antfx_SOURCES += lib/liblv/lv_draw/lv_draw_arc.c
antfx_SOURCES += lib/liblv/lv_draw/lv_img_cache.c
antfx_SOURCES += lib/liblv/lv_hal/lv_hal_disp.c
antfx_SOURCES += lib/liblv/lv_hal/lv_hal_indev.c
antfx_SOURCES += lib/liblv/lv_hal/lv_hal_tick.c
antfx_SOURCES += lib/liblv/lv_gpu/lv_gpu_stm32_dma2d.c