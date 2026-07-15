#include "display.h"

void update_buffer(uint8_t *fb);



void buffer_draw_borders(buffer_info *info);

void buffer_draw_char(buffer_info *info, bitmap_display* character, uint16_t posx, uint16_t posy, unsigned int color)
{
	for (int y = posy; y < y + character->height; y++)
	{
	    for (int x = posx; x < x + character->width; x++)
	    {

		// ONLY display if it intersectiosn with bitmap_display
	        unsigned int pos = (y * info->screen_width) + x;
		info->fb[pos] = color;
	    }
	}
}

