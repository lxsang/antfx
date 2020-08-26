#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pthread.h>
#include <curl/curl.h>
#include <regex.h>
#include "antfx.h"

#define FB_DEV "/dev/fb1"
#define TS_DEV "/dev/input/event0"
#define I2C_DELDEV_FILE "/sys/class/i2c-adapter/i2c-1/delete_device"
#define I2C_NEWDEV_FILE "/sys/class/i2c-adapter/i2c-1/new_device"
#define FORECAST_URI "http://api.openweathermap.org/data/2.5/weather?q=Grenoble&APPID=3e1941f1164b6111bff6f2c3f5ad32d9&&units=metric"
#define HW_CLOCK_ADDR 0x68
#define FM_RADIO_CTL_ADDR 0x60
#define WEATHER_CHECK_TO 60 * 3 //
#define MAX_CURL_PAGE_LENGTH 2048
#define LOCATION "Grenoble"

typedef struct
{
    lv_obj_t *lbl_time;
    lv_obj_t *lbl_date;
    lv_obj_t *lbl_weather;
    lv_obj_t *lbl_status;
    lv_obj_t *lbl_location;
    lv_obj_t *lbl_weather_img;
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

static int running = 0;
static antfx_screen_info_t g_scr_info;

void stop(int sig)
{
    UNUSED(sig);
    running = 0;
}

static void init_hw_clock();
static void fm_set_freq(double f);
static void fm_mute();
static double fm_get_freq();
static void time_update();
static void weather_update();
static void *weather_thread_handler(void *data);
static int regex_match(const char *expr, const char *search, int msize, regmatch_t *matches);
static size_t curl_callback(void *ptr, size_t size, size_t nmemb, void *buff);
static void create_tab1(lv_obj_t *parent);
static void create_tab2(lv_obj_t *parent);
static void create_tab3(lv_obj_t *parent);

static void antfx_ui(lv_theme_t *th)
{
    lv_theme_set_current(th);
    th = lv_theme_get_current(); /*If `LV_THEME_LIVE_UPDATE  1` `th` is not used directly so get the real theme after set*/
    lv_obj_t *scr = lv_cont_create(NULL, NULL);
    lv_disp_load_scr(scr);
    /*lv_obj_t *tv = lv_tabview_create(scr, NULL);
    lv_obj_set_size(tv, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_t *tab1 = lv_tabview_add_tab(tv, "Tab 1");
    lv_obj_t *tab2 = lv_tabview_add_tab(tv, "Tab 2");
    lv_obj_t *tab3 = lv_tabview_add_tab(tv, "Tab 3");

    create_tab1(tab1);
    create_tab2(tab2);
    create_tab3(tab3);*/
    lv_obj_t *img = lv_img_create(scr, NULL);
    lv_img_set_src(img, &default_wp);

    lv_obj_t *list = lv_list_create(scr, NULL);
    lv_style_t *style = lv_obj_get_style(list);
    //lv_obj_set_style(list, &lv_style_transp);
    style->body.opa = LV_OPA_60;
    lv_obj_set_size(list, 48, lv_disp_get_ver_res(NULL));
    //list_btn = lv_list_add_btn(list, LV_SYMBOL_GPS, NULL);
    //lv_btn_set_toggle(list_btn, true);
    lv_list_add_btn(list, &radio, NULL);
    lv_list_add_btn(list, LV_SYMBOL_AUDIO, NULL);
    lv_list_add_btn(list, &alarm_clock, NULL);
    lv_list_add_btn(list, &calendar, NULL);
    lv_list_add_btn(list, &camera, NULL);
    lv_list_add_btn(list, LV_SYMBOL_SETTINGS, NULL);

    lv_obj_t *cont = lv_cont_create(scr, NULL);
    lv_cont_set_layout(cont, LV_LAYOUT_COL_M);
    g_scr_info.lbl_time = lv_label_create(cont, NULL);
    lv_label_set_align(g_scr_info.lbl_time, LV_LABEL_ALIGN_CENTER);
    static lv_style_t time_style;
    static lv_style_t status_style;
    style = lv_obj_get_style(cont);
    lv_style_copy(&time_style, style);
    lv_style_copy(&status_style, style);
    time_style.text.font = &roboto_bold_50;
    lv_obj_set_style(g_scr_info.lbl_time, &time_style);
    lv_obj_set_pos(cont, lv_disp_get_hor_res(NULL) / 2 - 10, 10);
    lv_obj_set_size(cont, lv_disp_get_hor_res(NULL) / 2, lv_disp_get_ver_res(NULL) - 20);
    g_scr_info.lbl_date = lv_label_create(cont, NULL);
    style->text.font = &lv_font_roboto_28;
    style->body.opa = LV_OPA_40;
    style->body.radius = 20;
    status_style.text.font = &lv_font_roboto_22;

    g_scr_info.lbl_location = lv_label_create(cont, NULL);
    lv_label_set_text(g_scr_info.lbl_location, LOCATION);

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

static void time_update()
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
static size_t curl_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    int dlen = size * nmemb;
    int total_len = 0;
    char *buffer = (char *)stream;
    total_len = strlen(buffer) + dlen;
    if (total_len > MAX_CURL_PAGE_LENGTH - 1)
    {
        ERROR("Buffer overflow: page data is too long");
        return -1;
    }
    (void)memcpy(buffer + strlen(buffer), ptr, dlen);
    buffer[total_len] = '\0';
    return total_len;
}

static int regex_match(const char *expr, const char *search, int msize, regmatch_t *matches)
{
    regex_t regex;
    int reti;
    char msgbuf[100];
    int ret;
    /* Compile regular expression */
    reti = regcomp(&regex, expr, REG_ICASE | REG_EXTENDED);
    if (reti)
    {
        //ERROR("Could not compile regex: %s",expr);
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        ERROR("Regex match failed: %s", msgbuf);
        //return 0;
    }

    /* Execute regular expression */
    reti = regexec(&regex, search, msize, matches, 0);
    if (!reti)
    {
        //LOG("Match");
        ret = 1;
    }
    else if (reti == REG_NOMATCH)
    {
        //LOG("No match");
        ret = 0;
    }
    else
    {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        ERROR("Regex match failed: %s", msgbuf);
        ret = 0;
    }

    regfree(&regex);
    return ret;
}

static void *weather_thread_handler(void *data)
{
    UNUSED(data);
    char buffer[MAX_CURL_PAGE_LENGTH];
    char x_text[64];
    float temperature;
    regex_t regex;
    regmatch_t matches[2];
    int ret;
    LOG("Fetching weather infomation");
    CURL *curl_handle;
    CURLcode res;
    (void)memset(buffer, 0, MAX_CURL_PAGE_LENGTH);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, FORECAST_URI);
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, buffer);

    res = curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);

    if (res != CURLE_OK)
    {
        ERROR("Unable to fetch weather from url: %s", curl_easy_strerror(res));
        return NULL;
    }
    // now parse the temperature
    if (!regex_match("\\s*\"temp\":\\s*([0-9.]+),?\\s*", buffer, 2, matches))
    {
        ERROR("Unable to match temperature value");
        return NULL;
    }

    memset(x_text, '\0', sizeof(x_text));
    memcpy(x_text, buffer + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
    strcat(x_text, " C, ");

    // description
    if (!regex_match("\\s*\"main\":\\s*\"([a-zA-Z]+)\",?\\s*", buffer, 2, matches))
    {
        ERROR("Unable to match wheather description value");
        return NULL;
    }
    memcpy(x_text + strlen(x_text), buffer + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
    lv_label_set_text(g_scr_info.lbl_weather, x_text);

    // icons
    if (!regex_match("\\s*\"icon\":\\s*\"([a-zA-Z0-9]+)\",?\\s*", buffer, 2, matches))
    {
        ERROR("Unable to match wheather icon value");
        return NULL;
    }

    memset(x_text, '\0', sizeof(x_text));
    memcpy(x_text, buffer + matches[1].rm_so, matches[1].rm_eo - matches[1].rm_so);
    LOG("Icon: '%s'", x_text);
    // show icon

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
static void weather_update()
{
    pthread_t th;
    if (pthread_create(&th, NULL, weather_thread_handler, NULL) != 0)
    {

        ERROR("Error creating weather thread: %s", strerror(errno));
        return;
    }

    if (pthread_detach(th) != 0)
    {
        ERROR("Unable to detach weather thread: %s", strerror(errno));
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
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

int main(int argc, char *argv[])
{
    UNUSED(argc);
    UNUSED(argv);
    LOG_INIT("antfx");
    signal(SIGPIPE, SIG_IGN);
    signal(SIGABRT, SIG_IGN);
    signal(SIGINT, stop);
    signal(SIGTERM, stop);
    engine_config_t conf;
    conf.default_w = 480;
    conf.default_h = 320;
    conf.defaut_bbp = 16;
    conf.dev = FB_DEV;
    conf.tdev = TS_DEV;
    time_t last_weather_check = 0;
    time_t now;
    curl_global_init(CURL_GLOBAL_ALL);
    // start the hardware clock
    init_hw_clock();
    // start display engine
    antfx_init(conf);
    /* Initialize and set a theme. `LV_THEME_NIGHT` needs to enabled in lv_conf.h. */
    lv_theme_t *th = lv_theme_night_init(20, NULL);
    lv_theme_set_current(th);

    antfx_ui(th);
    fm_set_freq(94.1); // RFM
    running = 1;
    while (running)
    {
        now = time(NULL);
        if (difftime(now, last_weather_check) > WEATHER_CHECK_TO)
        {
            weather_update();
            last_weather_check = now;
        }
        time_update();
        lv_task_handler();
        lv_tick_inc(5);
        usleep(5000);
    }
    fm_mute();
    antfx_release();
    curl_global_cleanup();
}

static void init_hw_clock()
{
    int fd;
    char buf[32];
    int ret;
    fd = open(I2C_DELDEV_FILE, O_WRONLY);
    if (fd != -1)
    {
        snprintf(buf, 32, "0x%02X", HW_CLOCK_ADDR);
        ret = write(fd, buf, strlen(buf));
        close(fd);
    }
    fd = open(I2C_NEWDEV_FILE, O_WRONLY);
    if (fd == -1)
    {
        ERROR("Unable to open sys file for registering HW clock: %s", strerror(errno));
        return;
    }
    snprintf(buf, 32, "%s 0x%02X", "ds1307", HW_CLOCK_ADDR);
    ret = write(fd, buf, strlen(buf));
    close(fd);
    if (ret != (int)strlen(buf))
    {
        ERROR("Unable to write command to init HW clock");
        return;
    }
    usleep(1000000);
    ret = system("hwclock -s");
    if (ret == -1)
    {
        ERROR("Unable to execute commad for applying datetime from HW clock: %s", strerror(errno));
        return;
    }
}
static void fm_set_freq(double f)
{
    uint8_t radio[5] = {0};
    char buff[32];
    uint8_t freq_h = 0;
    uint8_t freq_l = 0;
    int fd;
    ssize_t ret;
    unsigned int freq_b;
    freq_b = 4 * (f * 1000000 + 225000) / 32768; //calculating PLL word
    freq_h = freq_b >> 8;
    freq_l = freq_b & 0XFF;

    //printf ("Frequency = "); printf("%f",frequency);
    //printf("\n"); // data to be sent

    radio[0] = freq_h; //FREQUENCY H
    radio[1] = freq_l; //FREQUENCY L
    radio[2] = 0xB0;   //3 byte (0xB0): high side LO injection is on,.
    radio[3] = 0x10;   //4 byte (0x10) : Xtal is 32.768 kHz
    radio[4] = 0x00;   //5 byte0x00)

    if ((fd = wiringPiI2CSetup(FM_RADIO_CTL_ADDR)) < 0)
    {
        ERROR("error opening i2c channel");
    }
    ret = write(fd, radio, 5);
    if (ret == -1 || ret != 5)
    {
        ERROR("Unable to write i2c set freq command to radio control: %s", strerror(errno));
    }
    close(fd);
    LOG("FM RADIO on at frequency: %f", fm_get_freq());
    snprintf(buff, 32, "FM: %.2f Mhz", fm_get_freq());
    lv_label_set_text(g_scr_info.lbl_status, buff);
}
static void fm_mute()
{
    uint8_t radio[5] = {0};
    uint8_t freq_h = 0;
    uint8_t freq_l = 0;
    int fd;
    ssize_t ret;
    unsigned int freq_b;
    double frequency = fm_get_freq();
    if (frequency == -1)
    {
        return;
    }
    freq_b = 4 * (frequency * 1000000 + 225000) / 32768; //calculating PLL word
    freq_h = freq_b >> 8;
    freq_h = freq_h | 0x80; // mutes the radio
    freq_l = freq_b & 0XFF;

    //load the above into the array
    radio[0] = freq_h; //FREQUENCY H
    radio[1] = freq_l; //FREQUENCY L
    radio[2] = 0xB0;   //3 byte (0xB0): high side LO injection is on,.
    radio[3] = 0x10;   //4 byte (0x10) : Xtal is 32.768 kHz
    radio[4] = 0x00;   //5 byte0x00)

    if ((fd = wiringPiI2CSetup(FM_RADIO_CTL_ADDR)) < 0)
    {
        ERROR("error opening i2c channel");
    }
    ret = write(fd, radio, 5);
    if (ret == -1 || ret != 5)
    {
        ERROR("Unable to write i2c mute command to radio control: %s", strerror(errno));
    }
    close(fd);
    LOG("FM RADIO off at frequency: %f", frequency);
    lv_label_set_text(g_scr_info.lbl_status, "");
}
static double fm_get_freq()
{
    uint8_t radio[5] = {0};
    ssize_t ret;
    int fd;
    double frequency;

    if ((fd = wiringPiI2CSetup(FM_RADIO_CTL_ADDR)) < 0)
    {
        ERROR("error opening i2c channel");
    }
    ret = read(fd, radio, 5);
    close(fd);
    if (ret == -1 || ret != 5)
    {
        ERROR("Unable to read i2c command from radio control: %s", strerror(errno));
        return -1;
    }

    frequency = ((((radio[0] & 0x3F) << 8) + radio[1]) * 32768 / 4 - 225000) / 10000;
    frequency = round(frequency * 10.0) / 1000.0;
    frequency = round(frequency * 10.0) / 10.0;

    return frequency;
}