#include <FastLED.h>
#include <stdbool.h>

#include "config.h"

/**
 * Pin 1: Front-left
 * Pin 2: Front-right
 */
#define LED_PIN_1   38
// #define LED_PIN_2   38
#define LED_COUNT_1 144
// #define LED_COUNT_2 144

#define UPDATE_FLAG_SATURATION  0x1
#define UPDATE_FLAG_BRIGHTNESS  0x2
#define UPDATE_FLAG_HUE         0x4
#define UPDATE_FLAG_4           0x8

unsigned int update_flags = ~0;

CRGB strip1[LED_COUNT_1];
// CRGB strip2[LED_COUNT_2];

void lighting_init()
{
    FastLED.addLeds<WS2812, LED_PIN_1, GRB>(strip1, LED_COUNT_1);
    // FastLED.addLeds<WS2812, LED_PIN_2, GRB>(strip2, LED_COUNT_2);
}

/**
 * Returns true if any lighting data was changed
 */
int update_lighting(int force_all = 0)
{
    if (force_all) update_flags = ~0;
    if (update_flags)
    {   
        if (update_flags & UPDATE_FLAG_BRIGHTNESS)
        {
            FastLED.setBrightness(GLOBAL_CONFIG.is_lighting_on ? GLOBAL_CONFIG.brightness : 0);
        }

        if (update_flags & (UPDATE_FLAG_HUE | UPDATE_FLAG_SATURATION))
        {
            for (int i = 0; i < LED_COUNT_1; i++)
            {
                strip1[i] = CHSV((uint8_t)(GLOBAL_CONFIG.lighting_hue), (uint8_t)(GLOBAL_CONFIG.saturation), 255);
            }

            // for (int i = 0; i < LED_COUNT_2; i++)
            // {
            //     strip2[i] = CHSV((uint8_t)(GLOBAL_CONFIG.lighting_hue), 255, 255);
            // }
        }

        update_flags = 0;
        FastLED.show();

        return 1;
    }
    else
    {
        return 0;
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