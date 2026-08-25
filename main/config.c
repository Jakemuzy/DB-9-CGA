#include "config.h"

static unsigned int DESIRED_CLOCK_RATE = 14318180;
static unsigned int VSYNC_RATE         = 60;
static unsigned int HSYNC_RATE         = 15750;
static unsigned int SCREEN_WIDTH       = 640;
static unsigned int SCREEN_HEIGHT      = 200;

unsigned int config_get_clock_rate(void)   { return DESIRED_CLOCK_RATE; }
unsigned int config_get_vsync(void)        { return VSYNC_RATE; }
unsigned int config_get_hsync(void)        { return HSYNC_RATE; }
unsigned int config_get_screen_width(void)  { return SCREEN_WIDTH; }
unsigned int config_get_screen_height(void) { return SCREEN_HEIGHT; }



void  config_set_clock_rate(unsigned int val)   { DESIRED_CLOCK_RATE = val; }
void  config_set_vsync(unsigned int val)        { VSYNC_RATE = val; }
void  config_set_hsync(unsigned int val)        { HSYNC_RATE = val; }
void  config_set_screen_width(unsigned int val) { SCREEN_WIDTH = val; }
void  config_set_screen_height(unsigned int val){ SCREEN_HEIGHT = val; }
