//#include <Arduino.h>
#include <SendOnlySoftwareSerial.h>
SendOnlySoftwareSerial BT (2);

#include <Adafruit_NeoPixel.h>
#define PIN 13
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, PIN, NEO_GRB + NEO_KHZ800);

// Timer Lib
#include <Timer.h>

// Swipe Detector Lib
#include "SwipeDetector.h"
#include "ProximitySensor.h"

// Sharp IR Setup
SharpIRProximitySensor sensorLeft(2);
SharpIRProximitySensor sensorRight(3);
SwipeDetector detector;

Timer disableSwipeTimer(100);

void consumerCommand(uint8_t mask0,uint8_t mask1);
void colorWipe(uint32_t c, uint8_t wait);
void reverseColorWipe(uint32_t c, uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);

void setup()
{
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'

    Serial.begin(9600);
    BT.begin(9600);
}


void detectSwipe(int distance2, int distance)
{
    SwipeDetector::Swipe s = detector.detect(distance, distance2);

    if (s == SwipeDetector::SWIPE_RIGHT) {

       // Serial.println("Next Track");
       // consumerCommand(0x80,0x00);
       // consumerCommand(0x00,0x00);
        
        Serial.println("Scan Next Track");
        consumerCommand(0x00,0x02);
        consumerCommand(0x00,0x00);
        
        // Serial.println("Right Arrow");
        // BT.write(7);

        colorWipe(strip.Color(25, 50, 0), 20);
        colorWipe(strip.Color(0, 0, 0), 20);
    }

    if (s == SwipeDetector::SWIPE_LEFT) {

        // Serial.println("Previous Track");
        // consumerCommand(0x00,0x01);
        // consumerCommand(0x00,0x00);
        
        Serial.println("Scan Prevoius Track");
        consumerCommand(0x00,0x04);
        consumerCommand(0x00,0x00); 
     
        // Serial.println("Left Arrow");
        // BT.write(11);    

        reverseColorWipe(strip.Color(50, 0, 1), 20);
        reverseColorWipe(strip.Color(0, 0, 0), 20);
    }
}


void detectHover(int start, int dist) 
{
    int diff = start-dist;
    //Serial.println(dist);

    if (diff < -1) {

        //Serial.println("Volume Up");
        consumerCommand(0x10,0x00);
        consumerCommand(0x00,0x00);
        
        //BT.write(14);
        //Serial.println("Up Arrow");

        theaterChase(strip.Color(25, 50, 10), 2);
        theaterChase(strip.Color(0, 0, 0), 0);

        sensorLeft.clear();
        sensorRight.clear();
    }

    if (diff > 2) {

        //Serial.println("Volume Down");
        consumerCommand(0x20,0x00);
        consumerCommand(0x00,0x00);

        //Serial.println("Down Arrow");
        //BT.write(12);
        
        theaterChase(strip.Color(40, 10, 5), 1);
        theaterChase(strip.Color(0, 0, 0), 0);

        sensorLeft.clear();
        sensorRight.clear();
    }
}

const int MAX = 30;
const int MIN = 4;

void detectHover1(int start, int dist)
{
    int diff = start-dist;

    //Serial.println(dist);

    if (diff < -1) {

        // Volume Up
        consumerCommand(0x10,0x00);
        consumerCommand(0x00,0x00);
        //Serial.println("Volume Up");

        //theaterChase(strip.Color(25, 50, 0), 20);
        //theaterChase(strip.Color(0, 0, 100), 20);

        sensorLeft.clear();
        sensorRight.clear();

        int range = MAX-start;
        float f = 1.0f+(float)diff/range;
        int d = f*250;
        if(d<0)
            d=1;
        Serial.println(d);
        delay(d);
    }

    if (diff > 2) {

        // Volume Down
        consumerCommand(0x20,0x00);
        consumerCommand(0x00,0x00);
        //Serial.println("Volume Down");

        //theaterChase(strip.Color(50, 0, 1), 20);
        //theaterChase(strip.Color(0, 0, 100), 20);

        int range = start-MIN;
        float f = 1.0f-(float)diff/range;

        int d = f*250;
        if(d<0)
            d=1;
        Serial.println(d);
        delay(d);

        sensorLeft.clear();
        sensorRight.clear();
    }
}


void detectHover2(int start, int dist)
{
    int diff = last-dist;

    if(diff != 0)
    Serial.println(diff);

    if (diff < -2) {

        // Volume Up
        consumerCommand(0x10,0x00);
        consumerCommand(0x00,0x00);
        //Serial.println("Volume Up");

        theaterChase(strip.Color(25, 50, 0), 0);
        theaterChase(strip.Color(0, 0, 0), 0);

        sensorLeft.clear();
        sensorRight.clear();
    }

    if (diff > 2) {

        // Volume Down
        consumerCommand(0x20,0x00);
        consumerCommand(0x00,0x00);
        //Serial.println("Volume Down");

        theaterChase(strip.Color(50, 0, 1), 0);
        theaterChase(strip.Color(0, 0, 0), 0);
    }

    delay(100);

    last = dist;
}

int last;
int hoverStart;

void loop()
{
    int distance = sensorLeft.read();
    int distance2 = sensorRight.read();
    int avgDist = (distance + distance2) / 2;

    if (distance != -1 && distance2 != -1) {
        //Serial.println(distance);
        //Serial.println(distance2);
    } else {
        disableSwipeTimer.reset();
    }

    if(disableSwipeTimer.singleShot()) {
        hoverStart = avgDist;
        last = hoverStart;
        Serial.print("Hover Start: ");
        Serial.println(hoverStart);
    }

    if(disableSwipeTimer.check()) {
        detectHover(hoverStart, avgDist);
        detector.reset();
    } else {
        detectSwipe(distance2, distance);

    }
}


void consumerCommand(uint8_t mask0,uint8_t mask1) 
{
    BT.write(0xFD);
    BT.write((byte)0x00);
    BT.write((byte)0x02);
    BT.write(mask0);
    BT.write(mask1);
    BT.write((byte)0x00);
    BT.write((byte)0x00);
    BT.write((byte)0x00);
    BT.write((byte)0x00);
}


void colorWipe(uint32_t c, uint8_t wait) 
{
    for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, c);
        strip.show();
        delay(wait);
    }
}

void reverseColorWipe(uint32_t c, uint8_t wait)
{
    for(int16_t i=(strip.numPixels()-1); i>=0; i--)
    {
        strip.setPixelColor(i, c);
        strip.show();
        delay(wait);
    }
}

//void colorFade(uint8_t wait) {
//  for (uint16_t i=255; i>0; i--) {
//      strip.setPixelColor(0, strip.Color(i,i,i));
//      strip.show();
//      delay(wait);
//  }
//}


void theaterChase(uint32_t c, uint8_t wait) {
    for (int j=0; j<10; j++) {  //do 10 cycles of chasing
        for (int q=0; q < 3; q++) {
            for (int i=0; i < strip.numPixels(); i=i+3) {
                strip.setPixelColor(i+q, c);    //turn every third pixel on
            }
            strip.show();

            delay(wait);

            for (int i=0; i < strip.numPixels(); i=i+3) {
                strip.setPixelColor(i+q, 0);        //turn every third pixel off
            }
        }
    }
}

