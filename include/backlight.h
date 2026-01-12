#include "Arduino.h"

#define BACKLIGHT_PIN 2

#define BACKLIGHT_DURATION 3000000

int64_t time_last_touch_us;

void enable_backlight()
{
  digitalWrite(BACKLIGHT_PIN, HIGH);
}

void disable_backlight()
{
  digitalWrite(BACKLIGHT_PIN, LOW);
}

void backlight_init()
{
  pinMode(BACKLIGHT_PIN, OUTPUT);
  enable_backlight();
  time_last_touch_us = esp_timer_get_time();
}

int should_sleep(int is_touched)
{
  if (is_touched)
  {
    time_last_touch_us = esp_timer_get_time();
    return 0;
  }
  else
    // return 1;
    return (esp_timer_get_time() - time_last_touch_us) > BACKLIGHT_DURATION;
}

void sleep_backlight_if_inactive(int is_touched)
{
  if (should_sleep(is_touched))
    disable_backlight();
  else
    enable_backlight();
}