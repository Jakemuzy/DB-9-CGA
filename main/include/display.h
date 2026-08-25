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

#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"

#include "config.h"

#define MARGIN_BETWEEN_ELEMENTS 5
#define BORDER_PADDING 5
#define ASPECT_RATIO 0.4f	// This honestly looks better than the mathematically correct formula
				

/* ----- Struct ----- */

typedef struct BufferInfo {
    esp_lcd_panel_handle_t handle;

    uint16_t *fb1, *fb2;
    uint16_t *draw_buf;  	
			 
    uint16_t screen_width;
    uint16_t screen_height;
} BufferInfo;


/* ----- Functions & Creation ----- */

BufferInfo* initialize_buffer_info();

void update_buffer(BufferInfo *buf, uint16_t* blob);
void swap_buffers(BufferInfo *buf);

#endif
