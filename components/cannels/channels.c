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

#include "driver/spi.h"

#include "channels.h"

static const char *TAG = "channels_driver";

typedef enum {
    SPI_SEND = 0,
    SPI_RECV
} spi_master_mode_t;

uint32_t channels_stats = 0;

void ch_set(uint32_t s)
{
  ESP_LOGI(TAG, "Channel set: %d", s);

  uint32_t setted_chanel = 1 << s;
  channels_stats = channels_stats ^ setted_chanel;

  ESP_LOGI(TAG, "Send to register: 0x%0x", channels_stats);

  spi_trans_t trans;
  memset(&trans, 0x0, sizeof(trans));
  trans.bits.mosi = 8;
  trans.mosi = &channels_stats;
  spi_trans(HSPI_HOST, &trans);

  //ESP_LOGI(TAG, "Regster response: 0x%0x", response);
}

void ch_init()
{
  ESP_LOGI(TAG, "init");

  spi_config_t spi_config;
  // Load default interface parameters
  // CS_EN:1, MISO_EN:1, MOSI_EN:1, BYTE_TX_ORDER:1, BYTE_TX_ORDER:1, BIT_RX_ORDER:0, BIT_TX_ORDER:0, CPHA:0, CPOL:0
  spi_config.interface.val = SPI_DEFAULT_INTERFACE;
  spi_config.interface.miso_en = 0;

  spi_config.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;
  spi_config.mode = SPI_MASTER_MODE;
  spi_config.clk_div = SPI_10MHz_DIV;
  spi_config.event_cb = NULL;
  spi_init(HSPI_HOST, &spi_config);

  ch_set(channels_stats);

}
