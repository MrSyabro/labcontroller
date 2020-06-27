#include "lab_main.h"
#include "nvs_flash.h"
#include "esp_wifi.h"

#ifndef LAB_WIFI_H
#define LAB_WIFI_H

EventGroupHandle_t wifi_event_group;
#define CONNECTED_BIT BIT0

esp_err_t Lab_WiFi_Init ();
esp_err_t Lab_MQTT_Init ();

#endif
