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

// Sharp IR Setup
SharpIRProximitySensor sensorLeft(A3);
SharpIRProximitySensor sensorRight(A8);
SwipeDetector detector;

Timer disableSwipeTimer(120);

// Low Bat Detection Setup
const uint8_t BAT_PIN = 4;

boolean modeSwitch = false;
byte mode = 1;

// 1 MUSIC MODE
bool pauseOn = false;

// 3 MIDI MODE
const int channel = 1;

const int controller_x = 10; // 10 = pan position
const int controller_y = 91; // 91 = reverb level
const int controller_ir = 11; // 11 = volume/expression

const int sensor_ir = A3;

byte current_x;
byte previous_x;

byte current_y;
byte previous_y;

byte current_ir;
byte previous_ir;


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


void detectSwipe(int distance, int distance2) {
    SwipeDetector::Swipe s = detector.detect(distance, distance2);

    if (mode == 1) {

        if (s == SwipeDetector::SWIPE_RIGHT && !pauseOn) {

            Serial.println("Scan Next Track");
            consumerCommand(0x00,0x02);
            consumerCommand(0x00, 0x00);

            NeoPixel::colorWipe(50, 50, 50, 15);
            NeoPixel::colorWipe(0, 0, 0, 10);

        } else if (s == SwipeDetector::SWIPE_RIGHT && pauseOn) {
            Serial.println("Play/Pause");
            consumerCommand(0x40, 0x00);
            consumerCommand(0x00, 0x00);

            NeoPixel::colorWipe(40, 0, 80, 15);
            NeoPixel::colorWipe(0, 0, 0, 10);

            pauseOn = false;
        }

        if (s == SwipeDetector::SWIPE_LEFT && !pauseOn) {
            Serial.println("Scan Previous Track");
            consumerCommand(0x00,0x04);
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


    if (mode == 2) {


        if (s == SwipeDetector::SWIPE_RIGHT) {

            Serial.println("Arrow Right");
            BT.write(7);

            NeoPixel::colorWipe(10, 50, 20, 5);
            NeoPixel::colorWipe(0, 0, 0, 10);
        }

        if (s == SwipeDetector::SWIPE_LEFT) {
            Serial.println("Arrow Left");
            BT.write(11);

            NeoPixel::reverseColorWipe(10, 50, 20, 5);
            NeoPixel::reverseColorWipe(0, 0, 0, 10);

        }
    }

    if (mode == 4) {


        if (s == SwipeDetector::SWIPE_RIGHT) {

            NeoPixel::colorWipe(100, 0, 0, 10);
            NeoPixel::colorWipe(0, 0, 0, 10);

        }

        if (s == SwipeDetector::SWIPE_LEFT) {
            NeoPixel::reverseColorWipe(100, 0, 0, 10);
            NeoPixel::reverseColorWipe(0, 0, 0, 10);

        }
    }
    
        if (modeSwitch) {


        if (s == SwipeDetector::SWIPE_RIGHT) {

            NeoPixel::colorWipe18(40, 0, 80, 50);
            mode = 3;
            modeSwitch = false;
            
            FastLED.clear();
            FastLED.show();

        }

        if (s == SwipeDetector::SWIPE_LEFT) {
            NeoPixel::colorWipe6(50, 50, 50, 50);
            mode = 1;
            modeSwitch = false;
            
            FastLED.clear();
            FastLED.show();
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
    }

    if (mode == 2) {

        const bool hover_up = diff > -7 && diff < -1;
        const bool hover_down = diff > 1 && diff <= 7;
        const bool hold_bottom = dist <= 3 && diff == 0;

        if (hover_up)
            doArrowUp();

        if (hover_down)
            doArrowDown();

        if (hold_bottom)
            doSpace();
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
    consumerCommand(0x40, 0x00);
    consumerCommand(0x00, 0x00);

    NeoPixel::fade(100, 100, 100, 2);

    pauseOn = true;

    sensorLeft.clear();
    sensorRight.clear();
}

void doPlay()
{
    consumerCommand(0x40, 0x00);
    consumerCommand(0x00, 0x00);

    NeoPixel::fade(100, 100, 100, 2);

    pauseOn = false;

    sensorLeft.clear();
    sensorRight.clear();
}

void doArrowUp() {

    BT.write(14);

    NeoPixel::movingPixel(10, 50, 20, 15);

    sensorLeft.clear();
    sensorRight.clear();
}

void doArrowDown() {

    BT.write(12);

    NeoPixel::reverseMovingPixel(10, 50, 20, 15);

    sensorLeft.clear();
    sensorRight.clear();
}

void doSpace() {

    BT.write(0x20);

    NeoPixel::theaterchase(10, 50, 20, 10);

    sensorLeft.clear();
    sensorRight.clear();
}

void loop()
{
    // Hover
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

    if(acc & Acc::ACTIVITY)
    {
        Serial.println("### Activity");
    }

    if(acc & Acc::INACTIVITY)
    {
        Serial.println("### Inactivity");
        modeSwitch = false;
    }

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

        NeoPixel::setPixel(5, 50, 50, 50);
        NeoPixel::setPixel(7, 50, 50, 50);
        NeoPixel::setPixel(11, 10, 50, 20);
        NeoPixel::setPixel(13, 10, 50, 20);
        NeoPixel::setPixel(17, 40, 0, 80);
        NeoPixel::setPixel(19, 40, 0, 80);
        NeoPixel::setPixel(1, 0, 0, 0);
        NeoPixel::setPixel(23, 0, 0, 0);

        FastLED.show();

        const bool tilt_left = x > 60;
        const bool tilt_forward = y < -60;
        const bool tilt_right = x < -60;
        const bool tilt_backward = y > 60;

        if (tilt_left) {
            Serial.println("Mode 1: Music Mode");
            mode = 1;
            modeSwitch = false;

            FastLED.clear();
            FastLED.show();

            NeoPixel::colorWipe6(50, 50, 50, 50);

            FastLED.clear();
            FastLED.show();
        }

        if (tilt_forward) {
            Serial.println("Mode 2: Arrow Mode");
            mode = 2;
            modeSwitch = false;

            FastLED.clear();
            FastLED.show();

            NeoPixel::colorWipe12(10, 50, 20, 50);

            FastLED.clear();
            FastLED.show();
        }

        if (tilt_right) {
            Serial.println("Mode 3: MIDI Mode");
            mode = 3;
            modeSwitch = false;

            FastLED.clear();
            FastLED.show();

            NeoPixel::colorWipe18(40, 0, 80, 50);

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

        const bool tilt_left = x > 100;
        const bool tilt_forward = y < -100;
        const bool tilt_right = x < -100;
        const bool tilt_backward = y > 100;

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

        NeoPixel::midiIndicator(40, 0, 80, 0);

        int val_ir = analogRead(sensor_ir);
        int midimap_ir = map(val_ir, 0, 960, 0, 127);

        current_ir = midimap_ir;
        if (current_ir != previous_ir) {
            previous_ir = current_ir;
            usbMIDI.sendControlChange(controller_ir, current_ir, channel);
            //delay(5);
        }

        int midimap_x = map(x, -127, 127, 0, 127);

        current_x = midimap_x;
        if (current_x != previous_x) {
            previous_x = current_x;
            usbMIDI.sendControlChange(controller_x, current_x, channel);
            //delay(5);
        }

        int midimap_y = map(y, -127, 127, 0, 127);

        current_y = midimap_y;
        if (current_y != previous_y) {
            previous_y = current_y;
            usbMIDI.sendControlChange(controller_y, current_y, channel);
            //delay(5);
        }

    }

    if(!modeSwitch) {

        const bool tilt_left = x > 100;
        const bool tilt_forward = y < -100;
        const bool tilt_right = x < -100;
        const bool tilt_backward = y > 100;

        if (tilt_left) {
            NeoPixel::setPixel(6, 0, 0, 100);
            FastLED.show();
            delay(10);
            FastLED.clear();
            FastLED.show();

        }

        if (tilt_forward) {
            NeoPixel::setPixel(12, 0, 0, 100);
            FastLED.show();
            delay(10);
            FastLED.clear();
            FastLED.show();
        }

        if (tilt_right) {
            NeoPixel::setPixel(18, 0, 0, 100);
            FastLED.show();
            delay(10);
            FastLED.clear();
            FastLED.show();

        }

        if (tilt_backward) {
            NeoPixel::setPixel(0, 0, 0, 100);
            FastLED.show();
            delay(10);
            FastLED.clear();
            FastLED.show();

        }

    }
    
    // BT Ent-Pair
    if (z < -100)
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
