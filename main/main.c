#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Measured in Hz
#define VSYNC_RATE 60
#define HSYNC_RATE 15750

#define GROUND_ONE 1
#define GROUND_TWO 2
#define RED 3
#define GREEN 4
#define BLUE 5
#define INTENSITY 6
// Pin 7 not in use for CGA 
#define H_SYNC 8
#define V_SYNC 9

/* 
   FOREWORD:
	The Samsung Santron SC-452C was a 1989 CRT monitor.
	   It features 16-bit color depth, and sports 
	    a legacy DB-9 CGA video input connector.
	 For purposes of this project, we aim to recreate
	     this video output via modern hardware.

   NOTES:
	- 
*/


#ifdef ESP32
    #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
    #include "esp_log.h"
    #include "esp_sleep.h"
    #include "soc/clk_tree_defs.h"

    static const char* TAG = "ESP32";
#else
#endif

const uint32_t CLOCK_RATE = 0;

void app_main()
{
    #ifdef ESP32
        //SOC_LCD_CLKS
        //CLOCK_RATE = // LCD_BUS
	
	// Probably I2s, or lcd bus should be used
	soc_periph_lcd_clk_src_t lcd_timers[] = (soc_periph_lcd_clk_src_t)SOC_LCD_CLKS;

	for (size_t i = 0; i < (sizeof(lcd_timers) / sizeof(lcd_timers[0])); i++) {
	    soc_periph_lcd_clk_src_t clk = lcd_timers[i];
	    esp_log();
	}

	esp_err_t err = esp_clk_tree_src_get_freq_hz(
	    clk, 
	    ESP_CLK_TREE_SRC_FREQ_PRECISIN_EXACT,
	    &CLOCK_RATE
	);

	if (err != ESP_OK) {
	    ESP_LOGW(TAG, "Clock rate could not be determined for the lcd timer");
	}
	else {
	    ESP_LOGV(TAG, "Clock rate is %lu\n", CLOCK_RATE);
	    esp_deep_sleep_start();
	}

	// LCD_CLK_SRC_PLL160M ???
    #endif

}
