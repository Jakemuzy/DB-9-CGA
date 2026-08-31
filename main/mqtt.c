#include "mqtt.h"

static const char* TAG = "DB9 (mqtt)";

// For reassembling messages
static uint8_t* rx_buffer = NULL;
static int rx_buffer_len = 0;
static int rx_received = 0;
static char rx_topic[32];
static int rx_topic_len = 0;

void* initialize_mqtt_client(void* args) // Args ignored for now (required to be NetworkCallback)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = ADDRESS,
        .credentials.client_id = CLIENTID,
        .buffer.size = 8192,       
        .buffer.out_size = 8192,

    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize native MQTT configuration structural bounds.");
        return NULL;
    }

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    // Starts the background thread automatically
    esp_err_t err = esp_mqtt_client_start(client);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start MQTT loop engine client thread, err=%d", err);
        esp_mqtt_client_destroy(client);
        return NULL;
    }

    return (void*)client;
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
            esp_mqtt_client_subscribe(client, TOPIC_BUZZER, 1);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGE(TAG, "MQTT Connection dropped. Automatic reconnection layer active.");
            break;

        case MQTT_EVENT_DATA:
            mqtt_reassamble_packet(event_data);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT internal engine error occurred.");
            break;

        default:
            break;
    }
}

// Need to reassemble larger packets because they get split into a bunch of smaller ones
void mqtt_reassamble_packet(void* event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    if (event->current_data_offset == 0) {
        if (rx_buffer) {
            ESP_LOGW(TAG, "New message started before previous one finished, discarding partial buffer.");
            free(rx_buffer);
            rx_buffer = NULL;
        }

        rx_buffer_len = event->total_data_len;
        rx_received = 0;
        rx_topic_len = event->topic_len < (int)sizeof(rx_topic) - 1 ? event->topic_len : (int)sizeof(rx_topic) - 1;
        memcpy(rx_topic, event->topic, rx_topic_len);
        rx_topic[rx_topic_len] = '\0';

        if (rx_buffer_len > 0) {
            // Allocate to SPRAM if above a certain size, otherwise SRAM is fine
            rx_buffer = heap_caps_malloc(rx_buffer_len, MALLOC_CAP_SPIRAM);
            if (rx_buffer == NULL) {
                ESP_LOGE(TAG, "Failed to allocate %d bytes for incoming message", rx_buffer_len);
                rx_buffer_len = 0;
                return;
            }
        }
    }

    if (event->data_len > 0) {
        if (rx_buffer == NULL || rx_received + event->data_len > rx_buffer_len) {
            ESP_LOGW(TAG, "Unexpected fragment, discarding message.");
            if (rx_buffer) { free(rx_buffer); rx_buffer = NULL; }
            rx_buffer_len = 0;
            return;
        }
        memcpy(rx_buffer + rx_received, event->data, event->data_len);
        rx_received += event->data_len;
    }

    if (rx_received < rx_buffer_len) {
        return; // still waiting on more fragments
    }

    mqtt_dispatch_event();

    if (rx_buffer) { free(rx_buffer); rx_buffer = NULL; }
    rx_buffer_len = 0;
    rx_received = 0;
}

void mqtt_dispatch_event(void)
{

    if (strncmp(rx_topic, TOPIC_BLOB, rx_topic_len) == 0) {
        receive_blob(rx_buffer, rx_buffer_len);
    }
    else if (strncmp(rx_topic, TOPIC_CONFIG, rx_topic_len) == 0) {
        receive_config(rx_buffer, rx_buffer_len);
    }
    else if (strncmp(rx_topic, TOPIC_SLEEP, rx_topic_len) == 0) {
        receive_sleep();
    }
    else if (strncmp(rx_topic, TOPIC_WAKE, rx_topic_len) == 0) {
        receive_wake();
    }
    else if (strncmp(rx_topic, TOPIC_BUZZER, rx_topic_len) == 0) {
        receive_buzzer((char*)rx_buffer, rx_buffer_len);   // Might need to accept len
    }
    else {
        ESP_LOGW(TAG, "Unhandled topic update received: %.*s", rx_topic_len, rx_topic);
    }
}






void receive_blob(void* blob, int len)
{
    ESP_LOGI(TAG, "Processing inbound binary display frame buffer updates (%d bytes)...", len);
    display_receive_blob(blob, len);
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

void receive_buzzer(char* notification_level, int len)
{
    ESP_LOGI(TAG, "Command received: Buzzer...");

    NotificationLevel level;
    if (len == sizeof("NOTIFY_INFO") - 1 && strncmp(notification_level, "NOTIFY_INFO", len) == 0) { level = NOTIFY_INFO; }
    else if (len == sizeof("NOTIFY_ALERT") - 1 && strncmp(notification_level, "NOTIFY_ALERT", len) == 0) { level = NOTIFY_ALERT; }
    else if (len == sizeof("NOTIFY_WARNING") - 1 && strncmp(notification_level, "NOTIFY_WARNING", len) == 0) { level = NOTIFY_WARNING; }
    else if (len == sizeof("NOTIFY_CRITICAL") - 1 && strncmp(notification_level, "NOTIFY_CRITICAL", len) == 0) { level = NOTIFY_CRITICAL; }
    else { level = NOTIFY_INFO; }

    ESP_LOGI(TAG, "NOTIF LEVEL: %.*s", len, notification_level);
    play_tone_buzzer_async(level);
}
