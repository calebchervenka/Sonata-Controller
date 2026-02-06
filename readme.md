# Lighting Controller

An ESP32-based lighting controller with a touchscreen UI, designed for vehicle interior / ambient lighting control. The system uses **LVGL** for the interface and supports persistent configuration storage via SD card.

---

## Features

- **Backlight with automatic sleep**
  - Display backlight turns off after inactivity to reduce power draw.
- **Support for multiple LED strips**
  - Modular design intended to scale to multiple outputs.
- **Persistent configuration (SD card)**
  - Lighting settings are saved and restored on boot.
- **LVGL-based UI**
  - Touch-friendly interface with smooth rendering.
- **Live LED updates**
  - Enable real-time LED changes while adjusting settings by setting  
    `FAST_LIGHTING_UPDATES 1` in `lighting.h`.

---

## Configuration

### Fast Lighting Updates

To update LEDs live while making adjustments in the UI:

```c
#define FAST_LIGHTING_UPDATES
```

### Pin Assignments

**LED strip pin:** Defined in `lighting.h`

```c
#define LED_PIN_1   38
```

## Planned Features

### Lighting Patterns

- Time-Based Animations
- Reactive Effects

### Expanded Multi-LED Strip Support

- Independently control multiple LED strips.

*Note: This is currently limited by the Elecrow board this program is designed to run on.*

## Tech Stack

- MCU: ESP32
- UI: LVGL
- Storage: SD card
- LED Control: Addressable LED strips (WS2812-class)

## Notes

Ensure proper 3.3 V decoupling near the ESP32 for stable operation.

Designed with automotive environments in mind (noisy power rails).