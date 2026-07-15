#ifndef DB9_STATUS_DISPLAY_H_
#define DB9_STATUS_DISPLAY_H_

#include <stdint.h>

#include "letters.h"

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

typedef struct buffer_info {
    uint8_t *fb;
    uint16_t screen_width;
    uint16_t screen_height;
} buffer_info;


/* ----- functions ----- */

void update_buffer(uint8_t *fb);

void buffer_draw_borders(buffer_info *info);
void buffer_draw_char(buffer_info *info, bitmap_display* character , uint16_t posx, uint16_t posy, unsigned int color);

#endif
