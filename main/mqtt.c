#include "mqtt.h"

static const char* TAG = "DB9 (mqtt)";


MQTTClient* initialize_mqtt_client()
{
    MQTTClient* client = malloc(sizeof(MQTTClient));
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;

    rc = MQTTClient_create(
	client, 
	ADDRESS, 
	CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, 
	NULL
    );

    if (rc != MQTTCLIENT_SUCCESS) {
        ESP_LOGE(TAG, "Failed to create MQTT client instance, rc=%d", rc);
        free(client);
        return NULL;
    }


    MQTTClient_setCallbacks(
	*client, 
	NULL, 
	callback_message_dropped, 
	callback_message_arrived, 
	NULL
    );

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(*client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
	ESP_LOGE(TAG, "Failed to establish MQTT broker connection links, rc=%d", rc);
        MQTTClient_destroy(client);
	return NULL;
    }

    MQTTClient_subscribe(*client, TOPIC_BLOB, 1);
    MQTTClient_subscribe(*client, TOPIC_CONFIG, 1);
    MQTTClient_subscribe(*client, TOPIC_SLEEP, 1);
    MQTTClient_subscribe(*client, TOPIC_WAKE, 1);

    ESP_LOGI(TAG, "MQTT subsystem successfully linked and monitoring topics.");
    return client;
}

void destroy_mqtt_client(MQTTClient* client)
{
    MQTTClient_disconnect(*client, 10000);
    MQTTClient_destroy(client);
    free(client);
}





void callback_message_arrived(void* context, char* topicName, int topicLen, MQTTClient_message* message)
{
    if (!message || !message->payload) return 1;

    // Route the message
    if (strcmp(topicName, TOPIC_BLOB) == 0) {
        receive_blob(message->payload, message->payloadlen);
    } 
    else if (strcmp(topicName, TOPIC_CONFIG) == 0) {
        receive_config(message->payload, message->payloadlen);
    } 
    else if (strcmp(topicName, TOPIC_SLEEP) == 0) {
        receive_sleep();
    } 
    else if (strcmp(topicName, TOPIC_WAKE) == 0) {
        receive_wake();
    } 
    else {
        ESP_LOGW(TAG, "Unhandled topic update received: %s", topicName);
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
}

void callback_message_dropped(void* context, char* cause)
{
    ESP_LOGE(TAG, "MQTT Connection dropped. Cause: %s. Initiating retry pipeline...", cause ? cause : "Unknown");
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
