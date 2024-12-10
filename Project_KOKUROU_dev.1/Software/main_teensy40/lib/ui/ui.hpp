#ifndef _UI_HPP
#define _UI_HPP

#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


extern Adafruit_SSD1306 display;

extern void ui_init();
extern void ui_loop();

#endif