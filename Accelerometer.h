#include <stdint.h>
#include <Arduino.h>
#include "binary_const.h"

namespace Acc
{
    enum Events {
        NONE = 0,
        INACTIVITY = B8(1000),
        ACTIVITY =  B8(10000),
        DOUBLE_TAP = B8(100000),
        SINGLE_TAP = B8(1000000)
    };
}


template <uint8_t interrupt_pin>
class Accelerometer
{
public:
    void setup();

    void readAxis(int16_t& x, int16_t& y, int16_t& z);

    byte update();

private:

    void writeTo(byte address, byte val);

    void readFrom(byte address, uint8_t num, byte buff[]);

    byte readByte(byte address);
};