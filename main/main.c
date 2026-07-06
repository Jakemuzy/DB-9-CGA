#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
#include "esp_log.h"
#include "esp_sleep.h"
#include "soc/clk_tree_defs.h"

#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "driver/gpio.h"

// Measured in Hz
// Center of acceptable the range for desired
#define DESIRED_CLOCK_RATE 14318180
#define VSYNC_RATE 60
#define HSYNC_RATE 15750

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 200

#define RED          GPIO_NUM_4
#define GREEN        GPIO_NUM_5
#define BLUE         GPIO_NUM_6
#define INTENSITY    GPIO_NUM_7
#define H_SYNC       GPIO_NUM_15
#define V_SYNC       GPIO_NUM_16
#define PCLK_PIN     GPIO_NUM_17  // Unused, required to be specified

/* 
   FOREWORD:
	The Samsung Santron SC-452C was a 1989 CRT monitor.
	   It features 16-bit color depth, and sports 
	    a legacy DB-9 CGA video input connector.
	 For purposes of this project, we aim to recreate
	     this video output via modern hardware.

   NOTES:
	- 
*/


static const char* TAG = "ESP32";

void app_main()
{
}

void Db9Clock()
{
    // Define a single configuration for the entire RGB panel
    esp_lcd_rgb_panel_config_t panel_cfg = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .pclk_gpio_num = PCLK_PIN, 
        .vsync_gpio_num = V_SYNC,         
        .hsync_gpio_num = H_SYNC,         
        .de_gpio_num = -1,                // CGA doesn't use Data Enable 

        .data_width = 4,                  
        .data_gpio_nums = {
            RED,        
            GREEN,      
            BLUE,       
            INTENSITY   
        },

	// Porch values straight from IBM hardware documentaiton for CGA, what a weird standard
        .timings = {
            .pclk_hz = DESIRED_CLOCK_RATE,
            .h_res = SCREEN_WIDTH,     
            .v_res = SCREEN_HEIGHT,    
	    .hsync_front_porch = 16,
	    .hsync_pulse_width = 72,
	    .hsync_back_porch = 180,  

	    .vsync_front_porch = 4,
	    .vsync_pulse_width = 3,
	    .vsync_back_porch = 55,    
            .flags = {
                .hsync_idle_low = 1,   // CGA active high
                .vsync_idle_low = 1,
            },
        },
        .flags = {
            .fb_in_psram = false,      
        },
    };

    esp_lcd_panel_handle_t panel_handle = NULL;

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_cfg, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
}


// REMEMBER: hsync and vsync are typically pulled high when writing. When pulled low it tells
// the hardware to switch scanlines. Vsync is pulled low when it hits the bottom right of the screen
// indicating that a new frame is about to be drawn and to move the ray to the top left of the screen.
// Hsync is pulled low when it hits the right of the screen, allowing for hte rays to move to the left
// of the screen on the next scanline. We can encode this information via the memory, since each distance
// in memory equates to consistent timing. So we would load the entire dma buffer with our image and 
// let the parallel i2s send out the data constantly
