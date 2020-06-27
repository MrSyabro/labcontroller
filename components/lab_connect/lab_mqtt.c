#include "lab_connect.h"
#include "lab_main.h"

static const char *TAG = "Lab_MQTT";

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
	client = event->client;
	int msg_id;
	// your_context_t *context = event->context;
	switch (event->event_id)
	{
	case MQTT_EVENT_CONNECTED:
		ESP_LOGD(TAG, "MQTT_EVENT_CONNECTED");

		msg_id = esp_mqtt_client_subscribe(client, "/lab/power", 1);
		ESP_LOGD(TAG, "sent subscribe successful, msg_id=%d", msg_id);

		msg_id = esp_mqtt_client_subscribe(client, "/lab/ch", 1);
		ESP_LOGD(TAG, "sent subscribe successful, msg_id=%d", msg_id);
		break;
	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGD(TAG, "MQTT_EVENT_DISCONNECTED");
		break;
	case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGD(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGD(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_PUBLISHED:
		ESP_LOGD(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_DATA:
		ESP_LOGD(TAG, "MQTT_EVENT_DATA");
		printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
		printf("DATA=%.*s\r\n", event->data_len, event->data);
		uint8_t data[2];
		if (!memcmp(event->topic, "/lab/power", event->topic_len))
		{
			data[0] = 'p';
		}
		else if (!memcmp(event->topic, "/lab/ch", event->data_len))
		{
			data[0] = 'c';
		}
		data[1] = atoi(event->data);
		xQueueSend(lab_message_queue, data, portMAX_DELAY);
		break;
	case MQTT_EVENT_ERROR:
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
		break;
	}
	return ESP_OK;
}

void mqtt_app_start(void)
{
	xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
	esp_mqtt_client_config_t mqtt_cfg = {
		.uri = CONFIG_BROKER_URL,
		.event_handle = mqtt_event_handler,
	// .user_context = (void *)your_context
#if CONFIG_BROKER_AUTH
		.username = CONFIG_BROKER_USER,
		.password = CONFIG_BROKER_PASS,
#endif
	};

	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(client);
}

void lab_mqtt_task(void *params)
{
	//int group_bits;
	int ps = 0;
	//int latch = 0;

	uint8_t receive[2];
	
	while (1)
	{		
		//receive = malloc(16);

		xQueuePeek(lab_message_queue, receive, portMAX_DELAY);
		if (receive[0] == 'e') 
		{
			xQueueReceive(lab_message_queue, receive, portMAX_DELAY);
			if (receive[1] == POWER_STATE_ON)
				esp_mqtt_client_publish(client, "/lab/power/s", "1", 0, 1, 0);
			else if (receive[1] == POWER_STATE_OFF)
				esp_mqtt_client_publish(client, "/lab/power/s", "0", 0, 1, 0);
			else if (receive[1] == POWER_STATE_IDLE)
				esp_mqtt_client_publish(client, "/lab/power/s", "2", 0, 1, 0);
			else if (receive[1] == POWER_STATE_ERR)
				esp_mqtt_client_publish(client, "/lab/power/s", "3", 0, 1, 1);
			else 
				ESP_LOGE(TAG, "Expceted state value: 0x%0x", ps);
		}

		//free(receive);
	}

	ESP_LOGD(TAG, "Stop mqtt task");
	vTaskDelete(NULL);
}

esp_err_t Lab_MQTT_Init()
{
	ESP_LOGD(TAG, "Init start..");
	mqtt_app_start();
	xTaskCreate(lab_mqtt_task, "lab_mqtt_task", configMINIMAL_STACK_SIZE * 2, NULL, 5, NULL);
	ESP_LOGI(TAG, "Init end");
	
	return ESP_OK;
}
