#include "supports.h"

engine_frame_t _screen;

#ifdef USE_BUFFER
uint8_t* _buffer;
#endif

uint8_t* screen_position(point_t p)
{
    if(p.x < 0 || p.y < 0 || p.x >= _screen.width || p.y >= _screen.height) return NULL;
    int loc  = (p.x+_screen.xoffset) * (_screen.bbp/8) + (p.y+_screen.yoffset) * _screen.line_length;
    
#ifdef USE_BUFFER
    return _buffer+loc;
#else
    return _screen.buffer+loc;
#endif
}

void _put_pixel(point_t at,color_code_t c)
{
    uint8_t* mem = screen_position(at);
    if(mem)
        memcpy(mem,&c.value,c.size);
}
void _draw_point(point_t _at,color_code_t code, point_t tr)
{
    point_t at = _T(_at,tr);
    _put_pixel(at,code);
}
void _draw_line(line_t l,color_code_t code,point_t tr)
{ 
    if(l.stroke == 0) l.stroke = 1;
    line_t o;
    o.from = _T(l.from,tr);
    o.to = _T(l.to,tr);
    point_t p;
    int a,b;
    a = (o.to.y - o.from.y)/(o.to.x - o.from.x);
    b = o.from.y - a*o.from.x;
    uint8_t* mem;
    int i;
    for(i= o.from.x; i <= o.to.x;i++)
    {
        p.x = i;
        p.y = p.x*a + b;
        mem = screen_position(p);
        if(mem)
            memcpy(mem ,&(code.value),code.size);
    }
    l.stroke--;
    if(l.stroke == 0) return;
    //int offset = 1;
    line_t err_line1,err_line2;

    err_line1.stroke =1;
    err_line1.from = l.from;
    err_line1.to = l.to;

    err_line2.stroke =1;
    err_line2.from = l.from;
    err_line2.to = l.to;

    while(l.stroke > 0)
    {

        // draw top
        err_line1.from = _T(err_line1.from,(point_t){1,0});
        err_line1.to = _T(l.to,(point_t){0,-1});
        _draw_line(err_line1,code,tr);
        //bot 
        err_line2.from = _T(err_line2.from,(point_t){0,1});
        err_line2.to = _T(err_line2.to,(point_t){-1,0});
        _draw_line(err_line2,code,tr);
        
        l.stroke--;
    }
}

void _draw_horizon_line(point_t from, point_t to, color_code_t cfill)
{
    int i;
    if(from.y != to.y) return;
    uint8_t* mem;
    for(i= from.x; i <= to.x; i++)
        _put_pixel((point_t){i, from.y},cfill);
}

void _draw_circle(circle_t cir, point_t tr)
{
    if(cir.fill == 0 && cir.stroke == 0) return;
    point_t org = _T(cir.at , tr);
    int x = cir.r-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (cir.r << 1);
    uint8_t* mem;
    color_code_t code = COLOR(cir.color);
    color_code_t bcode = COLOR(cir.bcolor);
    int i;
    while (x >= y)
    {
       if(cir.fill)
       {
           _draw_horizon_line(_T(org,(point_t){-x,y}),_T(org,(point_t){x,y}),bcode);
           _draw_horizon_line(_T(org,(point_t){-y,x}),_T(org,(point_t){y,x}),bcode);
           _draw_horizon_line(_T(org,(point_t){-x,-y}),_T(org,(point_t){x,-y}),bcode);
           _draw_horizon_line(_T(org,(point_t){-y,-x}),_T(org,(point_t){y,-x}),bcode);
       }

        if(cir.stroke>0)
        {


            _put_pixel(_T(org,(point_t){x,y}),code);
            _put_pixel(_T(org,(point_t){y,x}),code);
            _put_pixel(_T(org,(point_t){-y,x}),code);
            _put_pixel(_T(org,(point_t){-x,y}),code);
            _put_pixel(_T(org,(point_t){-x,-y}),code);
            _put_pixel(_T(org,(point_t){-y,-x}),code);
            _put_pixel(_T(org,(point_t){y,-x}),code);
            _put_pixel(_T(org,(point_t){x,-y}),code);
        }

        if (err <= 0)
        {
            y++;
            err += dy;
            dy +=2;
        }
        if (err > 0)
        {
            x--;
            dx += 2;
            err += (-cir.r << 1) + dx;
        }
    }
}

void _draw_rect(rect_t rect, point_t tr)
{
    int i,j;
    uint8_t* mem;
    point_t p;
    point_t org = _T(rect.at,tr);
    color_code_t code = COLOR(rect.color);
    color_code_t bcode = COLOR(rect.bcolor);
    if(rect.stroke == 0 && rect.fill==0) return;
    for(i = 0;i < rect.of.y; i++)
        for(j=0;j< rect.of.x; j++)
        {
            mem = screen_position(_T(org,(point_t){j,i}));
            if(mem)
            { 
                if(i < rect.stroke || i > rect.of.y - 1 - rect.stroke || j < rect.stroke || j > rect.of.x - 1 - rect.stroke)
                {
                     memcpy(mem,&code.value,code.size);
                     if(rect.fill == 0 && j == rect.stroke && i > rect.stroke && i < rect.of.y - rect.stroke)
                        j = rect.of.x - rect.stroke;
                }
                else if(rect.fill)
                    memcpy(mem,&bcode.value,bcode.size);
            }
        }
}
void _clear(color_code_t code)
{
#ifdef USE_BUFFER
    uint8_t *mem = _buffer;
#else
    uint8_t *mem = _screen.buffer;
#endif
    
    int i= 0;
    while(i < _screen.size)
    {
        memcpy(mem,&code.value,code.size);
        mem += code.size;
        i += code.size;
    }
}
color_code_t _color_code(pixel_t px,uint8_t bbp)
{
    color_code_t code;
    code.size = bbp/8;
    switch(code.size)
    {
        case COLOR8888: 
            code.value = (int)(px.a << 24 | px.r << 16 | px.g << 8 | px.b);
            break;
        case COLOR565:
            code.value = (int)( (px.r * 31 / 25) << 11 |  (px.g * 63 / 255) << 5  | (px.b * 31 / 255) );
            //*((unsigned short*)mem) = pixet_to_565(px);
            break;
        default:
            code.value = 255; // we do not support other color
    }
    return code;
}
point_t _T(point_t a,point_t b){ 
    return (point_t){a.x+b.x,a.y+b.y};
}
void antfx_init(engine_config_t conf)
{
    engine_init(&_screen,conf);
#ifdef USE_BUFFER
    if(_screen.buffer)
        _buffer = (uint8_t*) malloc(_screen.size);
#endif

}
void antfx_release()
{
      engine_release(&_screen);
#ifdef USE_BUFFER
    if(_buffer)
        free(_buffer);
#endif
}

void render()
{
#ifdef USE_BUFFER
    memcpy(_screen.buffer, _buffer, _screen.size);
#endif
}