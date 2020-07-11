#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "mqtt_client.h"
#include "esp_system.h"

#include "esp_event_loop.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "esp_log.h"

#ifndef MESSAGING_H
#define MESSAGING_H

const static int CONNECTED_BIT = BIT0;
esp_mqtt_client_handle_t client;

EventGroupHandle_t wifi_event_group;

void mqtt_app_start(void);
void wifi_init();
void send_message(char* channel, char* message);
#endif