#include "messaging.h"
#include "channels.h"
#include "powerblock.h"

static const char *TAG = "LABC_MQTT";

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
	client = event->client;
	int msg_id;
	// your_context_t *context = event->context;
	switch (event->event_id)
	{
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
		msg_id = esp_mqtt_client_subscribe(client, "/lab/power", 1);
		ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

		msg_id = esp_mqtt_client_subscribe(client, "/lab/ch", 1);
		ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
		break;
	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
		break;
	case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
		break;
	case MQTT_EVENT_DATA:
		ESP_LOGI(TAG, "MQTT_EVENT_DATA");
		printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
		printf("DATA=%.*s\r\n", event->data_len, event->data);
		if (!memcmp(event->topic, "/lab/power", event->topic_len))
		{
			if (event->data[0] == '1')
				pb_set(PB_ON);
			else if (event->data[0] == '0')
				pb_set(PB_OFF);
		}
		else if (!memcmp(event->topic, "/lab/ch", event->topic_len))
		{
			ch_set(atoi(event->data));
		}
		break;
	case MQTT_EVENT_ERROR:
		ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
		break;
	}
	return ESP_OK;
}

void send_message(char* channel, char* message)
{
    esp_mqtt_client_publish(client, channel, message, 0, 1, 0);
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

	client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(client);
}