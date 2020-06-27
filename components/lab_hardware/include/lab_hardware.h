#include "lab_main.h"
#include "driver/gpio.h"
#include "driver/spi.h"

#ifndef LAB_HARDWARE_H
#define LAB_HARDWARE_H

#define LAB_POWER_ON_PIN   5
#define LAB_POWER_ON_SEL   (1ULL<<LAB_POWER_ON_PIN)

#define LAB_POWER_ST_PIN   4
#define LAB_POWER_ST_SEL   (1ULL<<LAB_POWER_ST_PIN)

#define LAB_HARDWARE_DELAY 100

typedef enum pb_state_e {
  pb_off = 0,
  pb_on,
  pb_idle
} pb_state_t;

esp_err_t Lab_Channels_Init ();
esp_err_t Lab_Power_Init ();

esp_err_t Lab_Channels_set (uint8_t channel);
esp_err_t Lab_Channels_reset ();

esp_err_t Lab_Power_ch (uint8_t st);

#endif
