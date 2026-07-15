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

#define RED          GPIO_NUM_4
#define GREEN        GPIO_NUM_5
#define BLUE         GPIO_NUM_6
#define INTENSITY    GPIO_NUM_7
#define H_SYNC       GPIO_NUM_15
#define V_SYNC       GPIO_NUM_16

// Unused pins, required to be specified
#define PCLK_PIN     GPIO_NUM_17  
#define DUMMY_A      GPIO_NUM_9
#define DUMMY_B      GPIO_NUM_10
#define DUMMY_C      GPIO_NUM_11
#define DUMMY_D      GPIO_NUM_12

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

uint8_t custom_canvas[SCREEN_WIDTH * SCREEN_HEIGHT];
uint8_t *cga_frame_buffer = NULL;
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

        .data_width = 8,                 
	.in_color_format = LCD_COLOR_FMT_RGB888,
        .data_gpio_nums = {
            RED,        
            GREEN,      
            BLUE,       
            INTENSITY,
	    // Unused dummies since requires 8 bit width
	    DUMMY_A, 
	    DUMMY_B, 
	    DUMMY_C,
	    DUMMY_D
        },

	.user_fbs = {
	    custom_canvas
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
	    .disp_active_low = 1,
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

    ESP_LOGI(TAG, "Drawing circle...");
    void *fb_pointer = NULL;
    ESP_ERROR_CHECK(esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 1, &fb_pointer));

    cga_frame_buffer = custom_canvas;
    memset(cga_frame_buffer, 0x00, SCREEN_WIDTH * SCREEN_HEIGHT);

    buffer_info *info = malloc(sizeof(buffer_info));
    info->fb = cga_frame_buffer;
    info->screen_width = SCREEN_WIDTH;
    info->screen_height = SCREEN_HEIGHT;

    bitmap_display *letter_A = malloc(sizeof(bitmap_display));
    letter_A->width = 20;
    letter_A->height = 20; 
    letter_A->bitmap = BITMAP_A;

    buffer_draw_char(info, letter_A, 30, 30, 0xF0);

    // Temporary for dumping circle data
    FILE *fptr = fopen("output.txt", "wb");
    char size_header[10];
    
    snprintf(size_header, 10, "%d, %d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    fprintf(fptr, "%s", size_header);
    fwrite(cga_frame_buffer, 
	sizeof(custom_canvas[0]), 
	sizeof(custom_canvas) / sizeof(custom_canvas[0]), 
	fptr
    );
    fclose(fptr);

    /*
    uint16_t radius = SCREEN_WIDTH / 4;
    uint16_t center_x = SCREEN_WIDTH / 2,
	     center_y = SCREEN_HEIGHT / 2;

    float inner_radius_sq = (radius - 3) * (radius - 3),
          outer_radius_sq = (radius + 3) * (radius + 3);

    while (1) {

        int64_t ms = esp_timer_get_time() / 1000;

        for (int y = 0; y < SCREEN_HEIGHT; y++) {
	    for (int x = 0; x < SCREEN_WIDTH; x++) {

	        float dist_to_center = 
		        (x - center_x) * (x - center_x) +
		        (y - center_y) * (y - center_y);

	        float angle = (float)ms * 0.003f;
	        float normalized_sin = (sin(angle) + 1.0f) / 2.0f;
	        unsigned int color_multiplier = (unsigned int)(normalized_sin * 15.0f);

	        // Color perimeter only
	        unsigned int color = 0x00;
	        if (dist_to_center >= inner_radius_sq && 
		    dist_to_center <= outer_radius_sq ){
		    color = 0x0F & color_multiplier;
	        }

	        int idx = y * SCREEN_WIDTH + x;
	        cga_frame_buffer[idx] = color;
    	    }
        }
        ESP_LOGI(TAG, "Finished drawing circle.");


	#if DEBUG

		// Temporary for dumping circle data
		FILE *fptr = fopen("output.txt", "wb");
		char size_header[10];

		snprintf(size_header, 10, "%d, %d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
		fprintf(fptr, "%s", size_header);
		fwrite(cga_frame_buffer, 
			sizeof(custom_canvas[0]), 
			sizeof(custom_canvas) / sizeof(custom_canvas[0]), 
			fptr
		);
		fclose(fptr);

	#endif

	vTaskDelay(pdMS_TO_TICKS(3000));
    }
    */
}



// REMEMBER: hsync and vsync are typically pulled high when writing. When pulled low it tells
// the hardware to switch scanlines. Vsync is pulled low when it hits the bottom right of the screen
// indicating that a new frame is about to be drawn and to move the ray to the top left of the screen.
// Hsync is pulled low when it hits the right of the screen, allowing for hte rays to move to the left
// of the screen on the next scanline. We can encode this information via the memory, since each distance
// in memory equates to consistent timing. So we would load the entire dma buffer with our image and 
// let the parallel i2s send out the data constantly
