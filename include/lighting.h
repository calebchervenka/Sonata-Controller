#pragma once

#include <FastLED.h>
#include <stdbool.h>

#include "config.h"

// ----------------------------------------
// HARDWARE DEFINITIONS
// ----------------------------------------
#define LED_PIN_1   38
#define LED_COUNT_1 144

#define FAST_LIGHTING_UPDATES 1

// ----------------------------------------
// UPDATE FLAGS
// ----------------------------------------
#define UPDATE_FLAG_SATURATION  0x1
#define UPDATE_FLAG_BRIGHTNESS  0x2
#define UPDATE_FLAG_HUE         0x4
#define UPDATE_FLAG_ANIMATION   0x8

// ----------------------------------------
// ANIMATIONS AND PARAMETERS
// ----------------------------------------
#define ANIMATION_PARAMETER_COUNT 4

#define ANIMATION_PULSE_LINEAR  1
#define AP_PL_FALL animation_params[0]
#define AP_PL_RISE animation_params[1]

#define ANIMATION_PULSE_EXPONENTIAL 2
#define AP_PE_FALL_COEF animation_params[0]
#define AP_PE_FALL_EXP animation_params[1]
#define AP_PE_RISE_COEF animation_params[2]
#define AP_PE_RISE_EXP animation_params[3]

#define ANIMATION_RAINBOW 3
#define AP_RAINBOW_SPEED animation_params[0]

// Lighting updates and timestamps
int64_t init_time;
bool first_update_called;
unsigned int update_flags;

// Lighting hardware references
CRGB strip1[LED_COUNT_1];

// Variables for animation configuration
int animation_params[ANIMATION_PARAMETER_COUNT];
int64_t animation_start_time;

// ----------------------------------------
// FUNCTION DEFINITIONS
// ----------------------------------------
void lighting_init();
int update_lighting(int);
void update_animation();

void set_brightness(int level);
void set_hue(int hue);
void set_saturation(int saturation);
void set_animation(int animation_index, const int* params, int params_size);

void set_all_flags();


void lighting_init()
{
    FastLED.addLeds<WS2812, LED_PIN_1, GRB>(strip1, LED_COUNT_1);

    // Set all update flags
    set_all_flags();

    // Set LEDs to off
    FastLED.setBrightness(0);
}

/**
 * Returns true if any lighting data was changed
 */
int update_lighting(int force_all = 0)
{
    if (force_all) set_all_flags();

    update_animation();

    if (update_flags)
    {
        first_update_called = 1;
        if (update_flags & (UPDATE_FLAG_HUE | UPDATE_FLAG_SATURATION))
        {
            for (int i = 0; i < LED_COUNT_1; i++)
            {
                strip1[i] = CHSV((uint8_t)(GLOBAL_CONFIG.lighting_hue), (uint8_t)(GLOBAL_CONFIG.saturation), 255);
            }
            update_flags &= ~(UPDATE_FLAG_HUE | UPDATE_FLAG_SATURATION);
        }

        if (update_flags & UPDATE_FLAG_BRIGHTNESS)
        {
            FastLED.setBrightness(GLOBAL_CONFIG.is_lighting_on ? GLOBAL_CONFIG.brightness : 0);
            update_flags &= ~UPDATE_FLAG_BRIGHTNESS;
        }

        FastLED.show();

        Serial.println("Lighting updated.");
        return 1;
    }

    else if (GLOBAL_CONFIG.animation_index)
    {
        int64_t dt = esp_timer_get_time() - animation_start_time;

        // ------------------------------------------------------
        // PULSE LINEAR
        // ------------------------------------------------------
        if (GLOBAL_CONFIG.animation_index == ANIMATION_PULSE_LINEAR)
        {
            int pulse = dt % (AP_PL_FALL + AP_PL_RISE);
            if (pulse < AP_PL_FALL)
            {
                FastLED.setBrightness(GLOBAL_CONFIG.is_lighting_on ? ((AP_PL_FALL - pulse) * 255) / AP_PL_FALL : 0);
            }
            else
            {
                FastLED.setBrightness(GLOBAL_CONFIG.is_lighting_on ? ((pulse - AP_PL_FALL) * 255) / AP_PL_RISE : 0);
            }
        }

        // ------------------------------------------------------
        // PULSE EXPONENTIAL
        // ------------------------------------------------------
        else if (GLOBAL_CONFIG.animation_index == ANIMATION_PULSE_EXPONENTIAL)
        {
            int pulse = dt % (AP_PE_FALL_COEF + AP_PE_RISE_COEF);
            if (pulse < AP_PE_FALL_COEF)
            {
                
            }
            else
            {

            }
        }

        // ------------------------------------------------------
        // RAINBOW
        // ------------------------------------------------------
        else if (GLOBAL_CONFIG.animation_index == ANIMATION_RAINBOW)
        {
            // Map hue from (0 to LED_COUNT_1) to (0 to 255), shift by dt
            for (int i = 0; i < LED_COUNT_1; i++)
            {
                strip1[i] = CHSV(((i * 255 / LED_COUNT_1) + dt) % 255, GLOBAL_CONFIG.saturation, GLOBAL_CONFIG.brightness);
            }
        }

        FastLED.show();
        return 0;
    }
    else
    {
        return 0;
    }
}

void update_animation()
{
    if (update_flags & UPDATE_FLAG_ANIMATION)
    {
        if (GLOBAL_CONFIG.animation_index != -1)
        {
            // New animation started
            animation_start_time = esp_timer_get_time();
        }
        else
        {
            // Animation stopped
        }
        update_flags &= ~UPDATE_FLAG_ANIMATION;
    }
}

void toggle_lighting()
{
    GLOBAL_CONFIG.is_lighting_on = GLOBAL_CONFIG.is_lighting_on ? 0 : 1;
    update_flags |= UPDATE_FLAG_BRIGHTNESS;
}

void set_brightness(int level)
{
    GLOBAL_CONFIG.brightness = level;
    update_flags |= UPDATE_FLAG_BRIGHTNESS;
}

void set_hue(int hue)
{
    GLOBAL_CONFIG.lighting_hue = hue;
    update_flags |= UPDATE_FLAG_HUE;
}

void set_saturation(int saturation)
{
    GLOBAL_CONFIG.saturation = saturation;
    update_flags |= UPDATE_FLAG_SATURATION;
}

void set_animation(int animation_index, const int* params, int params_size)
{
    GLOBAL_CONFIG.animation_index = animation_index;
    update_flags |= UPDATE_FLAG_ANIMATION;

    for (int i = 0; i < params_size; i++)
    {
        animation_params[i] = *(params + i);
    }
}

void set_all_flags()
{
    update_flags = UPDATE_FLAG_BRIGHTNESS | UPDATE_FLAG_HUE | UPDATE_FLAG_SATURATION;
}