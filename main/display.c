#include "display.h"

static const char* TAG = "DB9 (display)";

BufferInfo* initialize_bufferinfo(esp_lcd_panel_handle_t handle, unsigned int screen_width, unsigned int screen_height) 
{
    // Initialize
    
    ESP_ERROR_CHECK(
	esp_lcd_rgb_panel_get_frame_buffer(
	    panel_handle, 
	    2, 
	    &fb1, 
	    &fb2
	)
    );

}

void update_buffer(BufferInfo *buf, uint16_t* blob)
{
    uint16_t* back_buffer = (buf->fb1 == buf->draw_buf) ? buf->fb2 : buf->fb1;

    memcpy(back_buffer, blob, sizeof(uint16_t));
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

void draw_buffers(BufferInfo *buf);
{
    // I dont think necessary since esp just reads this data
}

