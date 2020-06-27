#include <stdint.h>

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lab_main.h"
#include "lab_connect.h"
#include "lab_hardware.h"

static const char *TAG = "Lab_Main";

void app_main()
{
	ESP_LOGI(TAG, "Startup..");
	ESP_LOGI(TAG, "Free memory: %d bytes", esp_get_free_heap_size());
	ESP_LOGI(TAG, "IDF version: %s", esp_get_idf_version());

	/* MAIN CODE */
	
	lab_stop = 1; // Условие всех циклов (если 0 - все остановятся)

	lab_power_state_group = xEventGroupCreate();
	lab_message_queue = xQueueCreate(10, sizeof(uint8_t)*2); // 1: opt. 2: st.
	
	ESP_LOGI(TAG, "Start initializing...");

	ESP_ERROR_CHECK(Lab_Power_Init ());
	ESP_ERROR_CHECK(Lab_Channels_Init ());
	ESP_ERROR_CHECK(Lab_WiFi_Init ());
	ESP_ERROR_CHECK(Lab_MQTT_Init ());
	//ESP_ERROR_CHECK(Lock_TButton_Init ());
}
