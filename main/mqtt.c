#ifndef DB9_MQTT_H_
#define DB9_MQTT_H_

#include "MQTTClient.h"

#define MQTT_URL "127.0.0.1:6213"
#define MQTT_CLIENT_ID "..."
#define MQTT_USERNAME "jmuzy"
#define MQTT_PASSWORD "123456"  // Not in an .env because local get #rekt

typedef struct Client {
    MQTTClient client;
    
} Client;

Client* create_mqtt_client()
{
    Client* client = malloc(sizeof(Client));

    rc = MQTTClient_create(
        &client->client, 
        MQTT_URL, 
        MQTT_CLIENT_ID, 
        MQTTCLIENT_PERSISTENCE_NONE, 
        NULL
    );

    MQTTClient_connectOptions conn_options = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 10;
    conn_opts.cleansession = 1;
    conn_opts.username = MQTT_USERNAME;
    conn_opts.password = MQTT_PASSWORD;

    rc = MQTTClient_connect(client->, conn_opts);

    return client;
}

#endif