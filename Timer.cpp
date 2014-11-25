#include "Timer.h"
#include "Arduino.h"

Timer::Timer(unsigned long millis)
    : timeout_millis(millis), start_millis(::millis()), shot(false)
{
}

void Timer::setTimeout(unsigned long millis)
{
    timeout_millis = millis;
}

bool Timer::check()
{
    return (millis() - start_millis) >= timeout_millis;
}

bool Timer::singleShot()
{
    if(check() && !shot) {
        shot = true;
        return true;
    }
    return false;
}

void Timer::reset()
{
    shot = false;
    start_millis = millis();
}


