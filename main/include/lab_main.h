#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#ifndef LOCK_MAIN
#define LOCK_MAIN

#define POWER_STATE_ON      BIT0
#define POWER_STATE_ERR     BIT1
#define POWER_STATE_OFF     BIT2
#define POWER_STATE_IDLE    BIT3

#define POWER_STATE         (POWER_STATE_ON | POWER_STATE_OFF | POWER_STATE_IDLE | POWER_STATE_ERR)
//#define POWER_OFF_EVENT     BIT3
//#define CHANNELS_CHANGE     BIT4

EventGroupHandle_t lab_power_state_group;

QueueHandle_t lab_message_queue;

static uint8_t lab_stop;

esp_mqtt_client_handle_t client;

#endif
