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

#include "display.h"
#include "mqtt.h"
#include "config.h"


static const char* TAG = "DB9 (main)";


void app_main()
{
    // Starts up on another thread
    MQTTClient* client = initialize_mqtt_client();

    

    // 2.) Create buffer for rendering
    BufferInfo *info = initialize_buffer_info();

    // 3.) Run continuously on this thread
    while (1) 
    {
        swap_buffers(info);
	vTaskDelay(pdMS_TO_TICKS(300));
    }

    destroy_mqtt_client(client);

}
