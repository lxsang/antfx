#ifndef ENGINE_INTERFACE
#define ENGINE_INTERFACE
#include "types.h"
/*
    all display engines need to define this interface
    the system will load them dynamically
*/

void display_init(engine_frame_t*, engine_config_t);
void display_release(engine_frame_t*);
#endif