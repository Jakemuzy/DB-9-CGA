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
#include "mqtt_client.h"

#include "display.h"
#include "buzzer.h"

// These could potentially be in config
#define ADDRESS "mqtt://192.168.68.102"
#define CLIENTID "DB9Client"

#define TOPIC_BLOB "/db9/blob"
#define TOPIC_CONFIG "/db9/config"
#define TOPIC_POWER "/db9/power"
#define TOPIC_BUZZER "/db9/buzzer"


/* ----- Functions ----- */

void* initialize_mqtt_client(void* args);
void destroy_mqtt_client(esp_mqtt_client_handle_t client);

void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data);
void mqtt_reassamble_packet(void* event_data);
void mqtt_dispatch_event(void);

void receive_blob(void* blob, int len);
void receive_config(void* conf, int len);
void receive_power(void);
void receive_buzzer(char* notification_level, int len);

#endif 
