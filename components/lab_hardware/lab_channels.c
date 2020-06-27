#include <stdint.h>

#include "lab_main.h"
#include "lab_hardware.h"

static const char *TAG = "Lab_Channels";

static uint8_t register_count = 1;
static uint32_t *channels_stats;

esp_err_t Lab_Channels_init_spi()
{
  ESP_LOGD(TAG, "Init SPI");

  channels_stats = malloc(sizeof(uint8_t)*register_count);

  spi_config_t spi_config;
  spi_config.interface.val = SPI_DEFAULT_INTERFACE;
  spi_config.interface.miso_en = 0;

  spi_config.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;
  spi_config.mode = SPI_MASTER_MODE;
  spi_config.clk_div = SPI_10MHz_DIV;
  spi_config.event_cb = NULL;
  spi_init(HSPI_HOST, &spi_config);

  return ESP_OK;
}

void Lab_Channels_send_spi(uint32_t *channels, uint8_t reg_count)
{
  ESP_LOGD(TAG, "Send to register: 0x%0x", *channels);

  spi_trans_t trans;
  memset(&trans, 0x0, sizeof(trans));
  trans.bits.mosi = 8 * reg_count;
  trans.mosi = channels;
  spi_trans(HSPI_HOST, &trans);
}

esp_err_t Lab_Channels_set(uint8_t channel)
{
  ESP_LOGD(TAG, "Channel set");
  *channels_stats = *channels_stats ^ (1 << channel);
  Lab_Channels_send_spi (channels_stats, register_count);

	return ESP_OK;
}

esp_err_t Lab_Channels_reset()
{
  ESP_LOGD(TAG, "Reset register");
  *channels_stats = 1; // Reset state.. Сразу ключается лампа.
  Lab_Channels_send_spi(channels_stats, register_count);

  return ESP_OK;
}

void lab_channels_task(void *params)
{
  ESP_LOGD(TAG, "Start Channels task..");
  uint8_t receive[2];
  while (1)
  {
    //receive = calloc(2, 8);
    //xEventGroupWaitBits(lab_event_group, , true, true, portMAX_DELAY);
    xQueuePeek(lab_message_queue, receive, portMAX_DELAY);
    if (receive[0] == 'c')
    {
      xQueueReceive(lab_message_queue, receive, portMAX_DELAY);
      ESP_LOGD(TAG, "Change channel %d state", receive[1]);
      Lab_Channels_set(receive[1]);
    }
    else if (receive[0] == 'e' && receive[1] == POWER_STATE_ON)
    {
      Lab_Channels_reset();
    }
    //free(receive);
  }

  ESP_LOGD(TAG, "Stop Channels task");
  vTaskDelete(NULL);
}

esp_err_t Lab_Channels_Init()
{
  ESP_LOGD(TAG, "Init start..");

  Lab_Channels_init_spi();

  xTaskCreate(lab_channels_task, "lab_channels_task", configMINIMAL_STACK_SIZE * 2, NULL, 5, NULL);
  ESP_LOGI(TAG, "Init end");

  return ESP_OK;
}
