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

// Measured in Hz
// Center of acceptable the range for desired
#define DESIRED_CLOCK_RATE 14318180
#define VSYNC_RATE 60
#define HSYNC_RATE 15750

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 200

#define RED          GPIO_NUM_42
#define GREEN        GPIO_NUM_41
#define BLUE         GPIO_NUM_40
#define INTENSITY    GPIO_NUM_39
#define H_SYNC       GPIO_NUM_14
#define V_SYNC       GPIO_NUM_13

// Unused pins, required to be specified
#define PCLK_PIN     GPIO_NUM_17  
#define DUMMY_A      GPIO_NUM_9
#define DUMMY_B      GPIO_NUM_10
#define DUMMY_C      GPIO_NUM_11
#define DUMMY_D      GPIO_NUM_12
#define DUMMY_E      GPIO_NUM_5
#define DUMMY_F      GPIO_NUM_2
#define DUMMY_G      GPIO_NUM_15
#define DUMMY_H      GPIO_NUM_16
#define DUMMY_I      GPIO_NUM_6
#define DUMMY_J      GPIO_NUM_21
#define DUMMY_K      GPIO_NUM_3
#define DUMMY_L      GPIO_NUM_4

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

static const char* TAG = "main";

void *fb1 = NULL, *fb2 = NULL;
esp_lcd_panel_handle_t panel_handle = NULL;

void Db9Clock()
{
    // Define a single configuration for the entire RGB panel
    esp_lcd_rgb_panel_config_t panel_cfg = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .pclk_gpio_num = PCLK_PIN, 
        .vsync_gpio_num = V_SYNC,         
        .hsync_gpio_num = H_SYNC,         
        .de_gpio_num = -1,                // CGA doesn't use Data Enable 
        .bounce_buffer_size_px = 10 * SCREEN_WIDTH,

        .data_width = 16,                 
        .in_color_format = LCD_COLOR_FMT_RGB565,
        .data_gpio_nums = {
            RED,        
            GREEN,      
            BLUE,       
            INTENSITY,
	    // Unused dummies since requires 16 bit width
            DUMMY_A, 
            DUMMY_B, 
            DUMMY_C,
            DUMMY_D,
            DUMMY_E,
            DUMMY_F,
            DUMMY_G,
            DUMMY_H,
            DUMMY_I,
            DUMMY_J,
            DUMMY_K,
            DUMMY_L
        },

        // Porch values straight from IBM hardware documentaiton for CGA, what a weird standard
        .timings = {
            .pclk_hz = DESIRED_CLOCK_RATE,
            .h_res = SCREEN_WIDTH,     
            .v_res = SCREEN_HEIGHT,    
            .hsync_front_porch = 16,
            .hsync_pulse_width = 42,
            .hsync_back_porch = 160,  

            .vsync_front_porch = 27,
            .vsync_pulse_width = 3,
            .vsync_back_porch = 32,    
            .flags = {
                .hsync_idle_low = 1,   // CGA active high
                .vsync_idle_low = 1,
            },
        },
        .flags = {
            .fb_in_psram = true,      
	        .disp_active_low = 1,
            .double_fb = 1
        },
    };

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_cfg, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
}

void app_main()
{
    ESP_LOGI(TAG, "Initializing RGB panel.");
    Db9Clock();
    ESP_LOGI(TAG, "Finished initializing RGB panel.");

    ESP_LOGI(TAG, "Clearing Screen...");
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &fb1, &fb2));


    ESP_LOGI(TAG, "Drawing...");
    BufferInfo *info = malloc(sizeof(BufferInfo));
    info->screen_width = SCREEN_WIDTH;
    info->screen_height = SCREEN_HEIGHT;
    info->handle = panel_handle;
    info->fb1 = fb1;
    info->fb2 = fb2;
    info->draw_buf = fb1;


    /*
    bitmap_display *letter_A = malloc(sizeof(bitmap_display));
    letter_A->width = 20;
    letter_A->height = 20; 
    letter_A->bitmap = BITMAP_A;

    buffer_draw_char(info, letter_A, 30, 30, 0xF0);
    */
    Color color = DB9_RED;
    while (1) {
        buffer_draw_circle(
            info,
            (uint16_t)(SCREEN_WIDTH / 2),
            (uint16_t)(SCREEN_HEIGHT /2),
            (uint16_t)(SCREEN_WIDTH / 8),
	    3, 
            color,
            DB9_DARK
        );

        buffer_draw_char(
            info,
            (bitmap_display)BITMAP_Y,
            20,
            40,
            1,
            DB9_BLUE,
            DB9_BRIGHT
        );

        buffer_draw_char(
            info,
            (bitmap_display)BITMAP_O,
            36,
            40,
            1,
            DB9_BLUE,
            DB9_BRIGHT
        );

        buffer_draw_char(
            info,
            (bitmap_display)BITMAP_U,
            52,
            40,
            1,
            DB9_BLUE,
            DB9_BRIGHT
        );

        swap_buffers(info);

	vTaskDelay(pdMS_TO_TICKS(300));
    }

}



// REMEMBER: hsync and vsync are typically pulled high when writing. When pulled low it tells
// the hardware to switch scanlines. Vsync is pulled low when it hits the bottom right of the screen
// indicating that a new frame is about to be drawn and to move the ray to the top left of the screen.
// Hsync is pulled low when it hits the right of the screen, allowing for hte rays to move to the left
// of the screen on the next scanline. We can encode this information via the memory, since each distance
// in memory equates to consistent timing. So we would load the entire dma buffer with our image and 
// let the parallel i2s send out the data constantly
