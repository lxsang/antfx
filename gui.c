#include <time.h>
#include "gui.h"
#include "db.h"
#include "hw.h"

#define MAX_FIELD_SIZE 255
typedef struct
{
    char field_1[MAX_FIELD_SIZE];
    char field_2[MAX_FIELD_SIZE];
    char field_3[MAX_FIELD_SIZE];
    char field_4[MAX_FIELD_SIZE];
    char field_5[MAX_FIELD_SIZE];
} antfx_ui_form_data_t;

typedef struct
{
    lv_obj_t *lbl_time;
    lv_obj_t *lbl_date;
    lv_obj_t *lbl_weather;
    lv_obj_t *lbl_status;
    lv_obj_t *lbl_location;
    lv_obj_t *lbl_weather_img;
    lv_obj_t *keyboard;
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
static void antfx_ui_add_fm_channel_popup(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_add_fm_channel(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_attach_keyboard(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_attach_numpad(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_fm_list_refresh(lv_obj_t *list);
static void antfx_ui_fm_list_add(antfx_fm_record_t *r, void *list);
static void antfx_ui_fm_item_action(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_fm_item_action_cb(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_fm_mute_cb(lv_obj_t *obj, lv_event_t event);
static void antfx_ui_alert(const char *);

void antfx_ui_update_datetime()
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
}
void antfx_ui_update_location(const char *city)
{
    lv_label_set_text(g_scr_info.lbl_location, city);
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
        printf("icon set\n");
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
    /* Initialize and set a theme. `LV_THEME_NIGHT` needs to enabled in lv_conf.h. */
    lv_theme_t *th = lv_theme_night_init(20, NULL);
    lv_theme_set_current(th);
    lv_theme_set_current(th);
    th = lv_theme_get_current(); /*If `LV_THEME_LIVE_UPDATE  1` `th` is not used directly so get the real theme after set*/
    lv_obj_t *scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);

    lv_obj_t *img = lv_img_create(scr, NULL);
    lv_img_set_src(img, &default_wp);

    lv_obj_t *list = lv_list_create(scr, NULL);
    lv_style_t *style = (lv_style_t *)lv_obj_get_style(list);
    static lv_style_t cont_style;
    //lv_obj_set_style(list, &lv_style_transp);
    style->body.opa = LV_OPA_60;
    lv_obj_set_size(list, 48, lv_disp_get_ver_res(NULL));

    lv_obj_t *btn;
    btn = lv_list_add_btn(list, &radio, NULL);
    lv_obj_set_event_cb(btn, antfx_ui_show_fm_dialog);

    lv_list_add_btn(list, LV_SYMBOL_AUDIO, NULL);
    lv_list_add_btn(list, &alarm_clock, NULL);

    btn = lv_list_add_btn(list, &calendar, NULL);
    lv_obj_set_event_cb(btn, antfx_ui_show_calendar);

    lv_list_add_btn(list, &camera, NULL);
    lv_list_add_btn(list, LV_SYMBOL_SETTINGS, NULL);

    lv_obj_t *cont = lv_cont_create(scr, NULL);
    lv_cont_set_layout(cont, LV_LAYOUT_COL_M);
    g_scr_info.lbl_time = lv_label_create(cont, NULL);
    lv_label_set_align(g_scr_info.lbl_time, LV_LABEL_ALIGN_CENTER);
    static lv_style_t time_style;
    static lv_style_t status_style;
    lv_style_copy(&cont_style, lv_cont_get_style(cont, LV_CONT_STYLE_MAIN));
    lv_style_copy(&time_style, &cont_style);
    lv_style_copy(&status_style, &cont_style);
    time_style.text.font = &roboto_bold_50;
    lv_obj_set_style(g_scr_info.lbl_time, &time_style);
    lv_obj_set_pos(cont, lv_disp_get_hor_res(NULL) / 2 - 10, 10);
    lv_obj_set_size(cont, lv_disp_get_hor_res(NULL) / 2, lv_disp_get_ver_res(NULL) - 20);
    g_scr_info.lbl_date = lv_label_create(cont, NULL);
    cont_style.text.font = &lv_font_roboto_28;
    cont_style.body.opa = LV_OPA_40;
    cont_style.body.radius = 20;
    lv_cont_set_style(cont, LV_CONT_STYLE_MAIN, &cont_style);

    g_scr_info.lbl_location = lv_label_create(cont, NULL);
    lv_label_set_text(g_scr_info.lbl_location, "");

    g_scr_info.lbl_weather_img = lv_img_create(cont, NULL);
    //lv_img_set_src(g_scr_info.lbl_location, &w50d);
    lv_img_set_src(g_scr_info.lbl_weather_img, LV_SYMBOL_DUMMY);

    g_scr_info.lbl_weather = lv_label_create(cont, NULL);
    lv_label_set_text(g_scr_info.lbl_weather, "");
    lv_obj_set_style(g_scr_info.lbl_weather, &status_style);

    g_scr_info.lbl_status = lv_label_create(cont, NULL);
    lv_label_set_text(g_scr_info.lbl_status, "");
    lv_obj_set_style(g_scr_info.lbl_status, &status_style);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void antfx_ui_fm_mute_cb(lv_obj_t *obj, lv_event_t event)
{
    if (event == LV_EVENT_RELEASED)
    {
        fm_mute();
        antfx_ui_update_status("");
    }
}
static void antfx_ui_fm_list_add(antfx_fm_record_t *r, void *list)
{
    char buff[MAX_FIELD_SIZE * 2];
    snprintf(buff, MAX_FIELD_SIZE * 2, "%s : %.2f Mhz", r->name, r->freq);
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
        lv_obj_t *mbox = lv_mbox_create(lv_scr_act(), NULL);
        lv_mbox_set_text(mbox, "Choose action:");
        lv_mbox_add_btns(mbox, mbox_btn_map);
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
    if (event == LV_EVENT_VALUE_CHANGED)
    {
        r = (antfx_fm_record_t *)lv_obj_get_user_data(obj);
        action = lv_mbox_get_active_btn_text(obj);
        if (strcmp(action, "Play") == 0)
        {
            fm_set_freq(r->freq);
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
        lv_mbox_start_auto_close(obj, 0);
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
    lv_obj_t *mbox = lv_mbox_create(lv_scr_act(), NULL);
    lv_mbox_set_text(mbox, text);
    lv_mbox_add_btns(mbox, mbox_btn_map);
    lv_btnm_set_btn_ctrl(lv_mbox_get_btnm(mbox), 0, LV_BTNM_CTRL_HIDDEN);
    lv_btnm_set_btn_width(lv_mbox_get_btnm(mbox), 1, 7);
    lv_btnm_set_btn_ctrl(lv_mbox_get_btnm(mbox), 2, LV_BTNM_CTRL_HIDDEN);
    lv_obj_set_top(mbox, true);
}
static void antfx_ui_show_fm_dialog(lv_obj_t *obj, lv_event_t event)
{
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);

    if (event == LV_EVENT_CLICKED)
    {
        lv_obj_t *win = lv_win_create(scr, NULL);
        lv_obj_t *btn;

        btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);
        lv_obj_set_event_cb(btn, lv_win_close_event_cb);

        lv_obj_set_size(win, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
        lv_win_set_title(win, "Radio FM");
        lv_win_set_sb_mode(win, LV_SB_MODE_OFF);

        lv_obj_t *list;
        list = lv_list_create(win, NULL);
        lv_obj_set_size(list, lv_disp_get_hor_res(NULL) - 10, lv_disp_get_ver_res(NULL) - 60);

        btn = lv_win_add_btn(win, LV_SYMBOL_PLUS);
        lv_obj_set_user_data(btn, list);
        lv_obj_set_event_cb(btn, antfx_ui_add_fm_channel_popup);

        btn = lv_win_add_btn(win, LV_SYMBOL_MUTE);
        lv_obj_set_event_cb(btn, antfx_ui_fm_mute_cb);

        antfx_ui_fm_list_refresh(list);

        lv_obj_set_top(win, true);
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
        strncpy(record.name, g_scr_info.fields.field_1, MAX_FIELD_SIZE);
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
        lv_style_t *style;
        memset(g_scr_info.fields.field_1, 0, MAX_FIELD_SIZE);
        memset(g_scr_info.fields.field_2, 0, MAX_FIELD_SIZE);
        btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);
        lv_obj_set_event_cb(btn, lv_win_close_event_cb);

        btn = lv_win_add_btn(win, LV_SYMBOL_OK);
        lv_obj_set_user_data(btn, lv_obj_get_user_data(obj));
        lv_obj_set_event_cb(btn, antfx_ui_add_fm_channel);

        lv_obj_set_size(win, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
        lv_win_set_layout(win, LV_LAYOUT_OFF);
        lv_win_set_title(win, "Add radio channel");
        lv_win_set_sb_mode(win, LV_SB_MODE_OFF);

        lv_obj_t *ta = lv_ta_create(win, NULL);
        lv_ta_set_text(ta, "");
        lv_ta_set_one_line(ta, true);
        lv_ta_set_cursor_type(ta, LV_CURSOR_HIDDEN);
        lv_obj_set_width(ta, LV_HOR_RES / 2 - 30);
        lv_obj_set_pos(ta, 10, 30);
        lv_obj_set_user_data(ta, g_scr_info.fields.field_1);
        lv_obj_set_event_cb(ta, antfx_ui_attach_keyboard);

         /* Create.kea keyboard */
        g_scr_info.keyboard = lv_kb_create(win, NULL);
        lv_obj_set_size(g_scr_info.keyboard, LV_HOR_RES - 10, LV_VER_RES / 2);
        lv_obj_set_pos(g_scr_info.keyboard, 10, LV_VER_RES / 3);
        lv_obj_set_top(win, true);

        lv_kb_set_ta(g_scr_info.keyboard, ta);
        /* Create a label and position it above the text box */
        lv_obj_t *lbl = lv_label_create(win, NULL);
        lv_label_set_text(lbl, "Name:");
        lv_obj_align(lbl, ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

        /* Create the one-line mode text area */
        ta = lv_ta_create(win, ta);
        lv_ta_set_cursor_type(ta, LV_CURSOR_HIDDEN);
        lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 30);
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
            lv_kb_set_mode(g_scr_info.keyboard, LV_KB_MODE_TEXT);
            lv_kb_set_ta(g_scr_info.keyboard, obj);
        }
    }
    else if (event == LV_EVENT_INSERT)
    {
        const char *str = lv_ta_get_text(obj);
        char *buff = (char *)lv_obj_get_user_data(obj);
        if (buff)
        {
            strncpy(buff, str, MAX_FIELD_SIZE);
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
            lv_kb_set_mode(g_scr_info.keyboard, LV_KB_MODE_NUM);
            lv_kb_set_ta(g_scr_info.keyboard, obj);
        }
    }
    else if (event == LV_EVENT_INSERT)
    {
        const char *str = lv_ta_get_text(obj);
        char *buff = (char *)lv_obj_get_user_data(obj);
        if (buff)
        {
            strncpy(buff, str, MAX_FIELD_SIZE);
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
    lv_obj_t *scr = lv_disp_get_scr_act(NULL);

    if (event == LV_EVENT_CLICKED)
    {
        lv_obj_t *win = lv_cont_create(scr, NULL);
        lv_cont_set_layout(win, LV_LAYOUT_OFF);
        lv_style_t *style = (lv_style_t *)lv_obj_get_style(win);

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

static void create_tab1(lv_obj_t *parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY);

    lv_theme_t *th = lv_theme_get_current();

    static lv_style_t h_style;
    lv_style_copy(&h_style, &lv_style_transp);
    h_style.body.padding.inner = LV_DPI / 10;
    h_style.body.padding.left = LV_DPI / 4;
    h_style.body.padding.right = LV_DPI / 4;
    h_style.body.padding.top = LV_DPI / 10;
    h_style.body.padding.bottom = LV_DPI / 10;

    lv_obj_t *h = lv_cont_create(parent, NULL);
    lv_obj_set_style(h, &h_style);
    lv_obj_set_click(h, false);
    lv_cont_set_fit(h, LV_FIT_TIGHT);
    lv_cont_set_layout(h, LV_LAYOUT_COL_M);

    lv_obj_t *btn = lv_btn_create(h, NULL);
    lv_btn_set_fit(btn, LV_FIT_TIGHT);
    lv_btn_set_toggle(btn, true);
    lv_obj_t *btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Button");

    btn = lv_btn_create(h, btn);
    lv_btn_toggle(btn);
    btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Toggled");

    btn = lv_btn_create(h, btn);
    lv_btn_set_state(btn, LV_BTN_STATE_INA);
    btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Inactive");

    lv_obj_t *label = lv_label_create(h, NULL);
    lv_label_set_text(label, "Primary");
    lv_obj_set_style(label, th->style.label.prim);

    label = lv_label_create(h, NULL);
    lv_label_set_text(label, "Secondary");
    lv_obj_set_style(label, th->style.label.sec);

    label = lv_label_create(h, NULL);
    lv_label_set_text(label, "Hint");
    lv_obj_set_style(label, th->style.label.hint);

    static const char *btnm_str[] = {"1", "2", "3", LV_SYMBOL_OK, LV_SYMBOL_CLOSE, ""};
    lv_obj_t *btnm = lv_btnm_create(h, NULL);
    lv_obj_set_size(btnm, lv_disp_get_hor_res(NULL) / 4, 2 * LV_DPI / 3);
    lv_btnm_set_map(btnm, btnm_str);
    lv_btnm_set_btn_ctrl_all(btnm, LV_BTNM_CTRL_TGL_ENABLE);
    lv_btnm_set_one_toggle(btnm, true);

    lv_obj_t *table = lv_table_create(h, NULL);
    lv_table_set_col_cnt(table, 3);
    lv_table_set_row_cnt(table, 4);
    lv_table_set_col_width(table, 0, LV_DPI / 3);
    lv_table_set_col_width(table, 1, LV_DPI / 2);
    lv_table_set_col_width(table, 2, LV_DPI / 2);
    lv_table_set_cell_merge_right(table, 0, 0, true);
    lv_table_set_cell_merge_right(table, 0, 1, true);

    lv_table_set_cell_value(table, 0, 0, "Table");
    lv_table_set_cell_align(table, 0, 0, LV_LABEL_ALIGN_CENTER);

    lv_table_set_cell_value(table, 1, 0, "1");
    lv_table_set_cell_value(table, 1, 1, "13");
    lv_table_set_cell_align(table, 1, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_value(table, 1, 2, "ms");

    lv_table_set_cell_value(table, 2, 0, "2");
    lv_table_set_cell_value(table, 2, 1, "46");
    lv_table_set_cell_align(table, 2, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_value(table, 2, 2, "ms");

    lv_table_set_cell_value(table, 3, 0, "3");
    lv_table_set_cell_value(table, 3, 1, "61");
    lv_table_set_cell_align(table, 3, 1, LV_LABEL_ALIGN_RIGHT);
    lv_table_set_cell_value(table, 3, 2, "ms");

    h = lv_cont_create(parent, h);

    lv_obj_t *sw_h = lv_cont_create(h, NULL);
    lv_cont_set_style(sw_h, LV_CONT_STYLE_MAIN, &lv_style_transp);
    lv_cont_set_fit2(sw_h, LV_FIT_NONE, LV_FIT_TIGHT);
    lv_obj_set_width(sw_h, LV_HOR_RES / 4);
    lv_cont_set_layout(sw_h, LV_LAYOUT_PRETTY);

    lv_obj_t *sw = lv_sw_create(sw_h, NULL);
    lv_sw_set_anim_time(sw, 250);

    sw = lv_sw_create(sw_h, sw);
    lv_sw_on(sw, LV_ANIM_OFF);

    lv_obj_t *bar = lv_bar_create(h, NULL);
    lv_bar_set_value(bar, 70, false);

    lv_obj_t *slider = lv_slider_create(h, NULL);
    lv_bar_set_value(slider, 70, false);

    lv_obj_t *line = lv_line_create(h, NULL);
    static lv_point_t line_p[2];
    line_p[0].x = 0;
    line_p[0].y = 0;
    line_p[1].x = lv_disp_get_hor_res(NULL) / 5;
    line_p[1].y = 0;

    lv_line_set_points(line, line_p, 2);
    lv_line_set_style(line, LV_LINE_STYLE_MAIN, th->style.line.decor);

    lv_obj_t *cb = lv_cb_create(h, NULL);

    cb = lv_cb_create(h, cb);
    lv_btn_set_state(cb, LV_BTN_STATE_TGL_REL);

    lv_obj_t *ddlist = lv_ddlist_create(h, NULL);
    lv_ddlist_set_fix_width(ddlist, lv_obj_get_width(ddlist) + LV_DPI / 2); /*Make space for the arrow*/
    lv_ddlist_set_draw_arrow(ddlist, true);

    h = lv_cont_create(parent, h);

    lv_obj_t *list = lv_list_create(h, NULL);
    lv_obj_set_size(list, lv_disp_get_hor_res(NULL) / 4, lv_disp_get_ver_res(NULL) / 2);
    lv_obj_t *list_btn;
    list_btn = lv_list_add_btn(list, LV_SYMBOL_GPS, "GPS");
    lv_btn_set_toggle(list_btn, true);

    lv_list_add_btn(list, LV_SYMBOL_WIFI, "WiFi");
    lv_list_add_btn(list, LV_SYMBOL_GPS, "GPS");
    lv_list_add_btn(list, LV_SYMBOL_AUDIO, "Audio");
    lv_list_add_btn(list, LV_SYMBOL_VIDEO, "Video");
    lv_list_add_btn(list, LV_SYMBOL_CALL, "Call");
    lv_list_add_btn(list, LV_SYMBOL_BELL, "Bell");
    lv_list_add_btn(list, LV_SYMBOL_FILE, "File");
    lv_list_add_btn(list, LV_SYMBOL_EDIT, "Edit");
    lv_list_add_btn(list, LV_SYMBOL_CUT, "Cut");
    lv_list_add_btn(list, LV_SYMBOL_COPY, "Copy");

    lv_obj_t *roller = lv_roller_create(h, NULL);
    lv_roller_set_options(roller, "Monday\nTuesday\nWednesday\nThursday\nFriday\nSaturday\nSunday", true);
    lv_roller_set_selected(roller, 1, false);
    lv_roller_set_visible_row_count(roller, 3);
}

static void create_tab2(lv_obj_t *parent)
{
    lv_coord_t w = lv_page_get_scrl_width(parent);

    lv_obj_t *chart = lv_chart_create(parent, NULL);
    lv_chart_set_type(chart, LV_CHART_TYPE_AREA);
    lv_obj_set_size(chart, w / 3, lv_disp_get_ver_res(NULL) / 3);
    lv_obj_set_pos(chart, LV_DPI / 10, LV_DPI / 10);
    lv_chart_series_t *s1 = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_set_next(chart, s1, 30);
    lv_chart_set_next(chart, s1, 20);
    lv_chart_set_next(chart, s1, 10);
    lv_chart_set_next(chart, s1, 12);
    lv_chart_set_next(chart, s1, 20);
    lv_chart_set_next(chart, s1, 27);
    lv_chart_set_next(chart, s1, 35);
    lv_chart_set_next(chart, s1, 55);
    lv_chart_set_next(chart, s1, 70);
    lv_chart_set_next(chart, s1, 75);

    lv_obj_t *gauge = lv_gauge_create(parent, NULL);
    lv_gauge_set_value(gauge, 0, 40);
    lv_obj_set_size(gauge, w / 4, w / 4);
    lv_obj_align(gauge, chart, LV_ALIGN_OUT_BOTTOM_LEFT, 0, LV_DPI / 4);

    lv_obj_t *arc = lv_arc_create(parent, NULL);
    lv_obj_align(arc, gauge, LV_ALIGN_OUT_BOTTOM_MID, 0, LV_DPI / 8);

    lv_obj_t *ta = lv_ta_create(parent, NULL);
    lv_obj_set_size(ta, w / 3, lv_disp_get_ver_res(NULL) / 4);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_RIGHT, -LV_DPI / 10, LV_DPI / 10);
    lv_ta_set_cursor_type(ta, LV_CURSOR_BLOCK);

    lv_obj_t *kb = lv_kb_create(parent, NULL);
    lv_obj_set_size(kb, 2 * w / 3, lv_disp_get_ver_res(NULL) / 3);
    lv_obj_align(kb, ta, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, LV_DPI);
    lv_kb_set_ta(kb, ta);

#if LV_USE_ANIMATION
    lv_obj_t *loader = lv_preload_create(parent, NULL);
    lv_obj_align(loader, NULL, LV_ALIGN_CENTER, 0, -LV_DPI);
#endif
}

static void create_tab3(lv_obj_t *parent)
{
    /*Create a Window*/
    lv_obj_t *win = lv_win_create(parent, NULL);
    lv_obj_t *win_btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE);
    lv_obj_set_event_cb(win_btn, lv_win_close_event_cb);
    lv_win_add_btn(win, LV_SYMBOL_DOWN);
    lv_obj_set_size(win, lv_disp_get_hor_res(NULL) / 2, lv_disp_get_ver_res(NULL) / 2);
    lv_obj_set_pos(win, LV_DPI / 20, LV_DPI / 20);
    lv_obj_set_top(win, true);

    /*Create a Label in the Window*/
    lv_obj_t *label = lv_label_create(win, NULL);
    lv_label_set_text(label, "Label in the window");

    /*Create a  Line meter in the Window*/
    lv_obj_t *lmeter = lv_lmeter_create(win, NULL);
    lv_obj_align(lmeter, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, LV_DPI / 2);
    lv_lmeter_set_value(lmeter, 70);

    /*Create a 2 LEDs in the Window*/
    lv_obj_t *led1 = lv_led_create(win, NULL);
    lv_obj_align(led1, lmeter, LV_ALIGN_OUT_RIGHT_MID, LV_DPI / 2, 0);
    lv_led_on(led1);

    lv_obj_t *led2 = lv_led_create(win, NULL);
    lv_obj_align(led2, led1, LV_ALIGN_OUT_RIGHT_MID, LV_DPI / 2, 0);
    lv_led_off(led2);

    /*Create a Page*/
    lv_obj_t *page = lv_page_create(parent, NULL);
    lv_obj_set_size(page, lv_disp_get_hor_res(NULL) / 3, lv_disp_get_ver_res(NULL) / 2);
    lv_obj_set_top(page, true);
    lv_obj_align(page, win, LV_ALIGN_IN_TOP_RIGHT, LV_DPI, LV_DPI);

    label = lv_label_create(page, NULL);
    lv_label_set_text(label, "Lorem ipsum dolor sit amet, repudiare voluptatibus pri cu.\n"
                             "Ei mundi pertinax posidonium eum, cum tempor maiorum at,\n"
                             "mea fuisset assentior ad. Usu cu suas civibus iudicabit.\n"
                             "Eum eu congue tempor facilisi. Tale hinc unum te vim.\n"
                             "Te cum populo animal eruditi, labitur inciderint at nec.\n\n"
                             "Eius corpora et quo. Everti voluptaria instructior est id,\n"
                             "vel in falli primis. Mea ei porro essent admodum,\n"
                             "his ei malis quodsi, te quis aeterno his.\n"
                             "Qui tritani recusabo reprehendunt ne,\n"
                             "per duis explicari at. Simul mediocritatem mei et.");

    /*Create a Calendar*/
    lv_obj_t *cal = lv_calendar_create(parent, NULL);
    lv_obj_set_size(cal, 5 * LV_DPI / 2, 5 * LV_DPI / 2);
    lv_obj_align(cal, page, LV_ALIGN_OUT_RIGHT_TOP, -LV_DPI / 2, LV_DPI / 3);
    lv_obj_set_top(cal, true);

    static lv_calendar_date_t highlighted_days[2];
    highlighted_days[0].day = 5;
    highlighted_days[0].month = 5;
    highlighted_days[0].year = 2018;

    highlighted_days[1].day = 8;
    highlighted_days[1].month = 5;
    highlighted_days[1].year = 2018;

    lv_calendar_set_highlighted_dates(cal, highlighted_days, 2);
    lv_calendar_set_today_date(cal, &highlighted_days[0]);
    lv_calendar_set_showed_date(cal, &highlighted_days[0]);

    /*Create a Message box*/
    static const char *mbox_btn_map[] = {" ", "Got it!", " ", ""};
    lv_obj_t *mbox = lv_mbox_create(parent, NULL);
    lv_mbox_set_text(mbox, "Click on the window or the page to bring it to the foreground");
    lv_mbox_add_btns(mbox, mbox_btn_map);
    lv_btnm_set_btn_ctrl(lv_mbox_get_btnm(mbox), 0, LV_BTNM_CTRL_HIDDEN);
    lv_btnm_set_btn_width(lv_mbox_get_btnm(mbox), 1, 7);
    lv_btnm_set_btn_ctrl(lv_mbox_get_btnm(mbox), 2, LV_BTNM_CTRL_HIDDEN);
    lv_obj_set_top(mbox, true);
}