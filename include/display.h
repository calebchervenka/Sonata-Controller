#include <Arduino_GFX_Library.h>
#include <lvgl.h>

#include "touch.h"
#include "backlight.h"

// Display
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *disp_draw_buf;
static lv_disp_drv_t disp_drv;

// Input
static lv_indev_drv_t indev_drv;

Arduino_ESP32RGBPanel *bus = new Arduino_ESP32RGBPanel(
  GFX_NOT_DEFINED /* CS */, GFX_NOT_DEFINED /* SCK */, GFX_NOT_DEFINED /* SDA */,
  40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 0 /* PCLK */,
  45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
  5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
  8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */
);

Arduino_RPi_DPI_RGBPanel *lcd = new Arduino_RPi_DPI_RGBPanel(
  bus,
  800 /* width */, 0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 43 /* hsync_back_porch */,
  480 /* height */, 0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 12 /* vsync_back_porch */,
  1 /* pclk_active_neg */, 16000000 /* prefer_speed */, true /* auto_flush */
);

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;

  /* Cast the LVGL color buffer directly to the 16-bit pixel array expected
     by the Arduino_GFX draw function. Using &color_p->full can point to a
     single member and cause incorrect addressing/stride; casting the base
     pointer is correct for contiguous pixel data (RGB565). */
  lcd->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t *)color_p, w, h);
  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  if (ts.isTouched) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = SCREEN_WIDTH - ts.points[0].x;
    data->point.y = SCREEN_HEIGHT - ts.points[0].y;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void display_init() {
  touch_init();

  lcd->begin();
  lv_init();

  // allocate buffer (One quarter full screen or some size)
  disp_draw_buf = (lv_color_t*)heap_caps_malloc(sizeof(lv_color_t) * SCREEN_WIDTH * SCREEN_HEIGHT / 4, MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, SCREEN_WIDTH * SCREEN_HEIGHT / 4);

  /* Allocate the LVGL draw buffer in DMA-capable memory so the display
    driver (which may use DMA) reads valid RAM. Use MALLOC_CAP_DMA. */
  // disp_draw_buf = (lv_color_t*)heap_caps_malloc(sizeof(lv_color_t) * SCREEN_WIDTH * 40, MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
  // lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, SCREEN_WIDTH * 40);


  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = SCREEN_WIDTH;
  disp_drv.ver_res = SCREEN_HEIGHT;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // input device (touch)
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  ts.setRotation(ROTATION_NORMAL);
}
