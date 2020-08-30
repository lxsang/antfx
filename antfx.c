#include "antfx.h"

engine_frame_t _screen;

static void render(lv_disp_t * disp, const lv_area_t * area, lv_color_t * color_p)
{
    int32_t x, y;
    int code, loc;
    int size = _screen.bbp/8;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            //if(area->y2 < _screen.height && area->x2 < _screen.width)
            //{
                loc  = (x+_screen.xoffset) * (_screen.bbp/8) + (y+_screen.yoffset) * _screen.line_length;
                memcpy(_screen.buffer+loc,&(*color_p).full,size);
                //_screen.buffer[loc]  = (*color_p).full;
            //}
            color_p++;
            //code = (int)( (int)ceilf((float)(*color_p).ch.red * 31.0 / 255.0) << 11 |  (int)ceilf((float)(*color_p).ch.green * 63.0 / 255.0) << 5  | (int)ceilf((float)(*color_p).ch.blue * 31.0 / 255.0) );
            //set_pixel(x, y, *color_p);  /* Put a pixel to the display.*/
            
        }
    }

    lv_disp_flush_ready(&disp->driver);
#ifdef USE_SDL2
    display_update(&_screen);
#endif
}

static bool pointer_input(lv_indev_drv_t * indev_driver, lv_indev_data_t * data)
{
    uint8_t last_type = _screen.pointer.evt.type;
    get_pointer_input(&_screen);

    switch (_screen.pointer.evt.type)
    {
    case AFX_EVT_DOWN:
        data->state = LV_INDEV_STATE_PR;
        break;
    case AFX_EVT_RELEASE:
        data->state = LV_INDEV_STATE_REL;
        break;
    case AFX_EVT_MOVE:
        _screen.pointer.evt.type = last_type;
        data->state = LV_INDEV_STATE_REL;
        if(last_type == AFX_EVT_DOWN)
        {
            data->state = LV_INDEV_STATE_PR;
        }
        break;
    default:
        break;
    }
    data->point.x = _screen.pointer.evt.data[0];
    data->point.y = _screen.pointer.evt.data[1];
    return false;
}

void antfx_init(engine_config_t conf)
{
    //SYS_FONT = (afx_font_t){NULL,0,NULL,0,NULL,0,0,0,0};
    display_init(&_screen,conf);
    lv_init();
    lv_disp_buf_init(&_screen.disp_buf,_screen.color_buf , NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX); 
    lv_disp_drv_init(&_screen.disp_drv);
    _screen.disp_drv.flush_cb = render;
    _screen.disp_drv.buffer = &_screen.disp_buf;
    lv_disp_drv_register(&_screen.disp_drv);

    // input event
    lv_indev_drv_init(&_screen.pointer.driver);             /*Descriptor of a input device driver*/
    _screen.pointer.driver.type = LV_INDEV_TYPE_POINTER;    /*Touch pad is a pointer-like device*/
    _screen.pointer.driver.read_cb = pointer_input;      /*Set your driver function*/
    lv_indev_drv_register(&_screen.pointer.driver);                  /*Finally register the driver*/
    //lv_indev_init();
}
void antfx_release()
{
      display_release(&_screen);
#ifdef USE_BUFFER
    if(_screen.swap_buffer)
        free(_screen.swap_buffer);
#endif
}