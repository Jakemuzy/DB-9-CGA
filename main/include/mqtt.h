#ifndef MQTT_H_
#define MQTT_H_

/*
 	 Mqtt Client
	-------------
    Run on dedicated thread.
     Receives binary blobs 
   representing screen data as
   well as notificaiton levels

       Calls display to 
  write them to the back-buffer.

*/

#include "string.h"
 
#include "esp_log.h"

// These could potentially be in config
#define ADDRESS "tcp://localhost:1883"
#define CLIENTID "DB9Client"
#define QOS 1
#define TIMEOUT 10000L

#define TOPIC_BLOB "/db9/blob"
#define TOPIC_CONFIG "/db9/config"
#define TOPIC_SLEEP "/db9/sleep"
#define TOPIC_WAKE "/db9/wake"


/* ----- Functions ----- */

MQTTClient* initialize_mqtt_client();
void destroy_mqtt_client(MQTTClient* client);

void callback_message_arrived(void* context, char* topicName, int topicLen, MQTTClient_message* message);
void callback_message_dropped(void* context, char* cause);

void receive_blob(void* blob, int len);
void receive_config(void* conf int len);
void receive_sleep(void);
void receive_wake(void);

#endif 
