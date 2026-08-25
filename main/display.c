#include "display.h"

static const char* TAG = "DB9 (display)";

BufferInfo* initialize_buffer_info() 
{
    // Creates actual object
    ESP_LOGI(TAG, "Initializing Buffer Object...");
    BufferInfo* info = calloc(1, sizeof(BufferInfo));
    info->screen_height = config_get_screen_height();
    info->screen_width = config_get_screen_width();
    info->draw_buf = info->fb1;


    // Define a single configuration for the entire RGB panel
    ESP_LOGI(TAG, "Initializing LCD RGB Panel Object...");
    esp_lcd_rgb_panel_config_t panel_cfg = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .pclk_gpio_num = PCLK_PIN, 
        .vsync_gpio_num = V_SYNC_PIN,         
        .hsync_gpio_num = H_SYNC_PIN,         
        .de_gpio_num = -1,                // CGA doesn't use Data Enable 
        .bounce_buffer_size_px = 10 * config_get_screen_width(),
        .data_width = 16,                 
        .in_color_format = LCD_COLOR_FMT_RGB565,
        .data_gpio_nums = {
            RED_PIN,        
            GREEN_PIN,      
            BLUE_PIN,       
            INTENSITY_PIN,

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
            .pclk_hz = config_get_clock_rate(),
            .h_res = config_get_screen_width(),     
            .v_res = config_get_screen_height(),    
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

    ESP_ERROR_CHECK(
	esp_lcd_new_rgb_panel(
	    &panel_cfg, 
	    &info->handle
	)
    );

    ESP_ERROR_CHECK(esp_lcd_panel_reset(info->handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(info->handle));
    
    ESP_ERROR_CHECK(
	esp_lcd_rgb_panel_get_frame_buffer(
	    info->handle, 
	    2, 
	    (void**)&info->fb1, 
	    (void**)&info->fb2
	)
    );

    ESP_LOGI(TAG, "SUCCESS: Created buffer object.");
    return info;
}

void update_buffer(BufferInfo *buf, uint16_t* blob)
{
    uint16_t* back_buffer = (buf->fb1 == buf->draw_buf) ? buf->fb2 : buf->fb1;

    memcpy(
	back_buffer, 
	blob, 
	buf->screen_width * buf->screen_height * sizeof(uint16_t)
    );
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

