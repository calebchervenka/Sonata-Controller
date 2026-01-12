#include "Arduino.h"

#define BACKLIGHT_PIN 2

#define BACKLIGHT_DURATION_MS 3000000

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
  delay(200);
  enable_backlight();
  time_last_touch_us = esp_timer_get_time();
}

int should_sleep()
{
  return (esp_timer_get_time() - time_last_touch_us) > BACKLIGHT_DURATION_MS;
}

void sleep_backlight_if_inactive(int is_touched)
{
  if (is_touched)
    time_last_touch_us = esp_timer_get_time();

  if (should_sleep())
    disable_backlight();
  else
    enable_backlight();
}