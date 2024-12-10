#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "neopixel.hpp"
#include "config.hpp"

Adafruit_NeoPixel engel_neopixel = Adafruit_NeoPixel(   NEOPIXEL_ENGEL_PIXELS, 
                                                        NEOPIXEL_ENGEL_PIN, 
                                                        NEO_GRB + NEO_KHZ800    );

Adafruit_NeoPixel indicator_neopixel = Adafruit_NeoPixel(   NEOPIXEL_INDICATOR_PIXELS, 
                                                            NEOPIXEL_INDICATOR_PIN, 
                                                            NEO_GRB + NEO_KHZ800    );

void neopixel_init()
{
    if(IS_ENABLE_NEOPIXEL == 1)
    {
        engel_neopixel.begin();
        engel_neopixel.setBrightness(32);

        for(int i = 0; i < 32; i++)
        {
            engel_neopixel.setPixelColor(i, engel_neopixel.Color(0, 0, 0));
        }

        engel_neopixel.show();

        indicator_neopixel.begin();
        indicator_neopixel.setBrightness(32);

        indicator_neopixel.setPixelColor(0, indicator_neopixel.Color(0, 0, 0));
        indicator_neopixel.setPixelColor(1, indicator_neopixel.Color(0, 0, 0));
        indicator_neopixel.setPixelColor(2, indicator_neopixel.Color(0, 0, 0));

        indicator_neopixel.show();
    }
}

void neopixel_engel_loop()
{
    if(IS_ENABLE_NEOPIXEL == 1)
    {
        engel_neopixel.clear();

        for(int i = 0; i < 32; i++)
        {
            engel_neopixel.setPixelColor(i, engel_neopixel.Color(255, 0, 255));
        }

        engel_neopixel.show();
    }
    else
    {
        engel_neopixel.clear();
        
        for(int i = 0; i < 32; i++)
        {
            engel_neopixel.setPixelColor(i, engel_neopixel.Color(0, 0, 0));
        }
        
        engel_neopixel.show();
    }
}

void neopixel_indicator_loop()
{
    if(IS_ENABLE_NEOPIXEL == 1)
    {
        indicator_neopixel.clear();
        
        indicator_neopixel.setPixelColor(0, indicator_neopixel.Color(255, 0, 0));
        indicator_neopixel.setPixelColor(1, indicator_neopixel.Color(0, 255, 0));
        indicator_neopixel.setPixelColor(2, indicator_neopixel.Color(0, 0, 255));

        //indicator_neopixel.show();
    }
    else
    {
        indicator_neopixel.clear();
        
        indicator_neopixel.setPixelColor(0, indicator_neopixel.Color(0, 0, 0));
        indicator_neopixel.setPixelColor(1, indicator_neopixel.Color(0, 0, 0));
        indicator_neopixel.setPixelColor(2, indicator_neopixel.Color(0, 0, 0));

        indicator_neopixel.show();
    }
}