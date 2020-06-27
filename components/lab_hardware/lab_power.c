#include <stdint.h>

#include "lab_main.h"
#include "lab_hardware.h"

static const char *TAG = "Lab_Power";

static xQueueHandle gpio_evt_queue = NULL;

#define PB_ON 0
#define PB_OFF 1

static void gpio_isr_handler(void *arg)
{
  uint32_t gpio_num = (uint32_t)arg;
  xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void power_st_gpio_task(void *arg)
{
  gpio_num_t io_num;
  uint8_t message[2];
  uint8_t power_state;
  int gpio_state;

  ESP_LOGD(TAG, "Power state task start");

  while (1)
  {
    if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
    {
      if (io_num == LAB_POWER_ST_PIN)
      {
        gpio_state = gpio_get_level(io_num);
        power_state = xEventGroupGetBits(lab_power_state_group);
        //message = calloc(2, 8);
        message[0] = 'e';
        if (gpio_state && (power_state & POWER_STATE_IDLE))
        {
          xEventGroupClearBits(lab_power_state_group, POWER_STATE_IDLE);
          xEventGroupSetBits(lab_power_state_group, POWER_STATE_ON);
        }
        else if (!gpio_state && (power_state & POWER_STATE_IDLE))
        {
          xEventGroupClearBits(lab_power_state_group, POWER_STATE_IDLE);
          xEventGroupSetBits(lab_power_state_group, POWER_STATE_OFF);
        }
        else
        {
          gpio_set_level(LAB_POWER_ST_PIN, PB_OFF);
          xEventGroupClearBits(lab_power_state_group, POWER_STATE);
          xEventGroupSetBits(lab_power_state_group, POWER_STATE_ERR);
        }

        power_state = xEventGroupGetBits(lab_power_state_group);
        message[1] = power_state;

        ESP_LOGD(TAG, "Power state gpio changed. Send to event: {[0x%0x], [0x%0x]}", message[0], message[1]);
        xQueueSend(lab_message_queue, message, portMAX_DELAY);

        //free(message);
      }
    }
  }

  ESP_LOGD(TAG, "Power state task end");
  vTaskDelete(NULL);
}

void Lab_Power_task(void *arg)
{
  ESP_LOGD(TAG, "Power task start");
  uint8_t receive[2];
  while (1)
  {
    //receive = calloc(2, 8);
    xQueuePeek(lab_message_queue, receive, portMAX_DELAY);
    if (receive[0] == 'p')
    {
      xQueueReceive(lab_message_queue, receive, portMAX_DELAY);
      ESP_ERROR_CHECK(Lab_Power_ch(receive[1]));

      //free(receive);
      //receive = calloc(2, 8);
      uint8_t power_state = xEventGroupGetBits(lab_power_state_group);
      receive[0] = 'e';
      receive[1] = power_state;
      ESP_LOGD(TAG, "Send event: {[0x%0x], [0x%0x]}", receive[0], receive[1]);
      xQueueSend(lab_message_queue, receive, portMAX_DELAY);
    }
    //free(receive);
  }
  ESP_LOGD(TAG, "Power task end");
  vTaskDelete(NULL);
}

esp_err_t Lab_Power_Init()
{
  ESP_LOGD(TAG, "Init");

  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = LAB_POWER_ON_SEL;
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);

  io_conf.intr_type = GPIO_INTR_POSEDGE;
  io_conf.pin_bit_mask = LAB_POWER_ST_SEL;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);

  gpio_set_level(LAB_POWER_ON_PIN, PB_OFF);
  xEventGroupSetBits(lab_power_state_group, POWER_STATE_OFF);

  gpio_set_intr_type(LAB_POWER_ST_PIN, GPIO_INTR_ANYEDGE);

  gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
  xTaskCreate(power_st_gpio_task, "power_st_gpio_task", 2048, NULL, 6, NULL);
  xTaskCreate(Lab_Power_task, "Lab_Power_task", 2048, NULL, 5, NULL);

  gpio_install_isr_service(0);
  gpio_isr_handler_add(LAB_POWER_ST_PIN, gpio_isr_handler, (void *)LAB_POWER_ST_PIN);

  ESP_LOGI(TAG, "Init end");

  return ESP_OK;
}

esp_err_t Lab_Power_ch(uint8_t st)
{
  ESP_LOGD(TAG, "Chage power to: 0x%0x", st);
  //if (st == pb_on || st == pb_off)
  //return ESP_ERR_INVALID_ARG;

  uint8_t power_state = xEventGroupGetBits(lab_power_state_group);
  if (power_state & (POWER_STATE_ERR))
    return ESP_ERR_INVALID_STATE;

  if (power_state != st)
  {
    if (st == pb_on)
      gpio_set_level(LAB_POWER_ON_PIN, PB_ON);
    else if (st == pb_off)
      gpio_set_level(LAB_POWER_ON_PIN, PB_OFF);

    xEventGroupClearBits(lab_power_state_group, POWER_STATE);
    xEventGroupSetBits(lab_power_state_group, POWER_STATE_IDLE);
      
  }
  //else
  //{
  // xQueueSend(lab_message_queue, power_state, portMAX_DELAY);
  //}

  return ESP_OK;
}
