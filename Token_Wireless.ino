#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Token_SwipeDetector.h"
#include "Token_ProximitySensor.h"
#include "Token_Timer.h"
#include "Token_NeoPixel.h"
#include "FastLED.h"

SoftwareSerial BT(24, 16);

Timer disableSwipeTimer(120);

// Sharp IR Setup
SharpIRProximitySensor sensorLeft(A1);
SharpIRProximitySensor sensorRight(A0);
SwipeDetector detector;

// Low Bat Detection Setup
// const uint8_t LED_PIN = 13;
const uint8_t BAT_PIN = 4;

bool pauseOn = false;

void doVolumeUp();
void doVolumeUpMore();
void doVolumeDown();
void doVolumeDownMore();
void doPause();

void consumerCommand(uint8_t mask0, uint8_t mask1);

void setup()
{
    Serial.begin(9600);
    BT.begin(9600);

    NeoPixel::setup();

    pinMode(4, INPUT);
    pinMode(7, OUTPUT);
    pinMode(13, OUTPUT);
}


void detectSwipe(int distance, int distance2)
{
    SwipeDetector::Swipe s = detector.detect(distance, distance2);

    if (s == SwipeDetector::SWIPE_RIGHT && !pauseOn)
    {
        Serial.println("Next Track");
        consumerCommand(0x80, 0x00);
        consumerCommand(0x00, 0x00);

        NeoPixel::colorWipe(50, 50, 50, 15);
        NeoPixel::colorWipe(0, 0, 0, 10);
    }
    else if (s == SwipeDetector::SWIPE_RIGHT && pauseOn)
    {
        Serial.println("Play/Pause");
        consumerCommand(0x40, 0x00);
        consumerCommand(0x00, 0x00);

        NeoPixel::colorWipe(40, 0, 80, 15);
        NeoPixel::colorWipe(0, 0, 0, 10);

        pauseOn = false;
    }

    if (s == SwipeDetector::SWIPE_LEFT && !pauseOn)
    {
        Serial.println("Previous Track");
        consumerCommand(0x00, 0x01);
        consumerCommand(0x00, 0x00);

        NeoPixel::reverseColorWipe(50, 50, 50, 15);
        NeoPixel::reverseColorWipe(0, 0, 0, 10);


    } else if (SwipeDetector::SWIPE_LEFT == s && pauseOn)
    {
        Serial.println("Play/Pause");
        consumerCommand(0x40, 0x00);
        consumerCommand(0x00, 0x00);

        NeoPixel::reverseColorWipe(40, 0, 80, 15);
        NeoPixel::reverseColorWipe(0, 0, 0, 10);

        pauseOn = false;
    }
}


void detectHover(int start, int dist)
{
    int diff = start - dist;

    const bool hover_up = diff > -7 && diff < -1;
    const bool hover_up_more = diff <= -8;
    const bool nullpunkt_indikator = dist >= 5 && diff == 0;
    const bool hover_down = diff > 1 && diff <= 7;
    const bool hover_down_more = diff >= 8;
    const bool hold_bottom = dist <= 3 && diff == 0 && !pauseOn;
    const bool hold_bottom_again = dist <= 3 && diff == 0 && pauseOn;

    if(hold_bottom_again)
        doPlay();

    if (pauseOn)
        return;
    
    if (hover_up)
        doVolumeUp();

    if (hover_up_more)
        doVolumeUpMore();

    if (hover_down)
        doVolumeDown();

    if (hover_down_more)
        doVolumeDownMore();
    
    if (hold_bottom)
        doPause();

//    if (nullpunkt_indikator)
//        NeoPixel::theaterchase(20, 20, 20, 20);
}

void doVolumeDownMore()
{
    consumerCommand(0x20, 0x00);
    consumerCommand(0x00, 0x00);

    NeoPixel::reverseMovingPixel(230, 200, 200, 5);

    sensorLeft.clear();
    sensorRight.clear();
}

void doVolumeDown()
{
    consumerCommand(0x20, 0x00);
    consumerCommand(0x00, 0x00);

    NeoPixel::reverseMovingPixel(200, 200, 200, 10);

    sensorLeft.clear();
    sensorRight.clear();
}

void doVolumeUpMore()
{
    consumerCommand(0x10, 0x00);
    consumerCommand(0x00, 0x00);

    NeoPixel::movingPixel(200, 200, 200, 5);

    sensorLeft.clear();
    sensorRight.clear();
}

void doVolumeUp()
{
    consumerCommand(0x10, 0x00);
    consumerCommand(0x00, 0x00);

    NeoPixel::movingPixel(200, 200, 200, 10);
        
    sensorLeft.clear();
    sensorRight.clear();
}

void doPause()
{
    Serial.println("Pause");
    consumerCommand(0x40, 0x00);
    consumerCommand(0x00, 0x00);

    NeoPixel::fade(100, 100, 100, 2);

    pauseOn = true;

    sensorLeft.clear();
    sensorRight.clear();
}

void doPlay()
{
    Serial.println("Play");
    consumerCommand(0x40, 0x00);
    consumerCommand(0x00, 0x00);
    
    NeoPixel::fade(100, 100, 100, 2);

    pauseOn = false;
    
    sensorLeft.clear();
    sensorRight.clear();
}

void loop()
{
    static int hover_start;

    int distance = sensorLeft.read();
    int distance2 = sensorRight.read();
    int avgDist = (distance + distance2) / 2;

    if (distance != -1 && distance2 != -1)
    {
        Serial.print(distance);
        Serial.print("   ");
        Serial.print(distance2);
        Serial.print("   ");
        Serial.println(avgDist);
    } else
    {
        disableSwipeTimer.reset();
    }

    if (disableSwipeTimer.singleShot())
    {
        hover_start = avgDist;
        Serial.print("Hover Start: ");
        Serial.println(hover_start);
    }

    if (disableSwipeTimer.check())
    {
        detectHover(hover_start, avgDist);
        detector.reset();
    } else
    {
        detectSwipe(distance, distance2);
    }


    if (digitalRead(BAT_PIN) == 0)
    {
        NeoPixel::fade(80, 0, 0, 10);
    }
}


void consumerCommand(uint8_t mask0, uint8_t mask1)
{
    BT.write(0xFD);
    BT.write((byte) 0x00);
    BT.write((byte) 0x02);
    BT.write(mask0);
    BT.write(mask1);
    BT.write((byte) 0x00);
    BT.write((byte) 0x00);
    BT.write((byte) 0x00);
    BT.write((byte) 0x00);
}
