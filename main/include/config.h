#ifndef DB9_CONFIG_H_
#define DB9_CONFIG_H_

/*
       General Settings
	-------------
     Allows user to change
    settings for ease over 
       MQTT connection. 
   Primarily so we can tweak
     without recompiling

*/

// DB9 pins
#define RED_PIN          GPIO_NUM_42
#define GREEN_PIN        GPIO_NUM_41
#define BLUE_PIN         GPIO_NUM_40
#define INTENSITY_PIN    GPIO_NUM_39
#define H_SYNC_PIN       GPIO_NUM_14
#define V_SYNC_PIN       GPIO_NUM_13

// Unused pins, required to be specified
#define PCLK_PIN     GPIO_NUM_17  
#define DUMMY_A      GPIO_NUM_9
#define DUMMY_B      GPIO_NUM_10
#define DUMMY_C      GPIO_NUM_11
#define DUMMY_D      GPIO_NUM_12

// Buzzer and power pins
#define POWER_PIN   GPIO_NUM_2
#define BUZZER_PIN  GPIO_NUM_20

/* ----- Setters & Getters ----- */

unsigned int config_get_clock_rate(void);
unsigned int config_get_vsync(void);
unsigned int config_get_hsync(void);
unsigned int config_get_screen_width(void);
unsigned int config_get_screen_height(void);

void  config_set_clock_rate(unsigned int val);
void  config_set_vsync(unsigned int val);
void  config_set_hsync(unsigned int val);
void  config_set_screen_width(unsigned int val);
void  config_set_screen_height(unsigned int val);

#endif
