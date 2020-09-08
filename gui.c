#include <time.h>
#include "gui.h"
#include "db.h"
#include "hw.h"
#include "media.h"
#include "utils.h"

typedef struct
{
    char field_1[ANTFX_MAX_STR_BUFF_SZ];
    char field_2[ANTFX_MAX_STR_BUFF_SZ];
    char field_3[ANTFX_MAX_STR_BUFF_SZ];
    char field_4[ANTFX_MAX_STR_BUFF_SZ];
    char field_5[ANTFX_MAX_STR_BUFF_SZ];
} antfx_ui_form_data_t;

typedef struct
{
    lv_obj_t *lbl_time;
    lv_obj_t *lbl_date;
    lv_obj_t *lbl_weather;
    lv_obj_t *lbl_status;
    lv_obj_t *lbl_weather_img;
    lv_obj_t *keyboard;
    lv_obj_t *bt_play_pause;
    lv_obj_t *bt_shuffle;
    lv_obj_t *progress;
    antfx_ui_form_data_t fields;
} antfx_screen_info_t;

LV_IMG_DECLARE(default_wp);
LV_IMG_DECLARE(radio);
LV_IMG_DECLARE(alarm_clock);
LV_IMG_DECLARE(calendar);
LV_IMG_DECLARE(camera);
LV_IMG_DECLARE(w01d);
LV_IMG_DECLARE(w01n);
LV_IMG_DECLARE(w02d);
LV_IMG_DECLARE(w02n);
LV_IMG_DECLARE(w03d);
LV_IMG_DECLARE(w04d);
LV_IMG_DECLARE(w09d);
LV_IMG_DECLARE(w10n);
LV_IMG_DECLARE(w10d);
LV_IMG_DECLARE(w11d);
LV_IMG_DECLARE(w13d);
LV_IMG_DECLARE(w50d);
LV_FONT_DECLARE(roboto_bold_50);

static antfx_screen_info_t g_scr_info;

static void antfx_ui_show_calendar(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_close_popup(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_show_fm_dialog(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_show_music_dialog(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_music_play(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_music_stop(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_add_fm_channel_popup(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_fav_setting(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_add_fm_channel(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_attach_keyboard(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_attach_numpad(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_fm_list_refresh(lv_obj_t *list);
static void antfx_ui_music_list_refresh(lv_obj_t *list);
static void antfx_ui_fm_list_add(antfx_fm_record_t *r, void *list);
static void antfx_ui_fm_item_action(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_fm_item_action_cb(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_fm_mute_cb(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_fav_setting_save(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_alert(const char *);
static void antfx_ui_set_volume(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_set_volume_cb(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_music_shuffle_cb(lv_obj_t *obj, lv_event_t event);
static void antfx_media_music_next_page(lv_obj_t *obj, lv_event_t event);
static void antfx_media_music_prev_page(lv_obj_t *obj, lv_event_t event);

void antfx_ui_update()
{
    time_t rawtime;
    struct tm *timeinfo;

    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    lv_label_set_text(g_scr_info.lbl_time, buffer);
    strftime(buffer, sizeof(buffer), "%a %b %Y", timeinfo);
    lv_label_set_text(g_scr_info.lbl_date, buffer);
    antfx_conf_t *conf = antfx_get_config();
    // update play back
    lv_obj_t *img = lv_list_get_btn_img(g_scr_info.bt_play_pause);
    if (conf->media.music.status == MUSIC_STOP)
    {
        lv_bar_set_value(g_scr_info.progress, 0, LV_ANIM_OFF);
        lv_img_set_src(img, LV_SYMBOL_PLAY);
        if (conf->media.mode != M_FM_MODE)
            antfx_ui_update_status("");
    }
    else if (conf->media.music.status == MUSIC_PAUSE)
    {
        lv_img_set_src(img, LV_SYMBOL_PLAY);
    }
    else if (conf->media.music.status == MUSIC_PLAYING)
    {
        int percent = (int)conf->media.music.current_frame * 100 / conf->media.music.total_frames;
        lv_bar_set_value(g_scr_info.progress, percent, LV_ANIM_OFF);
        lv_img_set_src(img, LV_SYMBOL_PAUSE);
    }
    else
    {
        lv_img_set_src(img, LV_SYMBOL_PLAY);
    }
    // update weather
    if (conf->weather.update)
    {
        antfx_ui_update_weather(conf->weather.desc);
        antfx_ui_update_weather_icon(conf->weather.icon);
        conf->weather.update = 0;
    }
}
void antfx_ui_update_weather(const char *text)
{
    lv_label_set_text(g_scr_info.lbl_weather, text);
}
void antfx_ui_update_weather_icon(const char *x_text)
{
    if (strcmp(x_text, "01d") == 0)
    {
        lv_img_set_src(g_scr_info.lbl_weather_img, &w01d);
    }
    else if (strcmp(x_text, "01n") == 0)
    {
        lv_img_set_src(g_scr_info.lbl_weather_img, &w01n);
    }
    else if (strcmp(x_text, "02d") == 0)
    {
        lv_img_set_src(g_scr_info.lbl_weather_img, &w02d);
    }
    else if (strcmp(x_text, "02n") == 0)
    {
        lv_img_set_src(g_scr_info.lbl_weather_img, &w02n);
    }
    else if (strcmp(x_text, "03d") == 0 || strcmp(x_text, "03n") == 0)
    {
        lv_img_set_src(g_scr_info.lbl_weather_img, &w03d);
    }
    else if (strcmp(x_text, "04d") == 0 || strcmp(x_text, "04n") == 0)
    {
        lv_img_set_src(g_scr_info.lbl_weather_img, &w04d);
    }
    else if (strcmp(x_text, "09d") == 0 || strcmp(x_text, "09n") == 0)
    {
        lv_img_set_src(g_scr_info.lbl_weather_img, &w09d);
    }
    else if (strcmp(x_text, "10n") == 0)
    {
        lv_img_set_src(g_scr_info.lbl_weather_img, &w10n);
    }
    else if (strcmp(x_text, "10d") == 0)
    {
        lv_img_set_src(g_scr_info.lbl_weather_img, &w10d);
    }
    else if (strcmp(x_text, "11d") == 0 || strcmp(x_text, "11n") == 0)
    {
        lv_img_set_src(g_scr_info.lbl_weather_img, &w11d);
    }
    else if (strcmp(x_text, "13d") == 0 || strcmp(x_text, "13n") == 0)
    {
        lv_img_set_src(g_scr_info.lbl_weather_img, &w13d);
    }
    else if (strcmp(x_text, "50d") == 0 || strcmp(x_text, "50n") == 0)
    {
        lv_img_set_src(g_scr_info.lbl_weather_img, &w50d);
    }
}

void antfx_ui_update_status(const char *text)
{
    lv_label_set_text(g_scr_info.lbl_status, text);
}

void antfx_ui_main(engine_config_t conf)
{
    antfx_init(conf);
    antfx_conf_t *config = antfx_get_config();

    static lv_style_t time_style;
    static lv_style_t status_style;
    static lv_style_t date_style;
    static lv_style_t list_style;
    static lv_style_t control_bar_style;
    static lv_style_t btn_style;
    static lv_style_t cont_style;

    lv_style_init(&cont_style);
    lv_style_init(&time_style);
    lv_style_init(&status_style);
    lv_style_init(&date_style);
    lv_style_init(&list_style);
    lv_style_init(&btn_style);
    lv_style_init(&control_bar_style);

    lv_style_set_text_font(&time_style, LV_STATE_DEFAULT, &lv_font_montserrat_46);
    lv_style_set_margin_all(&time_style, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_all(&time_style, LV_STATE_DEFAULT, 0);

    lv_style_set_text_font(&date_style, LV_STATE_DEFAULT, &lv_font_montserrat_26);
    lv_style_set_margin_all(&date_style, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_all(&date_style, LV_STATE_DEFAULT, 0);

    lv_style_set_text_font(&status_style, LV_STATE_DEFAULT, &lv_font_montserrat_18);
    lv_style_set_margin_top(&status_style, LV_STATE_DEFAULT, -7);
    lv_style_set_pad_all(&status_style, LV_STATE_DEFAULT, 0);

    lv_style_set_bg_opa(&cont_style, LV_STATE_DEFAULT, LV_OPA_60);
    lv_style_set_radius(&cont_style, LV_STATE_DEFAULT, 20);
    lv_style_set_pad_all(&cont_style, LV_STATE_DEFAULT, 10);

    lv_style_set_bg_opa(&list_style, LV_STATE_DEFAULT, LV_OPA_60);

    lv_style_set_bg_opa(&btn_style, LV_STATE_DEFAULT, LV_OPA_0);

    lv_style_set_bg_opa(&control_bar_style, LV_STATE_DEFAULT, LV_OPA_100);

    lv_obj_t *scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);

    lv_obj_t *img = lv_img_create(scr, NULL);
    lv_img_set_src(img, &default_wp);

    lv_obj_t *list = lv_list_create(scr, NULL);
    lv_obj_add_style(list, LV_LIST_PART_BG, &list_style);

    lv_obj_set_size(list, 48, lv_disp_get_ver_res(NULL));

    lv_obj_t *btn;
    btn = lv_list_add_btn(list, &radio, NULL);
    lv_obj_set_event_cb(btn, antfx_ui_show_fm_dialog);
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &btn_style);

    btn = lv_list_add_btn(list, LV_SYMBOL_AUDIO, NULL);
    lv_obj_set_event_cb(btn, antfx_ui_show_music_dialog);
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &btn_style);

    btn = lv_list_add_btn(list, &alarm_clock, NULL);
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &btn_style);

    btn = lv_list_add_btn(list, &calendar, NULL);
    lv_obj_set_event_cb(btn, antfx_ui_show_calendar);
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &btn_style);

    btn = lv_list_add_btn(list, &camera, NULL);
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &btn_style);

    btn = lv_list_add_btn(list, LV_SYMBOL_SETTINGS, NULL);
    lv_obj_set_event_cb(btn, antfx_ui_fav_setting);
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &btn_style);

    lv_obj_t *cont = lv_cont_create(scr, NULL);
    lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_MID);
    lv_cont_set_fit(cont, LV_FIT_TIGHT);
    lv_obj_set_pos(cont, lv_disp_get_hor_res(NULL) / 2 - 30, 10);
    lv_obj_set_size(cont, (lv_disp_get_hor_res(NULL) / 2) + 20, lv_disp_get_ver_res(NULL) - 20);

    g_scr_info.lbl_time = lv_label_create(cont, NULL);
    lv_label_set_align(g_scr_info.lbl_time, LV_LABEL_ALIGN_CENTER);
    lv_obj_add_style(g_scr_info.lbl_time, LV_LABEL_PART_MAIN, &time_style);

    g_scr_info.lbl_date = lv_label_create(cont, NULL);
    lv_obj_add_style(g_scr_info.lbl_date, LV_LABEL_PART_MAIN, &date_style);

    lv_obj_add_style(cont, LV_CONT_PART_MAIN, &cont_style);

    g_scr_info.lbl_weather_img = lv_img_create(cont, NULL);
    lv_img_set_src(g_scr_info.lbl_weather_img, LV_SYMBOL_DUMMY);
    lv_obj_add_style(g_scr_info.lbl_weather_img, LV_IMG_PART_MAIN, &status_style);

    g_scr_info.lbl_weather = lv_label_create(cont, NULL);
    lv_label_set_text(g_scr_info.lbl_weather, "");
    lv_obj_add_style(g_scr_info.lbl_weather, LV_LABEL_PART_MAIN, &status_style);

    g_scr_info.lbl_status = lv_label_create(cont, NULL);
    lv_label_set_long_mode(g_scr_info.lbl_status, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(g_scr_info.lbl_status, (lv_disp_get_hor_res(NULL) / 2));
    lv_label_set_align(g_scr_info.lbl_status, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(g_scr_info.lbl_status, "");
    lv_obj_add_style(g_scr_info.lbl_status, LV_LABEL_PART_MAIN, &status_style);

    list = lv_list_create(cont, NULL);
    lv_obj_add_style(list, LV_LIST_PART_BG, &control_bar_style);

    lv_list_set_layout(list, LV_LAYOUT_ROW_MID);
    lv_obj_set_width(list, (lv_disp_get_hor_res(NULL) / 2));

    btn = lv_list_add_btn(list, LV_SYMBOL_PREV, "");
    lv_obj_set_width(btn, 40);

    g_scr_info.bt_play_pause = lv_list_add_btn(list, LV_SYMBOL_PLAY, "");
    lv_obj_set_width(g_scr_info.bt_play_pause, 40);
    lv_obj_set_event_cb(g_scr_info.bt_play_pause, antfx_ui_music_play);

    btn = lv_list_add_btn(list, LV_SYMBOL_STOP, "");
    lv_obj_set_width(btn, 40);
    lv_obj_set_event_cb(btn, antfx_ui_music_stop);

    btn = lv_list_add_btn(list, LV_SYMBOL_NEXT, "");
    lv_obj_set_width(btn, 40);

    g_scr_info.bt_shuffle = lv_list_add_btn(list, LV_SYMBOL_SHUFFLE, "");
    lv_obj_set_width(g_scr_info.bt_shuffle, 40);
    lv_btn_set_checkable(g_scr_info.bt_shuffle, true);
    if (config->fav.shuffle)
        lv_btn_set_state(g_scr_info.bt_shuffle, LV_BTN_STATE_CHECKED_RELEASED);
    lv_obj_set_event_cb(g_scr_info.bt_shuffle, antfx_ui_music_shuffle_cb);

    btn = lv_list_add_btn(list, LV_SYMBOL_VOLUME_MID, "");
    lv_obj_set_event_cb(btn, antfx_ui_set_volume);
    lv_obj_add_style(btn, LV_BTN_PART_MAIN, &btn_style);
    lv_obj_set_width(btn, 40);

    g_scr_info.progress = lv_bar_create(cont, NULL);
    lv_obj_set_size(g_scr_info.progress, (lv_disp_get_hor_res(NULL) / 2), 7);
    lv_bar_set_value(g_scr_info.progress, 0, LV_ANIM_OFF);
    lv_obj_add_style(g_scr_info.progress, LV_BAR_PART_BG, &status_style);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void antfx_ui_set_volume_cb(lv_obj_t *obj, lv_event_t event)
{
    antfx_conf_t *conf = antfx_get_config();
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        /*if (lv_slider_is_dragged(obj))
        {
            return;
        }*/
        int vol = lv_slider_get_value(obj);
        if (lv_obj_get_user_data(obj) == conf->audio.session.input_stream)
        {
            if (antfx_audio_set_input_volume(vol) == -1)
            {
                antfx_ui_alert("Unable to set input volume");
                lv_slider_set_value(obj, conf->fav.input_volume, LV_ANIM_OFF);
            }
            else
            {
                conf->fav.input_volume = vol;
            }
        }
        else if (lv_obj_get_user_data(obj) == conf->audio.session.output_stream)
        {
            if (antfx_audio_set_output_volume(vol) == -1)
            {
                antfx_ui_alert("Unable to set output volume");
                lv_slider_set_value(obj, conf->fav.output_volume, LV_ANIM_OFF);
            }
            else
            {
                conf->fav.output_volume = vol;
            }
        }
    }
}
static void antfx_ui_set_volume(lv_obj_t *obj, lv_event_t event)
{
    UNUSED(obj);
    if (event == LV_EVENT_RELEASED)
    {
        lv_obj_t *scr = lv_disp_get_scr_act(NULL);
        antfx_conf_t *conf = antfx_get_config();
        lv_obj_t *win = lv_win_create(scr, NULL);
        lv_obj_t *btn, *lbl, *sl, *cont;
        btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);
        lv_obj_set_event_cb(btn, lv_win_close_event_cb);
        lv_obj_set_size(win, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
        //lv_obj_set_pos(win, lv_disp_get_hor_res(NULL) / 4, lv_disp_get_ver_res(NULL) / 4 - 10);
        lv_win_set_scrollbar_mode(win, LV_SCROLLBAR_MODE_OFF);
        lv_win_set_title(win, "Volumes");

        cont = lv_cont_create(win, NULL);
        lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_LEFT);
        lv_cont_set_fit(cont, LV_FIT_PARENT);
        //lv_obj_set_opa(lv_obj_get_style(cont), LV_OPA_100);

        lbl = lv_label_create(cont, NULL);
        lv_label_set_text(lbl, "Output volume");

        sl = lv_slider_create(cont, NULL);
        //lv_obj_set_size(sl, lv_disp_get_hor_res(NULL) / 2 - 20, 5);
        lv_obj_set_width(sl, lv_disp_get_hor_res(NULL) - 70);
        lv_obj_set_user_data(sl, conf->audio.session.output_stream);
        lv_slider_set_anim_time(sl, 0);
        lv_obj_set_event_cb(sl, antfx_ui_set_volume_cb);
        lv_slider_set_value(sl, conf->fav.output_volume, LV_ANIM_OFF);

        lbl = lv_label_create(cont, NULL);
        lv_label_set_text(lbl, "Input volume");

        sl = lv_slider_create(cont, NULL);
        //lv_obj_set_size(sl, lv_disp_get_hor_res(NULL) / 2 - 20, 10);
        lv_obj_set_width(sl, lv_disp_get_hor_res(NULL) - 70);
        lv_slider_set_anim_time(sl, 0);
        lv_obj_set_user_data(sl, conf->audio.session.input_stream);
        lv_obj_set_event_cb(sl, antfx_ui_set_volume_cb);
        lv_slider_set_value(sl, conf->fav.input_volume, LV_ANIM_OFF);
    }
}
static void antfx_ui_music_shuffle_cb(lv_obj_t *obj, lv_event_t event)
{
    UNUSED(obj);
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        antfx_conf_t *conf = antfx_get_config();
        conf->fav.shuffle = !conf->fav.shuffle;
    }
}
static void antfx_ui_music_stop(lv_obj_t *obj, lv_event_t event)
{
    UNUSED(obj);
    if (event == LV_EVENT_RELEASED)
    {
        antfx_media_music_stop();
    }
}
static void antfx_ui_music_play(lv_obj_t *obj, lv_event_t event)
{
    char buff[ANTFX_MAX_STR_BUFF_SZ];
    if (event == LV_EVENT_RELEASED)
    {
        antfx_conf_t *conf = antfx_get_config();
        const char *f_name = lv_list_get_btn_text(obj);
        snprintf(buff, sizeof(buff), "%s/%s", conf->fav.music_path, f_name);
        antfx_media_fm_stop();
        if (strcmp(f_name, "") == 0)
        {
            if (conf->media.music.status == MUSIC_PLAYING)
            {
                antfx_media_music_pause();
            }
            else if (conf->media.music.status == MUSIC_PAUSE)
            {
                antfx_media_music_resume();
            }
        }
        else
        {
            antfx_ui_update_status("");
            if (antfx_media_music_play(buff) == 0)
            {
                antfx_ui_update_status(f_name);
            }
        }
    }
}
static void antfx_ui_fm_mute_cb(lv_obj_t *obj, lv_event_t event)
{
    UNUSED(obj);
    if (event == LV_EVENT_RELEASED)
    {
        antfx_media_fm_stop();
        antfx_ui_update_status("");
    }
}
static void antfx_ui_fm_list_add(antfx_fm_record_t *r, void *list)
{
    char buff[ANTFX_MAX_STR_BUFF_SZ];
    snprintf(buff, ANTFX_MAX_STR_BUFF_SZ, "%s : %.2f Mhz", r->name, r->freq);
    lv_obj_t *btn = lv_list_add_btn((lv_obj_t *)list, &radio, buff);
    r->user_data = (void *)list;
    lv_obj_set_user_data(btn, r);
    lv_obj_set_event_cb(btn, antfx_ui_fm_item_action);
}

static void antfx_ui_fm_item_action(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        /*Create a Message box*/
        static const char *mbox_btn_map[] = {"Play", "Delete", "Close", ""};
        lv_obj_t *mbox = lv_msgbox_create(lv_scr_act(), NULL);
        lv_msgbox_set_text(mbox, "Choose action:");
        lv_msgbox_add_btns(mbox, mbox_btn_map);
        lv_obj_set_event_cb(mbox, antfx_ui_fm_item_action_cb);
        lv_obj_set_user_data(mbox, lv_obj_get_user_data(obj));
        lv_obj_set_top(mbox, true);
    }
}
static void antfx_ui_fm_item_action_cb(lv_obj_t *obj, lv_event_t event)
{
    char buff[32];
    antfx_fm_record_t *r = NULL;
    const char *action = NULL;
    lv_obj_t *list = NULL;
    antfx_conf_t *conf = antfx_get_config();
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        r = (antfx_fm_record_t *)lv_obj_get_user_data(obj);
        action = lv_msgbox_get_active_btn_text(obj);
        if (strcmp(action, "Play") == 0)
        {
            antfx_media_music_stop();
            antfx_media_fm_start(r->freq, conf->fav.input);
            snprintf(buff, 32, "FM: %.2f Mhz", r->freq);
            antfx_ui_update_status(buff);
        }
        else if (strcmp(action, "Delete") == 0)
        {
            list = (lv_obj_t *)r->user_data;
            if (antfx_db_rm_fm_channel(r) == -1)
            {
                antfx_ui_alert("Unable to delete channel");
            }
            else
            {
                antfx_ui_fm_list_refresh(list);
            }
        }
        lv_msgbox_start_auto_close(obj, 0);
    }
}
static void antfx_media_music_next_page(lv_obj_t *obj, lv_event_t event)
{
    lv_obj_t *win;
    antfx_conf_t *conf;
    int total_page;
    if (event == LV_EVENT_CLICKED)
    {
        conf = antfx_get_config();
        win = (lv_obj_t *)lv_obj_get_user_data(obj);
        total_page = conf->media.music.total_songs / 6;
        if (conf->media.music.total_songs % 6 > 0)
            total_page++;
        if (conf->media.music.current_page < total_page - 1)
        {
            conf->media.music.current_page++;
            lv_obj_del(win);
            antfx_ui_show_music_dialog(NULL, LV_EVENT_CLICKED);
        }
    }
}
static void antfx_media_music_prev_page(lv_obj_t *obj, lv_event_t event)
{
    lv_obj_t *win;
    antfx_conf_t *conf;
    int total_page;
    if (event == LV_EVENT_CLICKED)
    {
        conf = antfx_get_config();
        win = (lv_obj_t *)lv_obj_get_user_data(obj);
        total_page = conf->media.music.total_songs / 6;
        if (conf->media.music.total_songs % 6 > 0)
            total_page++;
        if (conf->media.music.current_page > 0)
        {
            conf->media.music.current_page--;
            lv_obj_del(win);
            antfx_ui_show_music_dialog(NULL, LV_EVENT_CLICKED);
        }
    }
}
static void antfx_ui_music_list_refresh(lv_obj_t *list)
{
    antfx_conf_t *conf = antfx_get_config();
    bst_node_t *node;
    lv_obj_t *btn;
    lv_list_clean(list);
    if (conf->media.music.songs != NULL)
    {
        int pos = conf->media.music.current_page * 6;
        for (int i = pos; i < pos + 6; i++)
        {
            node = bst_find(conf->media.music.songs, i);
            if (node && node->data)
            {
                btn = lv_list_add_btn(list, LV_SYMBOL_AUDIO, (char *)node->data);
                lv_obj_set_event_cb(btn, antfx_ui_music_play);
            }
        }
    }
    else
    {
        antfx_ui_alert("Playlist is empty");
    }
}
static void antfx_ui_fm_list_refresh(lv_obj_t *list)
{
    lv_list_clean(list);
    int ret = antfx_db_fetch_fm_channels(antfx_ui_fm_list_add, (void *)list);
    if (ret == -1)
    {
        antfx_ui_alert("Unable to load channels list");
    }
}

static void antfx_ui_alert(const char *text)
{
    /*Create a Message box*/
    static const char *mbox_btn_map[] = {" ", "OK", " ", ""};
    lv_obj_t *mbox = lv_msgbox_create(lv_scr_act(), NULL);
    lv_msgbox_set_text(mbox, text);
    lv_msgbox_add_btns(mbox, mbox_btn_map);
    lv_btnmatrix_set_btn_ctrl(lv_msgbox_get_btnmatrix(mbox), 0, LV_BTNMATRIX_CTRL_HIDDEN);
    lv_btnmatrix_set_btn_width(lv_msgbox_get_btnmatrix(mbox), 1, 7);
    lv_btnmatrix_set_btn_ctrl(lv_msgbox_get_btnmatrix(mbox), 2, LV_BTNMATRIX_CTRL_HIDDEN);
    lv_obj_set_top(mbox, true);
}
static void antfx_ui_show_music_dialog(lv_obj_t *obj, lv_event_t event)
{
    UNUSED(obj);
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);
    char title[ANTFX_MAX_STR_BUFF_SZ];
    antfx_conf_t *conf = antfx_get_config();
    if (event == LV_EVENT_CLICKED)
    {
        lv_obj_t *win = lv_win_create(scr, NULL);
        lv_obj_t *btn;
        int total_page = conf->media.music.total_songs / 6;
        if (conf->media.music.total_songs % 6 > 0)
            total_page++;
        snprintf(
            title,
            ANTFX_MAX_STR_BUFF_SZ,
            "Playlist (%d/%d)",
            conf->media.music.current_page,
            total_page);
        btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);
        lv_obj_set_event_cb(btn, lv_win_close_event_cb);

        lv_obj_t *list;
        list = lv_list_create(win, NULL);
        lv_obj_set_size(list, lv_disp_get_hor_res(NULL) - 25, lv_disp_get_ver_res(NULL) - 75);
        lv_list_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);
        antfx_ui_music_list_refresh(list);

        btn = lv_win_add_btn(win, LV_SYMBOL_NEXT);
        lv_obj_set_event_cb(btn, antfx_media_music_next_page);
        lv_obj_set_user_data(btn, win);

        btn = lv_win_add_btn(win, LV_SYMBOL_PREV);
        lv_obj_set_event_cb(btn, antfx_media_music_prev_page);
        lv_obj_set_user_data(btn, win);

        lv_obj_set_size(win, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
        lv_win_set_title(win, title);
        lv_win_set_scrollbar_mode(win, LV_SCROLLBAR_MODE_OFF);

        lv_obj_set_top(win, true);
    }
}
static void antfx_ui_show_fm_dialog(lv_obj_t *obj, lv_event_t event)
{
    UNUSED(obj);
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);

    if (event == LV_EVENT_CLICKED)
    {
        lv_obj_t *win = lv_win_create(scr, NULL);
        lv_obj_t *btn;

        btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);
        lv_obj_set_event_cb(btn, lv_win_close_event_cb);

        lv_obj_set_size(win, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
        lv_win_set_title(win, "Radio FM");
        lv_win_set_scrollbar_mode(win, LV_SCROLLBAR_MODE_OFF);

        lv_obj_t *list;
        list = lv_list_create(win, NULL);
        lv_obj_set_size(list, lv_disp_get_hor_res(NULL) - 25, lv_disp_get_ver_res(NULL) - 75);

        btn = lv_win_add_btn(win, LV_SYMBOL_PLUS);
        lv_obj_set_user_data(btn, list);
        lv_obj_set_event_cb(btn, antfx_ui_add_fm_channel_popup);

        btn = lv_win_add_btn(win, LV_SYMBOL_MUTE);
        lv_obj_set_event_cb(btn, antfx_ui_fm_mute_cb);

        antfx_ui_fm_list_refresh(list);

        lv_obj_set_top(win, true);
    }
}
static void antfx_ui_fav_setting(lv_obj_t *obj, lv_event_t event)
{
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);

    if (event == LV_EVENT_CLICKED)
    {
        lv_obj_t *win = lv_win_create(scr, NULL);
        lv_obj_t *btn;
        antfx_conf_t *conf = antfx_get_config();
        memset(g_scr_info.fields.field_1, 0, ANTFX_MAX_STR_BUFF_SZ);
        memset(g_scr_info.fields.field_2, 0, ANTFX_MAX_STR_BUFF_SZ);
        btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);
        lv_obj_set_event_cb(btn, lv_win_close_event_cb);

        btn = lv_win_add_btn(win, LV_SYMBOL_OK);
        lv_obj_set_user_data(btn, lv_obj_get_user_data(obj));
        lv_obj_set_event_cb(btn, antfx_ui_fav_setting_save);

        lv_obj_set_size(win, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
        lv_win_set_layout(win, LV_LAYOUT_OFF);
        lv_win_set_title(win, "User setting");
        lv_win_set_scrollbar_mode(win, LV_SCROLLBAR_MODE_OFF);

        lv_obj_t *ta = lv_textarea_create(win, NULL);
        lv_textarea_set_text(ta, "");
        lv_textarea_set_one_line(ta, true);

        lv_textarea_set_cursor_hidden(ta, true);
        lv_obj_set_width(ta, LV_HOR_RES / 2 - 30);
        lv_obj_set_pos(ta, 10, 30);
        lv_obj_set_user_data(ta, g_scr_info.fields.field_1);
        lv_obj_set_event_cb(ta, antfx_ui_attach_keyboard);
        lv_textarea_set_text(ta, conf->fav.city);
        strncpy(g_scr_info.fields.field_1, conf->fav.city, ANTFX_MAX_STR_BUFF_SZ);

        /* Create.kea keyboard */
        g_scr_info.keyboard = lv_keyboard_create(win, NULL);
        lv_obj_set_size(g_scr_info.keyboard, LV_HOR_RES - 10, LV_VER_RES / 2);
        lv_obj_set_pos(g_scr_info.keyboard, 10, LV_VER_RES / 3);
        lv_obj_set_top(win, true);

        lv_keyboard_set_textarea(g_scr_info.keyboard, ta);
        /* Create a label and position it above the text box */
        lv_obj_t *lbl = lv_label_create(win, NULL);
        lv_label_set_text(lbl, "City:");
        lv_obj_align(lbl, ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

        /* Create the one-line mode text area */
        ta = lv_textarea_create(win, ta);
        lv_textarea_set_cursor_hidden(ta, true);
        lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 30);
        lv_obj_set_user_data(ta, g_scr_info.fields.field_2);
        lv_obj_set_event_cb(ta, antfx_ui_attach_keyboard);
        lv_textarea_set_text(ta, conf->fav.music_path);
        strncpy(g_scr_info.fields.field_2, conf->fav.music_path, ANTFX_MAX_STR_BUFF_SZ);

        /* Create a label and position it above the text box */
        lbl = lv_label_create(win, NULL);
        lv_label_set_text(lbl, "Music path:");
        lv_obj_align(lbl, ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    }
}
static void antfx_ui_fav_setting_save(lv_obj_t *obj, lv_event_t event)
{
    lv_obj_t *parent = lv_obj_get_parent(obj);
    parent = lv_obj_get_parent(parent);
    antfx_conf_t *conf = antfx_get_config();
    if (event == LV_EVENT_RELEASED)
    {
        trim(g_scr_info.fields.field_2, ' ');
        trim(g_scr_info.fields.field_1, ' ');
        if ((strcmp(g_scr_info.fields.field_1, "") == 0) || (strcmp(g_scr_info.fields.field_2, "") == 0))
        {
            antfx_ui_alert("Invalid input");
            return;
        }
        strncpy(conf->fav.city, g_scr_info.fields.field_1, ANTFX_MAX_STR_BUFF_SZ);
        strncpy(conf->fav.music_path, g_scr_info.fields.field_2, ANTFX_MAX_STR_BUFF_SZ);

        if (antfx_db_save_fav(1) == -1)
        {
            antfx_ui_alert("Unable to save user setting to database");
        }
        lv_obj_del(parent);
    }
}
static void antfx_ui_add_fm_channel(lv_obj_t *obj, lv_event_t event)
{
    lv_obj_t *parent = lv_obj_get_parent(obj);
    parent = lv_obj_get_parent(parent);
    if (event == LV_EVENT_RELEASED)
    {
        antfx_fm_record_t record;
        trim(g_scr_info.fields.field_2, ' ');
        trim(g_scr_info.fields.field_1, ' ');
        record.freq = atof(g_scr_info.fields.field_2);
        strncpy(record.name, g_scr_info.fields.field_1, ANTFX_MAX_STR_BUFF_SZ);
        if (record.freq > 0 && strcmp(record.name, "") != 0)
        {
            if (antfx_db_add_fm_channel(&record) == -1)
            {
                antfx_ui_alert("Unable to save channel to database");
            }
            else
            {
                antfx_ui_fm_list_refresh(lv_obj_get_user_data(obj));
            }
        }
        else
        {
            antfx_ui_alert("Invalid input data");
        }
        lv_obj_del(parent);
    }
}
static void antfx_ui_add_fm_channel_popup(lv_obj_t *obj, lv_event_t event)
{
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);

    if (event == LV_EVENT_CLICKED)
    {
        lv_obj_t *win = lv_win_create(scr, NULL);
        lv_obj_t *btn;
        memset(g_scr_info.fields.field_1, 0, ANTFX_MAX_STR_BUFF_SZ);
        memset(g_scr_info.fields.field_2, 0, ANTFX_MAX_STR_BUFF_SZ);
        btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);
        lv_obj_set_event_cb(btn, lv_win_close_event_cb);

        btn = lv_win_add_btn(win, LV_SYMBOL_OK);
        lv_obj_set_user_data(btn, lv_obj_get_user_data(obj));
        lv_obj_set_event_cb(btn, antfx_ui_add_fm_channel);

        lv_obj_set_size(win, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
        lv_win_set_layout(win, LV_LAYOUT_OFF);
        lv_win_set_title(win, "Add radio channel");
        lv_win_set_scrollbar_mode(win, LV_SCROLLBAR_MODE_OFF);

        lv_obj_t *ta = lv_textarea_create(win, NULL);
        lv_textarea_set_text(ta, "");
        lv_textarea_set_one_line(ta, true);
        lv_textarea_set_cursor_hidden(ta, true);
        lv_obj_set_width(ta, LV_HOR_RES / 2 - 30);
        lv_obj_set_pos(ta, 10, 30);
        lv_obj_set_user_data(ta, g_scr_info.fields.field_1);
        lv_obj_set_event_cb(ta, antfx_ui_attach_keyboard);

        /* Create.kea keyboard */
        g_scr_info.keyboard = lv_keyboard_create(win, NULL);
        lv_obj_set_size(g_scr_info.keyboard, LV_HOR_RES - 10, LV_VER_RES / 2);
        lv_obj_set_pos(g_scr_info.keyboard, 10, LV_VER_RES / 3);
        lv_obj_set_top(win, true);

        lv_keyboard_set_textarea(g_scr_info.keyboard, ta);
        /* Create a label and position it above the text box */
        lv_obj_t *lbl = lv_label_create(win, NULL);
        lv_label_set_text(lbl, "Name:");
        lv_obj_align(lbl, ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

        /* Create the one-line mode text area */
        ta = lv_textarea_create(win, ta);
        ;
        lv_textarea_set_cursor_hidden(ta, true);
        lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_RIGHT, -20, 30);
        lv_obj_set_user_data(ta, g_scr_info.fields.field_2);
        lv_obj_set_event_cb(ta, antfx_ui_attach_numpad);

        /* Create a label and position it above the text box */
        lbl = lv_label_create(win, NULL);
        lv_label_set_text(lbl, "Frequency:");
        lv_obj_align(lbl, ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    }
}

static void antfx_ui_attach_keyboard(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        if (g_scr_info.keyboard != NULL)
        {
            lv_keyboard_set_mode(g_scr_info.keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
            lv_keyboard_set_textarea(g_scr_info.keyboard, obj);
        }
    }
    else if (event == LV_EVENT_INSERT)
    {
        const char *str = lv_textarea_get_text(obj);
        char *buff = (char *)lv_obj_get_user_data(obj);
        if (buff)
        {
            strncpy(buff, str, ANTFX_MAX_STR_BUFF_SZ);
            strcat(buff, (const char *)lv_event_get_data());
        }
    }
}
static void antfx_ui_attach_numpad(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_CLICKED)
    {
        if (g_scr_info.keyboard != NULL)
        {
            lv_keyboard_set_mode(g_scr_info.keyboard, LV_KEYBOARD_MODE_NUM);
            lv_keyboard_set_textarea(g_scr_info.keyboard, obj);
        }
    }
    else if (event == LV_EVENT_INSERT)
    {
        const char *str = lv_textarea_get_text(obj);
        char *buff = (char *)lv_obj_get_user_data(obj);
        if (buff)
        {
            strncpy(buff, str, ANTFX_MAX_STR_BUFF_SZ);
            strcat(buff, (const char *)lv_event_get_data());
        }
    }
}
static void antfx_ui_close_popup(lv_obj_t *obj, lv_event_t event)
{
    lv_obj_t *parent = lv_obj_get_parent(obj);
    if (event == LV_EVENT_RELEASED)
    {
        lv_obj_del(parent);
    }
}
static void antfx_ui_show_calendar(lv_obj_t *obj, lv_event_t event)
{
    UNUSED(obj);
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);

    if (event == LV_EVENT_CLICKED)
    {
        lv_obj_t *win = lv_cont_create(scr, NULL);
        lv_cont_set_layout(win, LV_LAYOUT_OFF);

        //lv_obj_set_event_cb(win_btn, lv_win_close_event_cb);
        lv_obj_set_size(win, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
        lv_obj_t *cal = lv_calendar_create(win, NULL);
        lv_obj_set_size(cal, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
        /*Set today's date*/
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        lv_calendar_date_t today;
        today.year = tm.tm_year + 1900;
        today.month = tm.tm_mon + 1;
        today.day = tm.tm_mday;

        lv_calendar_set_today_date(cal, &today);
        lv_calendar_set_showed_date(cal, &today);

        lv_obj_t *btn = lv_btn_create(win, NULL);
        lv_obj_set_size(btn, 32, 32);
        lv_obj_set_pos(btn, lv_disp_get_hor_res(NULL) - 80, 3);

        lv_img_set_src(lv_img_create(btn, NULL), LV_SYMBOL_CLOSE);
        lv_obj_set_event_cb(btn, antfx_ui_close_popup);

        lv_obj_set_top(win, true);
    }
}