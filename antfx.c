#include "antfx.h"

void antfx_init(engine_config_t conf)
{
    SYS_FONT = (afx_font_t){NULL,0,NULL,0,NULL,0,0,0,0};
    engine_init(&_screen,conf);
#ifdef USE_BUFFER
    if(_screen.buffer)
        _screen.swap_buffer = (uint8_t*) malloc(_screen.size);
#endif

}
void antfx_release()
{
      engine_release(&_screen);
#ifdef USE_BUFFER
    if(_screen.swap_buffer)
        free(_screen.swap_buffer);
#endif
}

void render()
{
#ifdef USE_BUFFER
    memcpy(_screen.buffer, _screen.swap_buffer, _screen.size);
#endif
}