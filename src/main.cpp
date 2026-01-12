#include <lvgl.h>
#include <Wire.h>
#include <FastLED.h>

#include "display.h"
#include "lighting.h"

// Theme colors
lv_color_t C_BACKGROUND        = lv_color_hex(0x000000);
lv_color_t C_BUTTON_BACKGROUND = lv_color_hex(0x444444);
lv_color_t C_BUTTON_TEXT       = lv_color_hex(0xffffff);
lv_color_t C_SLIDER_INDIC_OFF  = lv_color_hex(0x222222);
lv_color_t C_SLIDER_INDIC_ON   = lv_color_hex(0x888888);

// Screen size information
#define LV_HOR_RES_MAX 800
#define LV_VER_RES_MAX 480

void on_write_to_config_button_clicked(lv_event_t*);
void on_read_config_button_clicked(lv_event_t*);
void on_toggle_button_clicked(lv_event_t*);
void on_brightness_slider_adjusted(lv_event_t*);
void on_hue_slider_adjusted(lv_event_t*);

void write_config_task(void*);
void update_from_file();

lv_obj_t* screen;
lv_obj_t *write_to_config_button;
lv_obj_t *write_to_config_button_label;
lv_obj_t *read_config_button;
lv_obj_t *read_config_button_label;
lv_obj_t *toggle_button;
lv_obj_t *toggle_button_label;
lv_obj_t *toggle_button_shadow;
lv_obj_t *hue_slider;
lv_obj_t *brightness_slider;

lv_obj_t *global_config_preview;

void setup() 
{
  // Wait 800ms to wait for voltage to level out when system starts
  // delay(800);

  // config_init();

  lighting_init();

  display_init();

  // Delay was present by default, may not be necessary
  delay(200);
  backlight_init();

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

  // // Lighting shadow behind toggle button
  // toggle_button_shadow = lv_obj_create(screen);
  // lv_obj_center(toggle_button);
  // lv_obj_set_style_bg_grad(toggle_button_shadow, lv_grad_)

  // Brightness slider
  brightness_slider = lv_slider_create(screen);
  lv_slider_set_range(brightness_slider, 0, 255);
  lv_slider_set_value(brightness_slider, GLOBAL_CONFIG.lighting_hue, LV_ANIM_OFF);
  lv_obj_add_event_cb(brightness_slider, on_brightness_slider_adjusted, LV_EVENT_RELEASED, NULL);
  lv_obj_center(brightness_slider);
  lv_obj_set_style_bg_color(brightness_slider, C_SLIDER_INDIC_ON, LV_PART_KNOB);
  lv_obj_set_style_bg_color(brightness_slider, C_SLIDER_INDIC_OFF, LV_PART_MAIN);
  lv_obj_set_style_bg_color(brightness_slider, C_SLIDER_INDIC_ON, LV_PART_INDICATOR);
  lv_obj_set_style_translate_y(brightness_slider, 100, LV_PART_MAIN);

  // Hue slider
  hue_slider = lv_slider_create(screen);
  lv_slider_set_range(hue_slider, 0, 255);
  lv_slider_set_value(hue_slider, GLOBAL_CONFIG.lighting_hue, LV_ANIM_OFF);
  lv_obj_add_event_cb(hue_slider, on_hue_slider_adjusted, LV_EVENT_RELEASED, NULL);
  lv_obj_center(hue_slider);
  lv_obj_set_style_bg_color(hue_slider, C_SLIDER_INDIC_ON, LV_PART_KNOB);
  lv_obj_set_style_bg_color(hue_slider, C_SLIDER_INDIC_OFF, LV_PART_MAIN);
  lv_obj_set_style_bg_color(hue_slider, C_SLIDER_INDIC_ON, LV_PART_INDICATOR);
  lv_obj_set_style_translate_y(hue_slider, 140, LV_PART_MAIN);


  // Final initialisation
  update_from_file();
}

char *preview_text = (char*) malloc(sizeof(char) * 128);
void loop() 
{
  update_lighting();
  lv_obj_set_style_shadow_color(toggle_button, lv_color_hsv_to_rgb(GLOBAL_CONFIG.lighting_hue * 360 / 255, 100, GLOBAL_CONFIG.brightness * GLOBAL_CONFIG.is_lighting_on * 100 / 255), LV_PART_MAIN);

  lv_timer_handler();

  ts.read();
  sleep_backlight_if_inactive(ts.isTouched);

  delay(5);
}

void on_write_to_config_button_clicked(lv_event_t*)
{
  // xTaskCreate(write_config_task, "write_config", 4096, NULL, 1, NULL);
}

void on_read_config_button_clicked(lv_event_t*)
{
  // update_from_file();
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

void write_config_task(void*)
{
  lv_obj_add_state(write_to_config_button, LV_STATE_DISABLED);
  // write_config();
  lv_obj_clear_state(write_to_config_button, LV_STATE_DISABLED);
  // vTaskDelete(NULL);
}

void update_from_file()
{
  // Update local data
  update_config_from_file();

  // Update UI to match new local data
  lv_slider_set_value(brightness_slider, GLOBAL_CONFIG.brightness, LV_ANIM_OFF);
  lv_slider_set_value(hue_slider, GLOBAL_CONFIG.lighting_hue, LV_ANIM_OFF);

  // Update lighting to match new local data
  update_lighting(1);
}