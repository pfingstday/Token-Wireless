#include "NeoPixel.h"
#include "FastLED.h"

namespace NeoPixel
{
    CRGB leds[NUM_LEDS];

    void setup()
    {
        FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    }

    void setPixel(int i, uint8_t r, uint8_t g, uint8_t b)
    {
        leds[i] = CRGB(r, g, b);
    }

    void show()
    {
        FastLED.show();
    }

    void midiIndicator(uint8_t r, uint8_t g, uint8_t b, uint8_t wait)
    {
        int val = analogRead(A3);
        int numLedsToLight = map(val, 20, 950, 0, 24);
        FastLED.clear();
        for (int led = 0; led < numLedsToLight; led++) {
            leds[led] = CRGB(40, 0, 80);
        }
        FastLED.show();
    }

    void colorWipe(uint8_t r, uint8_t g, uint8_t b, uint8_t wait)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(r, g, b);
            FastLED.show();
            delay(wait);
        }
    }

    void colorWipe6(uint8_t r, uint8_t g, uint8_t b, uint8_t wait)
    {
        for (int i = 5; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(r, g, b);
            FastLED.show();
            delay(wait);
        }

        for (int i = 0; i < 7; i++)
        {
            leds[i] = CRGB(r, g, b);
            FastLED.show();
            delay(wait);
        }
    }

    void colorWipe12(uint8_t r, uint8_t g, uint8_t b, uint8_t wait)
    {
        for (int i = 11; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(r, g, b);
            FastLED.show();
            delay(wait);
        }

        for (int i = 0; i < 13; i++)
        {
            leds[i] = CRGB(r, g, b);
            FastLED.show();
            delay(wait);
        }

    }

    void reverseColorWipe(uint8_t r, uint8_t g, uint8_t b, uint8_t wait)
    {
        for (int i = NUM_LEDS - 1; i >= 0; i--)
        {
            leds[i] = CRGB(r, g, b);
            FastLED.show();
            delay(wait);
        }
    }

    void movingPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t wait)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(r, g, b);
            FastLED.show();
            leds[i] = CRGB::Black;
            delay(wait);
        }
        FastLED.show();
    }

    void reverseMovingPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t wait)
    {
        for (int i = NUM_LEDS - 1; i >= 0; i--)
        {
            leds[i] = CRGB(r, g, b);
            FastLED.show();
            leds[i] = CRGB::Black;
            delay(wait);
        }
        FastLED.show();
    }

    void theaterchase(uint8_t r, uint8_t g, uint8_t b, uint8_t wait)
    {
        for (int j = 0; j < 10; j++)
        {  //do 10 cycles of chasing
            for (int q = 0; q < 3; q++)
            {
                for (int i = 0; i < NUM_LEDS; i = i + 3)
                {
                    leds[i + q] = CRGB(r, g, b);    //turn every third pixel on
                }
                FastLED.show();

                delay(wait);

                for (int i = 0; i < NUM_LEDS; i = i + 3)
                {
                    leds[i + q] = CRGB::Black;
                }
                FastLED.show();
            }
        }
    }

    void fade(uint8_t r, uint8_t g, uint8_t b, uint8_t wait)
    {
        const int steps = 256;
        const int scale = 4;
        int step_r = r * scale / steps;
        int step_g = g * scale / steps;
        int step_b = b * scale / steps;
        int cur_r = 0;
        int cur_b = 0;
        int cur_g = 0;
        for (int k = 0; k < steps; k++)
        {
            cur_r += step_r;
            cur_g += step_g;
            cur_b += step_b;
            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB((uint8_t)(cur_r / scale), (uint8_t)(cur_g / scale), (uint8_t)(cur_b / scale));
            }
            FastLED.show();
            delay(wait);
        }

        for (int k = steps; k > 0; k--)
        {
            cur_r -= step_r;
            cur_g -= step_g;
            cur_b -= step_b;

            for (int i = 0; i < NUM_LEDS; i++)
            {
                leds[i] = CRGB((uint8_t)(cur_r / scale), (uint8_t)(cur_g / scale), (uint8_t)(cur_b / scale));
            }
            FastLED.show();
            delay(wait);
        }
    }

    void firstLight(uint8_t r, uint8_t g, uint8_t b, uint8_t wait)
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            leds[i] = CRGB(r, g, b);
            FastLED.show();
            delay(wait);
            leds[i] = CRGB::Black;
        }
        FastLED.show();
    }
}
