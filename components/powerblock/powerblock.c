#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp8266/spi_struct.h"
#include "esp8266/gpio_struct.h"
#include "esp_system.h"
#include "esp_log.h"

#include "mqtt_client.h"

#include "driver/gpio.h"

#include "powerblock.h"
#include "messaging.h"
#include "channels.h"

extern esp_mqtt_client_handle_t client;

static const char *TAG = "power_driver";

static pb_state_t power_state = pb_on;

static xQueueHandle gpio_evt_queue = NULL;

static void gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void *arg)
{
  uint32_t io_num;

  for (;;) {
    if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
      if (!gpio_get_level (io_num) && power_state == PB_ON)
      {
        gpio_set_level(PB_ON_PIN, PB_OFF);
        power_state = PB_OFF;
      }
      if (gpio_get_level(io_num)) {
        send_message("/lab/power/s", "1");
        ch_reset();
      }
      else send_message("/lab/power/s", "0");

      ESP_LOGI(TAG, "GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));
    }
  }
}

void pb_set(uint8_t s)
{
  if ((s == PB_ON || s == PB_OFF) && s != power_state) {
    gpio_set_level(PB_ON_PIN, s);
    send_message("/lab/power/s", "2");
    power_state = s;
    ESP_LOGI(TAG, "Power set: %d", s);
  } else {
    if (power_state == PB_OFF)
      send_message("/lab/power/s", "0");
    else
      send_message("/lab/power/s", "1");
  }
}

void pb_init()
{
  ESP_LOGI(TAG, "init powerblock");

  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = PB_ON_SEL;
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);

  pb_set(PB_OFF);

  io_conf.intr_type = GPIO_INTR_POSEDGE;
  io_conf.pin_bit_mask = PB_S_SEL;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);

  gpio_set_intr_type(PB_S_PIN, GPIO_INTR_ANYEDGE);

  gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
  xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

  gpio_install_isr_service(0);
  gpio_isr_handler_add(PB_S_PIN, gpio_isr_handler, (void *) PB_S_PIN);
}
