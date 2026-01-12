#include <Wire.h>
#include <TAMC_GT911.h>

#define TOUCH_SDA 19
#define TOUCH_SCL 20
#define TOUCH_INT 0
#define TOUCH_RST 38
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 480
#define TOUCH_GT911_ROTATION ROTATION_NORMAL

int touch_last_x, touch_last_y;

TAMC_GT911 ts(TOUCH_SDA, TOUCH_SCL, TOUCH_INT, TOUCH_RST, SCREEN_WIDTH, SCREEN_HEIGHT);

void touch_init()
{
  // --- HARD RESET SEQUENCE ---
  pinMode(TOUCH_RST, OUTPUT);
  pinMode(TOUCH_INT, OUTPUT);
  digitalWrite(TOUCH_RST, LOW);
  digitalWrite(TOUCH_INT, LOW);
  delay(10);
  digitalWrite(TOUCH_INT, HIGH);
  delay(2);
  digitalWrite(TOUCH_RST, HIGH);
  delay(50);
  // ----------------------------

  Wire.begin(TOUCH_SDA, TOUCH_SCL);
  delay(100);

  ts.begin(0x5D);
}

bool touch_has_signal()
{
  // ts.read();
  return ts.isTouched;
}

bool touch_touched()
{
  // ts.read();
  if (ts.isTouched)
  {
    touch_last_x = ts.points[0].x;
    touch_last_y = ts.points[0].y;
    return true;
  }
  return false;
}

bool touch_released()
{
  // ts.read();
  return !ts.isTouched;
}