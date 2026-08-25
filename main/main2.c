/* 
 
			    FOREWORD

	The Samsung Santron SC-452C was a 1989 CRT monitor.
	   It features 16-bit color depth, and sports 
	    a legacy DB-9 CGA video input connector.
	 For purposes of this project, we aim to recreate
	     this video output via modern hardware.

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "soc/clk_tree_defs.h"

#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"

#include "secrets.h"
#include "config.h"
#include "network.h"
#include "mqtt.h"
#include "display.h"


static const char* TAG = "DB9 (main)";


void app_main()
{
    NetworkCallback network_event = { 
    	.event_base = IP_EVENT,
	.event_id = IP_EVENT_STA_GOT_IP,
  	.function = initialize_mqtt_client,
	.output = NULL
    };
    initialize_network_events(&network_event, 1);
    init_network_stack();

    while (network_event.output == NULL) {
	ESP_LOGI(TAG, "Wifi not found yet, scanning...");
	vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_LOGI(TAG, "Initializing screen buffers...");
    BufferInfo *info = initialize_buffer_info();
    ESP_LOGI(TAG, "SUCCESS: initialized screen buffers.");

    while (1) 
    {
        swap_buffers(info);
	vTaskDelay(pdMS_TO_TICKS(300));
    }

    // Disconnect from mqtt
    if (network_event.output != NULL) {
	ESP_LOGI(TAG, "Destroying mqtt client...");

        esp_mqtt_client_handle_t client = network_event.output;
        destroy_mqtt_client(client);

	ESP_LOGI(TAG, "SUCCESS: destroyed mqtt client...");
    }
}
