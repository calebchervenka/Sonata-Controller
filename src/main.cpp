#include <lvgl.h>
#include <Wire.h>
#include <FastLED.h>

#include "display.h"
#include "lighting.h"

// Enable to update lighting live from sliders instead of when released
#define FAST_LIGHTING_UPDATES 1

// Theme colors
lv_color_t C_BACKGROUND        = lv_color_hex(0x000000);
lv_color_t C_BUTTON_BACKGROUND = lv_color_hex(0x444444);
lv_color_t C_BUTTON_TEXT       = lv_color_hex(0xffffff);
lv_color_t C_SLIDER_INDIC_OFF  = lv_color_hex(0x222222);
lv_color_t C_SLIDER_INDIC_ON   = lv_color_hex(0x888888);

// Screen size information
#define LV_HOR_RES_MAX 800
#define LV_VER_RES_MAX 480

void on_save_button_clicked(lv_event_t*);
void on_toggle_button_clicked(lv_event_t*);
void on_brightness_slider_adjusted(lv_event_t*);
void on_hue_slider_adjusted(lv_event_t*);
void on_saturation_slider_adjusted(lv_event_t*);

void update_from_file();

lv_obj_t* screen;
lv_obj_t *save_button;
lv_obj_t *save_button_label;
lv_obj_t *toggle_button;
lv_obj_t *toggle_button_label;
lv_obj_t *hue_slider;
lv_obj_t *brightness_slider;
lv_obj_t *saturation_slider;

void setup() 
{
  Serial.begin(115200);
  Serial.println("Starting setup...");
  // Wait 800ms to wait for voltage to level out when system starts
  // delay(800);

  if (!config_init())
  {
    // Switch to fully local mode because the SD card cannot be mounted
  }

  update_config_from_file();

  lighting_init();

  display_init();

  // Main screen object
  screen = lv_scr_act();
  lv_obj_set_style_bg_color(screen, C_BACKGROUND, LV_PART_MAIN);

  // Toggle button
  toggle_button = lv_btn_create(screen);
  lv_obj_add_event_cb(toggle_button, on_toggle_button_clicked, LV_EVENT_CLICKED, NULL);
  lv_obj_center(toggle_button);
  lv_obj_set_style_bg_color(toggle_button, C_BUTTON_BACKGROUND, LV_PART_MAIN);
  lv_obj_set_style_shadow_color(toggle_button, lv_color_hsv_to_rgb(0, 100, 100), LV_PART_MAIN);
  lv_obj_set_style_shadow_spread(toggle_button, 10, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(toggle_button, 85, LV_PART_MAIN);
  toggle_button_label = lv_label_create(toggle_button);
  lv_label_set_text(toggle_button_label, "Toggle");
  lv_obj_set_style_bg_color(toggle_button_label, C_BUTTON_TEXT, LV_PART_MAIN);

  save_button = lv_btn_create(screen);
  lv_obj_add_event_cb(save_button, on_save_button_clicked, LV_EVENT_CLICKED, NULL);
  lv_obj_center(save_button);
  lv_obj_set_style_translate_y(save_button, -40, LV_PART_MAIN);
  lv_obj_set_style_bg_color(save_button, C_BUTTON_BACKGROUND, LV_PART_MAIN);
  save_button_label = lv_label_create(save_button);
  lv_label_set_text(save_button_label, "Save");
  lv_obj_set_style_bg_color(save_button_label, C_BUTTON_TEXT, LV_PART_MAIN);

  // Brightness slider
  brightness_slider = lv_slider_create(screen);
  lv_slider_set_range(brightness_slider, 0, 255);
  lv_slider_set_value(brightness_slider, GLOBAL_CONFIG.brightness, LV_ANIM_OFF);
  lv_obj_add_event_cb(brightness_slider, on_brightness_slider_adjusted, FAST_LIGHTING_UPDATES ? LV_EVENT_VALUE_CHANGED : LV_EVENT_RELEASED, NULL);
  lv_obj_center(brightness_slider);
  lv_obj_set_width(brightness_slider, 640);
  lv_obj_set_style_bg_color(brightness_slider, C_SLIDER_INDIC_ON, LV_PART_KNOB);
  lv_obj_set_style_bg_color(brightness_slider, C_SLIDER_INDIC_OFF, LV_PART_MAIN);
  lv_obj_set_style_bg_color(brightness_slider, C_SLIDER_INDIC_ON, LV_PART_INDICATOR);
  lv_obj_set_style_translate_y(brightness_slider, 100, LV_PART_MAIN);

  // Hue slider
  hue_slider = lv_slider_create(screen);
  lv_slider_set_range(hue_slider, 0, 255);
  lv_slider_set_value(hue_slider, GLOBAL_CONFIG.lighting_hue, LV_ANIM_OFF);
  lv_obj_add_event_cb(hue_slider, on_hue_slider_adjusted, FAST_LIGHTING_UPDATES ? LV_EVENT_VALUE_CHANGED : LV_EVENT_RELEASED, NULL);
  lv_obj_center(hue_slider);
  lv_obj_set_width(hue_slider, 640);
  lv_obj_set_style_bg_color(hue_slider, C_SLIDER_INDIC_ON, LV_PART_KNOB);
  lv_obj_set_style_bg_color(hue_slider, C_SLIDER_INDIC_OFF, LV_PART_MAIN);
  lv_obj_set_style_bg_color(hue_slider, C_SLIDER_INDIC_ON, LV_PART_INDICATOR);
  lv_obj_set_style_translate_y(hue_slider, 140, LV_PART_MAIN);

  // Saturation slider
  saturation_slider = lv_slider_create(screen);
  lv_slider_set_range(saturation_slider, 0, 255);
  lv_slider_set_value(saturation_slider, GLOBAL_CONFIG.saturation, LV_ANIM_OFF);
  lv_obj_add_event_cb(saturation_slider, on_saturation_slider_adjusted, FAST_LIGHTING_UPDATES ? LV_EVENT_VALUE_CHANGED : LV_EVENT_RELEASED, NULL);
  lv_obj_center(saturation_slider);
  lv_obj_set_width(saturation_slider, 640);
  lv_obj_set_style_bg_color(saturation_slider, C_SLIDER_INDIC_ON, LV_PART_KNOB);
  lv_obj_set_style_bg_color(saturation_slider, C_SLIDER_INDIC_OFF, LV_PART_MAIN);
  lv_obj_set_style_bg_color(saturation_slider, C_SLIDER_INDIC_ON, LV_PART_INDICATOR);
  lv_obj_set_style_translate_y(saturation_slider, 180, LV_PART_MAIN);

  // Turn the screen on to finish setup
  backlight_init();
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

void on_save_button_clicked(lv_event_t*)
{
  write_config_to_file();
}

void on_toggle_button_clicked(lv_event_t*)
{
  toggle_lighting();
}

void on_brightness_slider_adjusted(lv_event_t*)
{
  set_brightness(lv_slider_get_value(brightness_slider));
}

void on_hue_slider_adjusted(lv_event_t*)
{
  set_hue(lv_slider_get_value(hue_slider));
}

void on_saturation_slider_adjusted(lv_event_t*)
{
  set_saturation(lv_slider_get_value(saturation_slider));
}

void update_from_file()
{
  // Update local data
  update_config_from_file();

  // Update UI to match new local data
  lv_slider_set_value(brightness_slider, GLOBAL_CONFIG.brightness, LV_ANIM_OFF);
  lv_slider_set_value(hue_slider, GLOBAL_CONFIG.lighting_hue, LV_ANIM_OFF);
  lv_slider_set_value(saturation_slider, GLOBAL_CONFIG.saturation, LV_ANIM_OFF);

  // Update lighting to match new local data
  update_lighting(1);
  lv_obj_set_style_shadow_color(toggle_button, lv_color_hsv_to_rgb(GLOBAL_CONFIG.lighting_hue * 360 / 255, GLOBAL_CONFIG.saturation * 100 / 255, GLOBAL_CONFIG.brightness * GLOBAL_CONFIG.is_lighting_on * 100 / 255), LV_PART_MAIN);
}