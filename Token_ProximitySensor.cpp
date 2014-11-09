#include "Token_ProximitySensor.h"
#include <Arduino.h>


void SharpIRProximitySensor::clear()
{
    memset(_hist, 0, FILTER_SIZE * sizeof(int));
}

int SharpIRProximitySensor::read()
{
    float volts = analogRead(_pin) * 0.0048828125;   // value from sensor * (5/1024) - if running 3.3.volts then change 5 to 3.3
    int distance = (int) (13 * pow(volts, -1));          // worked out from graph 65 = theoretical distance / (1/Volts)S

    return filter(distance);
}

int SharpIRProximitySensor::filter(int input)
{
    _hist[_curr] = input;

    bool first = true;
    for (int i = _curr; _curr != i || first; i = (i + 1) % FILTER_SIZE)
    {
        if (_hist[i] < 30 && _hist[i] != 0)
        {
            inc();
            return _hist[i];
        }
        first = false;
    }

    inc();
    return -1;
}

void SharpIRProximitySensor::inc()
{
    _curr++;
    if (_curr >= FILTER_SIZE)
    {
        _curr = 0;
    }
}

SharpIRProximitySensor::SharpIRProximitySensor(uint8_t pin) :
    _pin(pin), _curr(0)
{
    clear();
}
