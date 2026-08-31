#ifndef DB9_BUZZER_H_
#define DB9_BUZZER_H_

/*
	Buzzer Sounds
	-------------
     Maps frequencies and 
    durations to different
     notification levels
*/

#include "config.h"

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


void start_buzzer(NotificationLevel notification_level);
void stop_buzzer();
 
#endif
