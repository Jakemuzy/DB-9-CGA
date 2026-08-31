#ifndef DB9_BUZZER_H_
#define DB9_BUZZER_H_

/*
	Buzzer Sounds
	-------------
     Maps frequencies and 
    durations to different
     notification levels
*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/ledc.h"

#include "config.h"

#define BUZZER_CHANNEL LEDC_CHANNEL_0
#define BUZZER_TIMER   LEDC_TIMER_0

/* ----- Structs & Enums ----- */

typedef struct Notification {
     unsigned int frequency;
     unsigned int duration_ms;
} Notification;

typedef enum {
    NOTIFY_INFO = 0,
    NOTIFY_ALERT,
    NOTIFY_WARNING,
    NOTIFY_CRITICAL,
    NOTIFY_COUNT
} NotificationLevel;

extern const Notification NOTIFICATION_LEVELS[NOTIFY_COUNT];


/* ----- Functions ----- */

void initialize_buzzer();

void play_tone_buzzer_async(NotificationLevel notification_level);
void start_buzzer(NotificationLevel notification_level);
void stop_buzzer();
 
#endif
