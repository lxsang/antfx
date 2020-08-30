#ifndef hw_H
#define hw_H

#include <stdint.h>
#include "default.h"

typedef struct {
    char dev_new[ANTFX_MAX_STR_BUFF_SZ];
    char dev_del[ANTFX_MAX_STR_BUFF_SZ];
    uint8_t clock_addr;
    uint8_t radio_addr; 
} antfx_i2cdev_t;

void init_hw_clock();
void fm_set_freq(double f);
void fm_mute();
double fm_get_freq();

#endif