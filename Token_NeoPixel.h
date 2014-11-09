#include <inttypes.h>


namespace NeoPixel
{
    void setup();

    const int NUM_LEDS = 24;
    const uint8_t DATA_PIN = 22;

    void colorWipe(uint8_t r, uint8_t g, uint8_t b, uint8_t wait);
    void reverseColorWipe(uint8_t r, uint8_t g, uint8_t b, uint8_t wait);
    void movingPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t wait);
    void reverseMovingPixel(uint8_t r, uint8_t g, uint8_t b, uint8_t wait);
    void fade(uint8_t r, uint8_t g, uint8_t b, uint8_t wait);
    void theaterchase(uint8_t r, uint8_t g, uint8_t b, uint8_t wait);
    void firstLight(uint8_t r, uint8_t g, uint8_t b, uint8_t wait);
}
