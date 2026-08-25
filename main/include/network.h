#ifndef DB9_NETWORK_H_
#define DB9_NETWORK_H_

/*
       Network Events
	-------------
    Callbacks for network
        driven events

*/

#include <stdint.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_netif_types.h"
#include "esp_event.h"


#include "config.h"


/* ----- Struct ----- */

typedef struct NetworkCallback {
    esp_event_base_t event_base;
    int32_t event_id;

    void* (*function)(void*);
    void* output;
} NetworkCallback;


/* ----- Functions & Callbacks ----- */

void initialize_network_stack(void);
void initialize_network_events(NetworkCallback* _callbacks, uint16_t _num_callbacks);

void network_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

void network_wifi_start();
void network_wifi_disconnect();
void network_wifi_ip_acquired();

#endif
