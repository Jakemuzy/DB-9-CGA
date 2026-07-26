#include "display.h"


void update_buffers(BufferInfo *info)
{

}

void swap_buffers(BufferInfo *info)
{
	esp_lcd_panel_draw_bitmap(
		info->handle, 
		0, 0, 
		info->screen_width, 
		info->screen_height, 
		info->draw_buf
	);

	info->draw_buf = (info->draw_buf == (uint16_t*)info->fb1) ? (uint16_t*)info->fb2 : (uint16_t*)info->fb1;
}

void buffer_draw_borders(BufferInfo *info)
{

}

void buffer_draw_circle(BufferInfo *info, uint16_t posx, uint16_t posy, uint16_t radius, Color color, Brightness brightness)
{
    float inner_radius_sq = (radius - 3) * (radius - 3),
          outer_radius_sq = (radius + 3) * (radius + 3);

	for (int y = 0; y < info->screen_height; y++) {
		for (int x = 0; x < info->screen_width; x++) {

			float dx = (x * ASPECT_RATIO) - (posx * ASPECT_RATIO);
			float dy = y - posy;

			float dist_to_center = dx * dx + dy * dy;

			// Color the perimeter only
			Color pixel_color = DB9_BLACK;
			if (dist_to_center >= inner_radius_sq && 
				dist_to_center <= outer_radius_sq ){
				pixel_color = color | brightness;
			}

			int idx = y * info->screen_width + x;
			info->draw_buf[idx] = pixel_color;
		}
	}
}

void buffer_draw_char(BufferInfo *info, bitmap_display* character, uint16_t posx, uint16_t posy, uint16_t scale, Color color, Brightness brightness)
{
/*
	for (int y = posy; y < y + character->height; y++)
	{
	    for (int x = posx; x < x + character->width; x++)
	    {

		// ONLY display if it intersectiosn with bitmap_display
	        unsigned int pos = (y * info->screen_width) + x;
		info->fb[pos] = color;
	    }
	}
*/
}

