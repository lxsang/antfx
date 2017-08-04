#include "supports.h"

engine_frame_t _screen;


uint8_t* screen_position(point_t p)
{
    if(p.x < 0 || p.y < 0 || p.x >= _screen.width || p.y >= _screen.height) return NULL;
    int loc  = (p.x+_screen.xoffset) * (_screen.bbp/8) + (p.y+_screen.yoffset) * _screen.line_length;
    
#ifdef USE_BUFFER
    return _screen.swap_buffer+loc;
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
static void _draw_line_(point_t from, point_t to, uint8_t _stroke, color_code_t code,point_t tr)
{ 
    if(_stroke == 0) return;
    line_t o;
    o.from = _T(from,tr);
    o.to = _T(to,tr);
    point_t px;

    signed char offset,dir;
    uint8_t stroke;
    
    int16_t dx, dy, sx,sy,err,e2;
 
    dx =  abs (o.from.x - o.to.x);
    sx = o.from.x  < o.to.x ? 1 : -1;
    dy = -abs (o.from.y - o.to.y);
    sy = o.from.y < o.to.y ? 1 : -1; 
    err = dx + dy; /* error value e_xy */
 
    px = o.from;
    for (;;){  /* loop */
        _put_pixel(px,code);
        stroke = _stroke-1;
        dir = 1;
        offset = 1;
        if(dx != 0 && dy != 0) stroke=(uint8_t)ceilf((float)stroke*3.0/2.0);
        while(stroke>0)
        {
            _put_pixel(_T(px,(_P(0,offset*dir))),code);
            if(dir == -1) 
                offset++;
            dir = -dir;
            stroke--;
        }
        if (px.x == o.to.x && px.y == o.to.y) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; px.x += sx; } /* e_xy+e_x > 0 */
        if (e2 <= dx) { err += dx; px.y += sy; } /* e_xy+e_y < 0 */
    }
}
void _draw_line(line_t l,point_t tr)
{
    if(l.stroke == 0) return;
    color_code_t code = COLOR(l.color);
    _draw_line_(l.from,l.to,l.stroke,code,tr);
}
static void _draw_horizon_line(point_t from, point_t to, color_code_t cfill)
{
    int i;
    if(from.y != to.y) return;
    uint8_t* mem;
    for(i= from.x; i <= to.x; i++)
        _put_pixel((point_t){i, from.y},cfill);
}
static point_t _draw_circle_stroke(point_t org,point_t off, uint16_t r,  color_code_t c, uint8_t stroke)
{
    float sinx = (float)off.y/(float)r;
    float cosx = (float)off.x/(float)r;
    //LOG("stroke %d\n",stroke);
    _put_pixel(_T(off,org),c);
    stroke--;
    while(stroke > 0)
    {
        if(off.x == 0) return off;
        r--;
        off.x = (int16_t)ceilf((float)r*cosx);
        off.y = (int16_t)ceilf((float)r*sinx);
        _put_pixel(_T(off,org),c);
        _put_pixel(_T(org,_P(off.x,off.y-1)),c);
        _put_pixel(_T(org,_P(off.y,off.x)),c);
        _put_pixel(_T(org,_P(off.y,off.x-1)),c);
        _put_pixel(_T(org,_P(-off.y,off.x)),c);
        _put_pixel(_T(org,_P(-off.y,off.x-1)),c);
        _put_pixel(_T(org,_P(-off.x,off.y)),c);
        _put_pixel(_T(org,_P(-off.x,off.y-1)),c);
        _put_pixel(_T(org,_P(-off.x,-off.y)),c);
        _put_pixel(_T(org,_P(-off.x,-off.y+1)),c);
        _put_pixel(_T(org,_P(-off.y,-off.x)),c);
         _put_pixel(_T(org,_P(-off.y,-off.x+1)),c);
        _put_pixel(_T(org,_P(off.y,-off.x)),c);
        _put_pixel(_T(org,_P(off.y,-off.x+1)),c);
        _put_pixel(_T(org,_P(off.x,-off.y)),c);
        _put_pixel(_T(org,_P(off.x,-off.y+1)),c);
        stroke--;
    }
    r--;
    off.x = (int16_t)ceilf((float)r*cosx);
    off.y = (int16_t)ceilf((float)r*sinx);
    return off;
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
    //_draw_circle_stroke(_P(50,10),_P(70,89),code,3);
    int i;
    while (x >= y)
    {
         int16_t  _x  = x, _y = y;
        if(cir.stroke)
        {
            point_t inner = _draw_circle_stroke(org,_P(x,y),cir.r,code,cir.stroke);
            _x = inner.x;
            _y = inner.y;
        }
        if(cir.fill)
        {
            _draw_horizon_line(_T(org,_P(-_x,_y)),_T(org,_P(_x,_y)),bcode);
            _draw_horizon_line(_T(org,_P(-_y,_x)),_T(org,_P(_y,_x)),bcode);
            _draw_horizon_line(_T(org,_P(-_x,-_y)),_T(org,_P(_x,-_y)),bcode);
            _draw_horizon_line(_T(org,_P(-_y,-_x)),_T(org,_P(_y,-_x)),bcode);
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
void _draw_polygon(polygon_t p, point_t tr)
{
    int i ;
    color_code_t code = COLOR(p.color);
    for(i=0;i < p.size - 1;i++)
        _draw_line_(p.points[i],p.points[i+1],p.stroke,code,tr);
    if(p.connected)
        _draw_line_(p.points[i],p.points[0],p.stroke,code,tr);
}

void _draw_composite(composite_t comp,point_t tr)
{
    shape_t s;
    for_each(s,comp.shapes)
    {
        switch(s->type)
        {
            case S_LINE: 
                _draw_line(*((line_t*)s->value),comp.at);
                break;
            case S_CIRCLE: 
                _draw_circle(*((circle_t*)s->value),comp.at);
                break; 
            case S_RECT:
                _draw_rect(*((rect_t*)s->value),comp.at);
                break;
            case S_POLY: 
                _draw_polygon(*((polygon_t*)s->value),comp.at);
                break;
            case S_COMPOSITE: 
                _draw_composite(*((composite_t*)s->value),comp.at);
                break;
            default: ;
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
    uint8_t *mem = _screen.swap_buffer;
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
            code.value = (int)( (int)ceilf((float)px.r * 31.0 / 255.0) << 11 |  (int)ceilf((float)px.g * 63.0 / 255.0) << 5  | (int)ceilf((float)px.b * 31.0 / 255.0) );
            //*((unsigned short*)mem) = pixet_to_565(px);
            break;
        default:
            code.value = 255; // we do not support other color
    }
    return code;
}
point_t _T(point_t a,point_t b){ 
    return (point_t){(int16_t)(a.x+b.x),(int16_t)(a.y+b.y)};
}
void antfx_init(engine_config_t conf)
{
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