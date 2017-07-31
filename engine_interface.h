#ifndef ENGINE_INTERFACE
#define ENGINE_INTERFACE
#include "engine.h"
/*
    all display engines need to define this interface
    the system will load them dynamically
*/

void engine_init(engine_frame_t*, engine_config_t);
void engine_release(engine_frame_t*);
#endif