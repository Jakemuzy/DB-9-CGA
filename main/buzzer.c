#include "buzzer.h"

const Notification NOTIFICATION_LEVELS[NOTIFY_COUNT] = {
    [NOTIFY_INFO]     = { .frequency = 2500, .duration_ms = 80 },
    [NOTIFY_ALERT]    = { .frequency = 1200, .duration_ms = 150 },
    [NOTIFY_WARNING]  = { .frequency = 800,  .duration_ms = 300 },
    [NOTIFY_CRITICAL] = { .frequency = 3500, .duration_ms = 100 }
};
