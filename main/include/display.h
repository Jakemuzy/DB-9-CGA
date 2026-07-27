#ifndef DB9_STATUS_DISPLAY_H_
#define DB9_STATUS_DISPLAY_H_

#include <stdint.h>

#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"

#include "letters.h"
#include "colors.h"

/* 
   NOTES: 
   	Converts system status information into distinct
	 display formatting. Upon being fed status info, 
	  will automatically update the visual buffer.
	
	Will only update the section in which was changed
	   the section of buffer memory will remain
	  unchanged if the system status is unchanged.
*/

#define MARGIN_BETWEEN_ELEMENTS 5
#define BORDER_PADDING 5
#define ASPECT_RATIO 0.4f
    // This honestly looks better than the mathematically correct formula

typedef struct BufferInfo {
	esp_lcd_panel_handle_t handle;

    uint16_t *fb1, *fb2;
	uint16_t *draw_buf; 
		// This is which fb is being drawn to at any given point

    uint16_t screen_width;
    uint16_t screen_height;
} BufferInfo;


/* ----- functions ----- */

void update_buffer(BufferInfo *buf);
void swap_buffers(BufferInfo *buf);

void buffer_draw_borders(BufferInfo *info);
void buffer_draw_circle(BufferInfo *info, uint16_t posx, uint16_t posy, uint16_t radius, Color color, Brightness brightness);
void buffer_draw_char(BufferInfo *info, bitmap_display* character, uint16_t posx, uint16_t posy, uint16_t scale, Color color, Brightness brightness); // Centered on (posx, posy)

#endif
