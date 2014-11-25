#include <string.h>
#include <stdint.h>

/**
* Ignores up to FILTER_SIZE - 1 invalid values
*/
class SharpIRProximitySensor
{
public:
    SharpIRProximitySensor(uint8_t pin);
    void clear();
    int read();

private:
    static const int FILTER_SIZE = 3;
    const uint8_t _pin;

    int _hist[FILTER_SIZE];
    int _curr;

    void inc();
    int filter(int);
};