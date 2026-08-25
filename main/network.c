#include "network.h"

typedef struct NetworkCallback {
    esp_event_base_t event_base;
    int32_t event_id;

    void* (*function)(void*);
    void* output;
} NetworkCallback;

static NetworkCallback* callbacks;
static uint16_t num_callbacks;


/* ----- Functions & Callbacks ----- */

void initialize_network_events(NetworkCallback* _callbacks, uint16_t _num_callbacks)
{
    callbacks = _callbacks;
    num_callbacks = _num_callbacks;
}

void network_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    for (size_t i = 0; i < num_callbacks; i++) {
	NetworkCallback callback = callbacks[i];

	if (event_base == callback.event_base && event_id == callback.event_id) {
  	     callack.output = (void*)callback.function();
	}
     }
}

