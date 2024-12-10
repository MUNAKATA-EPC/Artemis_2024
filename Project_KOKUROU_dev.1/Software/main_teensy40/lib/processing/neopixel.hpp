#ifndef _NEOPIXEL_HPP
#define _NEOPIXEL_HPP

#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_ENGEL_PIN 5
#define NEOPIXEL_ENGEL_PIXELS 32

#define NEOPIXEL_INDICATOR_PIN 22
#define NEOPIXEL_INDICATOR_PIXELS 3

extern Adafruit_NeoPixel engel_neopixel;
extern Adafruit_NeoPixel indicator_neopixel;

extern void neopixel_init();
extern void neopixel_engel_loop();
extern void neopixel_indicator_loop();

#endif