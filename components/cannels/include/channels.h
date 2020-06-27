#include <stdint.h>

#ifndef BACKLIGHT_H
#define BACKLIGHT_H

#define PWM_L_OUT_IO_NUM   12

#define PWM_PERIOD    (500)

void ch_init();
void ch_set(uint32_t s);

#endif
