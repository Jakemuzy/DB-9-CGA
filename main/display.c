#include "display.h"

static const char* TAG = "DB9 (display)";
static BufferInfo* buffer_info = NULL;
static SemaphoreHandle_t display_mutex = NULL;

BufferInfo* initialize_buffer_info() 
{
    // Creates actual object
    ESP_LOGI(TAG, "Initializing Buffer Object...");
    BufferInfo* info = calloc(1, sizeof(BufferInfo));
    info->screen_height = config_get_screen_height();
    info->screen_width = config_get_screen_width();

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
    info->draw_buf = info->fb1;

    ESP_LOGI(TAG, "SUCCESS: Created buffer object.");
    return info;
}

void update_buffer(uint16_t* blob)
{
    uint16_t* back_buffer = (buffer_info->fb1 == buffer_info->draw_buf) ? buffer_info->fb2 : buffer_info->fb1;

    memcpy(
        back_buffer, 
        blob, 
        buffer_info->screen_width * buffer_info->screen_height * sizeof(uint16_t)
    );
}

void swap_buffers()
{
    esp_lcd_panel_draw_bitmap(
        buffer_info->handle, 
        0, 0, 
        buffer_info->screen_width, 
        buffer_info->screen_height, 
        buffer_info->draw_buf
    );

    buffer_info->draw_buf = (buffer_info->draw_buf == (uint16_t*)buffer_info->fb1) ? 
                            (uint16_t*)buffer_info->fb2 : 
                            (uint16_t*)buffer_info->fb1;

    // If front and back buffer differ, copy the front into the back upon switch
    if (buffer_info->fb1 != buffer_info->fb2)
        buffer_info->fb2 = buffer_info->fb1;
}





void display_init(void)
{
    display_mutex = xSemaphoreCreateMutex();
}

void display_task(void* pvParamaters)
{
    buffer_info = initialize_buffer_info();

    while (1) {
        xSemaphoreTake(display_mutex, PORT_MAX_DELAY_TICKS);
        swap_buffers();
        xSemaphoreGive(display_mutex);
        vTaskDelay(pdMS_TO_TICKS(DISPLAY_DELAY_MS));
    }
}

void display_receive_blob(void* blob, int len)
{
    if (display_mutex == NULL || buffer_info == NULL) {
        ESP_LOGW(TAG, "Blob arrived before display ready, dropping.");
        return;
    }
    size_t expected = (size_t)buffer_info->screen_width
                     * (size_t)buffer_info->screen_height
                     * sizeof(uint16_t);
 
    if ((size_t)len != expected) {
        ESP_LOGW(TAG, "Blob size mismatch: got %d bytes, expected %zu", len, expected);
        return;
    }

    xSemaphoreTake(display_mutex, PORT_MAX_DELAY_TICKS);
    update_buffer((uint16_t*)blob);
    xSemaphoreGive(display_mutex);
}