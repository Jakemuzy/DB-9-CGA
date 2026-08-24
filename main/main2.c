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
#include "config.h"

#include "config.h"


static const char* TAG = "DB9 (main)";



void app_main()
{
    // Creates esp lcd obj
    //

    ESP_LOGI(TAG, "Clearing Screen...");

    ESP_LOGI(TAG, "Drawing...");
    BufferInfo *info = malloc(sizeof(BufferInfo));
    info->screen_width = SCREEN_WIDTH;
    info->screen_height = SCREEN_HEIGHT;
    info->handle = panel_handle;
    info->fb1 = fb1;
    info->fb2 = fb2;
    info->draw_buf = fb1;

    Color color = DB9_RED;
    while (1) {
        buffer_draw_circle(
            info,
            (uint16_t)(SCREEN_WIDTH / 2),
            (uint16_t)(SCREEN_HEIGHT /2),
            (uint16_t)(SCREEN_WIDTH / 8),
            color,
            DB9_DARK
        );

        swap_buffers(info);

        ESP_LOGI(TAG, "Finished drawing circle.");
	vTaskDelay(pdMS_TO_TICKS(300));
    }

}
