#include <lvgl.h>
#include <Wire.h>
#include <FastLED.h>

#include "widgets.h"
#include "display.h"
#include "wireless.h"

// Screen size information
#define LV_HOR_RES_MAX 800
#define LV_VER_RES_MAX 480

void screen_init();
void startup();

lv_obj_t* screen;

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting setup...");

  if (!config_init())
    update_config_from_file();

  lighting_init();

  wireless_init();

  screen_init();

  startup();

  Serial.println("Setup complete. Starting loop.");
}

char *preview_text = (char*) malloc(sizeof(char) * 128);
void loop() 
{
  // Update led strip lighting and display preview to match
  if (update_lighting()) lv_obj_set_style_shadow_color(toggle_button, lv_color_hsv_to_rgb(GLOBAL_CONFIG.lighting_hue * 360 / 255, GLOBAL_CONFIG.saturation * 100 / 255, GLOBAL_CONFIG.brightness * GLOBAL_CONFIG.is_lighting_on * 100 / 255), LV_PART_MAIN);

  lv_timer_handler();

  ts.read();
  sleep_backlight_if_inactive(ts.isTouched);

  delay(5);
}

// -------------------
//      Initialisers
// -------------------
void screen_init()
{
  display_init();

  // Main screen object
  screen = lv_scr_act();
  lv_obj_set_style_bg_color(screen, C_BACKGROUND, LV_PART_MAIN);

  widgets_init(screen);

  backlight_init();
}

void startup()
{
  // Set screen image

  // Lighting animation
  for (int i = 0; i < 255; i++)
  {
    set_brightness(i);
    delay(2);
    update_lighting();
  }

  // Clear screen image
}