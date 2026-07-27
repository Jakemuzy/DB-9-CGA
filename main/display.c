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
	// Position is top left, draws from top left to bottom right
	
	uint16_t char_width_pixels = scale * DEFAULT_CHAR_WIDTH_PIXELS;
	uint16_t char_height_pixels = scale * DEFAULT_CHAR_HEIGHT_PIXELS;

	// Index of the bitmap (for sampling)
	uint16_t idx_x = 0, idx_y = 0;

	for (uint16_t y = posy; y < posy + char_height_pixels; y++)
	{
	    if (y > info->screen_height)
	        break;

	    for (uint16_t x = posx; x < posx + char_width_pixels; x++)
	    {
 		if (x > info->screen_width)
		    break;	

		uint8_t mapped_x = idx_x / scale;
		uint8_t mapped_y = idx_y / scale;

		// Shift is backwards cuz old fonts were written backwards
	        bool activated = (character[mapped_y] & (1 >> mapped_x)) != 0; 
		
		if (activated)
		{
		    int idx = y * info->screen_width + x;
		    info->draw_buf[idx] = color | brightness;
		}

		idx_x++;
	    }

	    idx_x = 0;
	    idx_y++;
	}

    /*
     So the real question is since this function isn't very cachce locality friendly there must be a lot of page misses (debateable since the screen is only 640x200 which can almost be fit into esp32-s3s rom if not for the fact that each pixel uses uint16_t). But anyways the real question is since the locality isn't the best and the esp32-s3 must supply consistent vsync and hsync pulses to keep the display driven on a crt wouldn't this function slow that down a lot? Especially if many characters need to be written?
    */

}

