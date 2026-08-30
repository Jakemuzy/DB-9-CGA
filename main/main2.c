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
    initialize_network_stack();

    while (network_event.output == NULL) {
        // TODO: Display a screen saying "loading mqtt connection..." hardcoded binary blob in a .txt file
        ESP_LOGI(TAG, "Wifi not found yet, scanning...");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }


    // Creates the shared static mutex
    display_init();

    // Rendering on background thread
    xTaskCreatePinnedToCore(
        display_task,      
        "display_task",   
        4096,              
        NULL,              
        5,                 // Highest priority
        NULL,              
        1                  // 1st core
    );

}
