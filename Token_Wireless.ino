#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "SwipeDetector.h"
#include "ProximitySensor.h"
#include "Timer.h"
#include "NeoPixel.h"
#include "FastLED.h"
#include "Accelerometer.h"


SoftwareSerial BT(33, 11);
Accelerometer<10> accelerometer;

Timer disableSwipeTimer(120);

// Sharp IR Setup
SharpIRProximitySensor sensorLeft(A3);
SharpIRProximitySensor sensorRight(A8);
SwipeDetector detector;

// Low Bat Detection Setup
const uint8_t BAT_PIN = 4;

boolean modeSwitch = false;
byte mode = 3;

// mode 1
bool pauseOn = false;

//mode 3
const int sensor = A3;

byte current;
byte previous;

byte current2;
byte previous2;


void doVolumeUp();
void doVolumeUpMore();
void doVolumeDown();
void doVolumeDownMore();
void doPause();
void doPlay();
void doArrowUp();
void doArrowDown();
void doSpace();

void consumerCommand(uint8_t mask0, uint8_t mask1);

void setup()
{
    Serial.begin(9600);
    BT.begin(9600);

    analogReadResolution(10);
    analogReadAveraging(4);

    NeoPixel::setup();

    pinMode(4, INPUT); // Low Bat Detection
    pinMode(8, OUTPUT); // BT Pair
    pinMode(13, OUTPUT); // Indicator LED

    accelerometer.setup();
}


void detectSwipe(int distance, int distance2)
{
    SwipeDetector::Swipe s = detector.detect(distance, distance2);

    if (mode == 1) {

        if (s == SwipeDetector::SWIPE_RIGHT && !pauseOn) {

            Serial.println("Next Track");
            consumerCommand(0x80, 0x00);
            consumerCommand(0x00, 0x00);

            NeoPixel::colorWipe(50, 50, 50, 15);
            NeoPixel::colorWipe(0, 0, 0, 10);
        }
        else if (s == SwipeDetector::SWIPE_RIGHT && pauseOn) {
            Serial.println("Play/Pause");
            consumerCommand(0x40, 0x00);
            consumerCommand(0x00, 0x00);

            NeoPixel::colorWipe(40, 0, 80, 15);
            NeoPixel::colorWipe(0, 0, 0, 10);

            pauseOn = false;
        }

        if (s == SwipeDetector::SWIPE_LEFT && !pauseOn) {
            Serial.println("Previous Track");
            consumerCommand(0x00, 0x01);
            consumerCommand(0x00, 0x00);

            NeoPixel::reverseColorWipe(50, 50, 50, 15);
            NeoPixel::reverseColorWipe(0, 0, 0, 10);


        } else if (SwipeDetector::SWIPE_LEFT == s && pauseOn) {
            Serial.println("Play/Pause");
            consumerCommand(0x40, 0x00);
            consumerCommand(0x00, 0x00);

            NeoPixel::reverseColorWipe(50, 50, 50, 15);
            NeoPixel::reverseColorWipe(0, 0, 0, 10);

            pauseOn = false;
        }
    }


    if(mode == 2) {


        if (s == SwipeDetector::SWIPE_RIGHT && mode == 2) {

            Serial.println("Arrow Right");
            BT.write(7);

            NeoPixel::colorWipe(10, 50, 20, 5);
            NeoPixel::colorWipe(0, 0, 0, 10);
        }


        if (s == SwipeDetector::SWIPE_LEFT && mode == 2) {
            Serial.println("Arrow Left");
            BT.write(11);

            NeoPixel::reverseColorWipe(10, 50, 20, 5);
            NeoPixel::reverseColorWipe(0, 0, 0, 10);

        }
    }
}


void detectHover(int start, int dist)
{
    int diff = start - dist;

    if (mode == 1) {

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
    if (mode == 2) {

        const bool hover_up = diff > -7 && diff < -1;
        const bool hover_down = diff > 1 && diff <= 7;

        if (hover_up)
            doArrowUp();

        if (hover_down)
            doArrowDown();
    }
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

void doArrowUp() {

    BT.write(14);

    NeoPixel::movingPixel(20, 100, 40, 10);

    sensorLeft.clear();
    sensorRight.clear();
}

void doArrowDown() {

    BT.write(12);

    NeoPixel::reverseMovingPixel(100, 40, 20, 10);

    sensorLeft.clear();
    sensorRight.clear();
}

void doSpace() {

    BT.write(0x20);

    NeoPixel::reverseMovingPixel(100, 0, 0, 10);

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

    // Accelerometer
    byte acc = accelerometer.update();

    if(acc & Acc::DOUBLE_TAP && !modeSwitch)
    {
        Serial.println("### TILT TO SELECT MODE");
        modeSwitch = true;
    }

    else if(acc & Acc::SINGLE_TAP)
    {
        Serial.println("### Single Tap");
    }

    int16_t x, y, z;

    accelerometer.readAxis(x, y, z);

//    Serial.print("Axis: ");
//    Serial.print(x);
//    Serial.print("   ");
//    Serial.print(y);
//    Serial.print("   ");
//    Serial.println(z);
//    delay(50);


    if (modeSwitch) {

        NeoPixel::setPixel(23, 50, 50, 50);
        NeoPixel::setPixel(1, 50, 50, 50);
        NeoPixel::setPixel(5, 10, 50, 20);
        NeoPixel::setPixel(7, 10, 50, 20);
        NeoPixel::setPixel(11, 40, 0, 80);
        NeoPixel::setPixel(13, 40, 0, 80);
        NeoPixel::setPixel(17, 0, 0, 0);
        NeoPixel::setPixel(19, 0, 0, 0);

        FastLED.show();

        const bool tilt_left = x < -100;
        const bool tilt_forward = y > 100;
        const bool tilt_right = x > 100;
        const bool tilt_backward = y < -100;

        if (tilt_left) {
            Serial.println("Mode 1: Music Mode");
            mode = 1;
            modeSwitch = false;

            FastLED.clear();
            FastLED.show();

            NeoPixel::colorWipe(50, 50, 50, 50);

            FastLED.clear();
            FastLED.show();
        }

        if (tilt_forward) {
            Serial.println("Mode 2: Arrow Mode");
            mode = 2;
            modeSwitch = false;

            FastLED.clear();
            FastLED.show();

            NeoPixel::colorWipe6(10, 50, 20, 50);

            FastLED.clear();
            FastLED.show();
        }

        if (tilt_right) {
            Serial.println("Mode 3: MIDI Mode");
            mode = 3;
            modeSwitch = false;

            FastLED.clear();
            FastLED.show();

            NeoPixel::colorWipe12(40, 0, 80, 50);

            FastLED.clear();
            FastLED.show();
        }

        if (tilt_backward) {
            Serial.println("Mode 4: Test Mode");
            mode = 4;
            modeSwitch = false;

            FastLED.clear();
            FastLED.show();
        }
    }


    if (mode == 1 && !modeSwitch) {

        const bool tilt_left = x < -100;
        const bool tilt_forward = y > 100;
        const bool tilt_right = x > 100;
        const bool tilt_backward = y < -100;

        if (tilt_left) {
            //Serial.println("Previous Track");
            //consumerCommand(0x00, 0x01);
            //consumerCommand(0x00, 0x00);

            //NeoPixel::reverseColorWipe(50, 50, 50, 15);
            //NeoPixel::reverseColorWipe(0, 0, 0, 10);
        }

        if (tilt_forward) {

        }

        if (tilt_right) {
            //Serial.println("Next Track");
            //consumerCommand(0x80, 0x00);
            //consumerCommand(0x00, 0x00);

            //NeoPixel::colorWipe(50, 50, 50, 15);
            //NeoPixel::colorWipe(0, 0, 0, 10);

        }

        if (tilt_backward) {

        }

    }

    if (mode == 3 && !modeSwitch) {
        int val = analogRead(A3);
        int midimap = map(val, 0, 960, 0, 127);

        NeoPixel::midiIndicator(40, 0, 80, 0);

        current = midimap;
        if (current != previous) {
            previous = current;
            usbMIDI.sendControlChange(11, current, 1);
            //delay(5);
        }


        int midimap_x = map(x, -127, 127, 0, 127);

        current2 = midimap_x;
        if (current2 != previous2) {
            previous2 = current2;
            usbMIDI.sendControlChange(10, current2, 1);
            //delay(5);
        }

//        int midimap_y = map(y, -255, 270, 0, 127);
//        usbMIDI.sendControlChange(1, midimap_y, 1);
//
//        int midimap_z = map(z, -255, 270, 0, 127);
//        usbMIDI.sendControlChange(3, midimap_z, 3);
    }



    if(!modeSwitch) {

        if (x < -100)
        {
            NeoPixel::setPixel(0, 0, 0, 100);
            FastLED.show();
            delay(10);
            FastLED.clear();
            FastLED.show();
        }

        if (y > 100)
        {
            NeoPixel::setPixel(6, 0, 0, 100);
            FastLED.show();
            delay(10);
            FastLED.clear();
            FastLED.show();
        }

        if (x > 100)
        {
            NeoPixel::setPixel(12, 0, 0, 100);
            FastLED.show();
            delay(10);
            FastLED.clear();
            FastLED.show();
        }

        if (y < -100)
        {
            NeoPixel::setPixel(18, 0, 0, 100);
            FastLED.show();
            delay(10);
            FastLED.clear();
            FastLED.show();
        }
    }


    // BT Ent-Pair
    if (z < -120)
    {
        digitalWrite(8, HIGH);
        digitalWrite(13, HIGH);
    } else
    {
        digitalWrite(8, LOW);
        digitalWrite(13, LOW);
    }

    // Low Bat Detection
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
