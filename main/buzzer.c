#include "buzzer.h"

static const char* TAG = "DB9 (buzzer)";

const Notification NOTIFICATION_LEVELS[NOTIFY_COUNT] = {
    [NOTIFY_INFO]     = { .frequency = 2500, .duration_ms = 150 },
    [NOTIFY_ALERT]    = { .frequency = 1200, .duration_ms = 400 },
    [NOTIFY_WARNING]  = { .frequency = 800,  .duration_ms = 800 },
    [NOTIFY_CRITICAL] = { .frequency = 400, .duration_ms = 1500 }
};

void initialize_buzzer()
{
    ESP_LOGI(TAG, "Initializing buzzer...");
    ledc_timer_config_t buzzer_timer = {
        .speed_mode      = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num       = BUZZER_TIMER,
        .freq_hz         = 400,     // Start at 4 kHz
        .clk_cfg         = LEDC_USE_RC_FAST_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&buzzer_timer));

    ledc_channel_config_t buzzer_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = BUZZER_CHANNEL, 
        .timer_sel      = BUZZER_TIMER,
        .gpio_num       = BUZZER_PIN,
        .duty           = 0,    // Start silent
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&buzzer_channel));
    ESP_LOGI(TAG, "SUCCESS: Created buzzer channel");
}

void play_tone_buzzer_async(NotificationLevel notification_level)
{
    if (notification_level == NOTIFY_CRITICAL) ESP_LOGI(TAG, "Notification level is critical");
    else ESP_LOGI(TAG, "Notifiication level isn't critical");

    // Blocking for now, but will make non blocking
    ESP_LOGI(TAG, "Starting buzzer...");
    start_buzzer(notification_level);
    ESP_LOGI(TAG, "Waiting...");
    vTaskDelay(pdMS_TO_TICKS(NOTIFICATION_LEVELS[notification_level].duration_ms));
    ESP_LOGI(TAG, "Stopping buzzer...");
    stop_buzzer();
}

void start_buzzer(NotificationLevel notification_level)
{
    // This will be run on a separate thread
    Notification notif = NOTIFICATION_LEVELS[notification_level];
    ledc_set_freq(LEDC_LOW_SPEED_MODE, BUZZER_TIMER, notif.frequency);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, BUZZER_CHANNEL, 512);    // 50% volume since using 10 bit 
    ledc_update_duty(LEDC_LOW_SPEED_MODE, BUZZER_CHANNEL);
}

void stop_buzzer()
{
    ledc_set_duty(LEDC_LOW_SPEED_MODE, BUZZER_CHANNEL, 0);  // Silent
    ledc_update_duty(LEDC_LOW_SPEED_MODE, BUZZER_CHANNEL);
}