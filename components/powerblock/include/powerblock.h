#include <stdint.h>

#ifndef POWERBLOCK_H
#define POWERBLOCK_H

#define PB_ON_PIN 5
#define PB_ON_SEL (1ULL<<PB_ON_PIN)

#define PB_S_PIN 4
#define PB_S_SEL (1ULL<<PB_S_PIN)

typedef enum pb_state_e {
  pb_on,
  pb_off,
  pb_idle
} pb_state_t;

#define PB_ON 0
#define PB_OFF 1
#define PB_IDLE 2

void pb_init();
void pb_set(uint8_t s);

#endif
