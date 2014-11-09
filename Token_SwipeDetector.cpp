#include "Token_SwipeDetector.h"
#include <Arduino.h>


SwipeDetector::Swipe SwipeDetector::detect(int left, int right)
{
    return filter((char) (isTriggered(left) * 2 | isTriggered(right)));
}

void SwipeDetector::reset()
{
    z = REST;
}

bool SwipeDetector::isTriggered(int distance)
{
    if (distance != -1 && distance < 30)
    {
        return true;
    }

    return false;
}

SwipeDetector::Swipe SwipeDetector::filter(char t)
{
    return stateMachineInternal(t);
}

SwipeDetector::Swipe SwipeDetector::stateMachineInternal(char t)
{

    switch (z)
    {

        case REST:
            if (t == T01)
            {
                z = Z1;
            }

            if (t == T10)
            {
                z = Z4;
            }
            break;

        case Z1:
            if (t == T00)
            {
                z = REST;

            }
            if (t == T10)
            {
                z = REST;
            }

            if (t == T11)
            {
                z = Z2;
            }
            break;

        case Z2:
            if (t == T00)
            {
                z = REST;
            }

            if (t == T01)
            {
                z = REST;
            }

            if (t == T10)
            {
                z = Z3;
            }
            break;

        case Z3:
            if (t == T01)
            {
                z = REST;
            }

            if (t == T10)
            {
            }

            if (t == T00)
            {
                z = REST;
                return SWIPE_LEFT;
            }
            break;

        case Z4:

            if (t == T00)
            {
                z = REST;
            }

            if (t == T01)
            {
                z = REST;
            }

            if (t == T11)
            {
                z = Z5;
            }
            break;

        case Z5:

            if (t == T00)
            {
                z = REST;
            }

            if (t == T10)
            {
                z = REST;
            }

            if (t == T01)
            {
                z = Z6;
            }
            break;

        case Z6:

            if (t == T10)
            {
                z = REST;
            }

            if (t == T01)
            {
            }

            if (t == T00)
            {
                z = REST;
                return SWIPE_RIGHT;
            }
            break;
    }

    return SWIPE_NONE;
}

SwipeDetector::SwipeDetector() :
    z(REST)
{
}
