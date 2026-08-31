#include "network.h"

/* ----- Persistent Variables ----- */

static NetworkCallback* callbacks;
static uint16_t num_callbacks;


/* ----- Functions & Callbacks ----- */

void initialize_network_stack()
{
    // Wifi storage
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
	ESP_ERROR_CHECK(nvs_flash_erase());
	ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Init net interface and event loop
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    // Register custom callbacks
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &network_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &network_event_handler, NULL, NULL));

    // Configurations
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    strncpy((char*)wifi_config.sta.ssid, secret_get_wifi_ssid(),
        sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char*)wifi_config.sta.password, secret_get_wifi_password(),
            sizeof(wifi_config.sta.password) - 1);
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // Start 
    ESP_ERROR_CHECK(esp_wifi_start());
    esp_wifi_set_ps(WIFI_PS_NONE); 
}

void initialize_network_events(NetworkCallback* _callbacks, uint16_t _num_callbacks)
{
    callbacks = _callbacks;
    num_callbacks = _num_callbacks;
}

void network_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        return;
    }
    
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        return;
    }


    // Iterate the custom events
    for (size_t i = 0; i < num_callbacks; i++) {
	NetworkCallback* callback = &callbacks[i];

	if (event_base == callback->event_base && event_id == callback->event_id) {
  	     callback->output = callback->function(NULL);
	}
     }
}

