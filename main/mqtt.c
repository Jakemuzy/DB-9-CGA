#include "mqtt.h"

static const char* TAG = "DB9 (mqtt)";


esp_mqtt_client_handle_t initialize_mqtt_client()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URI,
        .credentials.client_id = CLIENTID,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize native MQTT configuration structural bounds.");
        return NULL;
    }

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    // Starts the background daemon thread automatically
    esp_err_t err = esp_mqtt_client_start(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start MQTT loop engine client thread, err=%d", err);
        esp_mqtt_client_destroy(client);
        return NULL;
    }

    return client;
}

void destroy_mqtt_client(esp_mqtt_client_handle_t client)
{
    if (client) {
        esp_mqtt_client_stop(client);
        esp_mqtt_client_destroy(client);
    }
}





void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id) {

        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected. Subscribing to topics...");
            esp_mqtt_client_subscribe(client, TOPIC_BLOB, 1);
            esp_mqtt_client_subscribe(client, TOPIC_CONFIG, 1);
            esp_mqtt_client_subscribe(client, TOPIC_SLEEP, 1);
            esp_mqtt_client_subscribe(client, TOPIC_WAKE, 1);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGE(TAG, "MQTT Connection dropped. Automatic reconnection layer active.");
            break;

        case MQTT_EVENT_DATA:
            if (strncmp(event->topic, TOPIC_BLOB, event->topic_len) == 0) {
                receive_blob(event->data, event->data_len);
            }
            else if (strncmp(event->topic, TOPIC_CONFIG, event->topic_len) == 0) {
                receive_config(event->data, event->data_len);
            }
            else if (strncmp(event->topic, TOPIC_SLEEP, event->topic_len) == 0) {
                receive_sleep();
            }
            else if (strncmp(event->topic, TOPIC_WAKE, event->topic_len) == 0) {
                receive_wake();
            }
            else {
                // Formatting out untrimmed topic data safely via precision flag
                ESP_LOGW(TAG, "Unhandled topic update received: %.*s", event->topic_len, event->topic);
            }
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT internal engine error occurred.");
            break;

        default:
            break;
    }
}






void receive_blob(uint16_t* blob, int len)
{
    ESP_LOGI(TAG, "Processing inbound binary display frame buffer updates (%d bytes)...", len);
}

void receive_config(void* conf, int len)
{
    ESP_LOGI(TAG, "Parsing text geometry structural mutations configurations...");
}

void receive_sleep(void)
{
    ESP_LOGI(TAG, "Command received: Sleeping...");
}

void receive_wake(void)
{
    ESP_LOGI(TAG, "Command received: Waking...");
}
