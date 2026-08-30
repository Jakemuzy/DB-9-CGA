#ifndef DB9_DISPLAY_H_
#define DB9_DISPLAY_H_

/*
       Display Wrapper
	-------------
     Object that handles 
      ESP32 lcd display.

*/

#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"

#include "config.h"

#define PORT_MAX_DELAY_TICKS 100
#define DISPLAY_DELAY_MS 300
				

/* ----- Struct ----- */

typedef struct BufferInfo { // TODO: Shared resource, should handle
    esp_lcd_panel_handle_t handle;

    uint16_t *fb1, *fb2;
    uint16_t *draw_buf;  	
			 
    uint16_t screen_width;
    uint16_t screen_height;
} BufferInfo;


/* ----- Functions & Creation ----- */


BufferInfo* initialize_buffer_info(void);

void update_buffer(uint16_t* blob);
void swap_buffers(void);

// Starts rendering loop
void display_init(void);
void display_task(void* pvParamaters);
void display_receive_blob(void* blob, int len);

#endif
