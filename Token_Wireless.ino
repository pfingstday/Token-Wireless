#include <SoftwareSerial.h>
SoftwareSerial BT = SoftwareSerial(3,2);

#include <Adafruit_NeoPixel.h>
#define PIN 13
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, PIN, NEO_GRB + NEO_KHZ800);

//Include Metro Lib
#include <Metro.h> 

// Swipe Detector Lib
#include "SwipeDetector.h"
#include "ProximitySensor.h"

// Sharp IR Setup
SharpIRProximitySensor sensorLeft(2);
SharpIRProximitySensor sensorRight(3);
SwipeDetector detector;

// Delay Setup
boolean delayOn = true;
Metro delaySensor = Metro(1000);

boolean resetColor = true;
Metro delayColor = Metro(2000);

boolean MusicOff = false;
boolean MuteOn = false;

boolean CursorModus = false;



void setup(){
  
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  Serial.begin(9600);
  BT.begin(9600); 
}



void loop(){
  
  
  /* Sharp IR */

  int distance = sensorLeft.read();

  int distance2 = sensorRight.read();

  SwipeDetector::Swipe s = detector.detect(distance, distance2);
  
  if (distance != -1 && distance2 != -1) {
    //Serial.println(distance);
    //Serial.println(distance2);
  }


 /* Sensor Delay */  
   
   if (delaySensor.check() == 1) {
     
   if (delayOn == true && distance != -1 && distance < 28 && distance2 < 28){
    delayOn = false;
     //Serial.println("delayOn = false");
    }else{
     delayOn = true;
     //Serial.println("delayOn = true");
    }
 }
   
      
    // Track Control

if (CursorModus == false && delayOn == true) {

   if (s == SwipeDetector::SWIPE_RIGHT) {
   //Serial.println("Swipe: Next");        


//        // PlayPause
//        consumerCommand(0x40,0x00);
//        consumerCommand(0x00,0x00);
//        Serial.println("Play/Pause");
//        
        // Next Track
        consumerCommand(0x80,0x00);
        consumerCommand(0x00,0x00);
        Serial.println("Next Track");
//
//        // Scan Prevoius Track
//        consumerCommand(0x00,0x05);
//        consumerCommand(0x00,0x00);
//        Serial.println("Scan Prevoius Track");

        colorWipe(strip.Color(25, 50, 0), 20); // Green
        colorWipe(strip.Color(0, 0, 0), 20); // Green
        
        delaySensor.reset();
        delay(30);
      }
      
      
    if (s == SwipeDetector::SWIPE_LEFT) {
    //Serial.println("Swipe: Previous");
        

//        // PlayPause
//        consumerCommand(0x40,0x00);
//        consumerCommand(0x00,0x00);
//        Serial.println("Play/Pause");
//        
//        // Next Track
//        consumerCommand(0x80,0x00);
//        consumerCommand(0x00,0x00);
//        Serial.println("Next Track");
//
        // Scan Prevoius Track
        consumerCommand(0x00,0x05);
        consumerCommand(0x00,0x00);
        Serial.println("Previous Track");
        
        reverseColorWipe(strip.Color(50, 0, 1), 20); // Red
        reverseColorWipe(strip.Color(0, 0, 0), 20); // Red
        
        delaySensor.reset();
        delay(30);
      }
   }   
      
      
      
    // Volume Control   
     
    if (CursorModus == false && delayOn == false) {   
       
     if (distance2 != -1 && distance2 >= 9 && distance2 <= 20) {
        
        // Volume Up
        consumerCommand(0x10,0x00);
        consumerCommand(0x00,0x00);
        Serial.println("Volume Up");
        
        //Serial.println("Up Arrow");
        //BT.write(14);
        
  theaterChase(strip.Color(25, 50, 0), 0);
  theaterChase(strip.Color(0, 0, 0), 0);
        
        sensorRight.clear();
        delaySensor.reset();
        delay(200);

        MusicOff = false;
      }

      if (distance2 != -1 && distance2 >= 4 && distance2 <= 7) {
        
        // Volume Down
        consumerCommand(0x20,0x00);
        consumerCommand(0x00,0x00);
        Serial.println("Volume Down");
        
//        Serial.println("Down Arrow");
//        BT.write(12);
        
  theaterChase(strip.Color(50, 0, 1), 0);
  theaterChase(strip.Color(0, 0, 0), 0);

        sensorRight.clear();
        delaySensor.reset();         
        delay(200);
        
        MusicOff = false;
      }  
    }
    
    
      // Cursor Modus  
      

if (CursorModus == true && delayOn == true) {

   if (s == SwipeDetector::SWIPE_RIGHT) {
   Serial.println("Swipe: Next");        

      
   BT.write(6);
   Serial.println("Right Arrow");


        colorWipe(strip.Color(25, 50, 0), 10); // Green
        colorWipe(strip.Color(0, 0, 0), 10); // Green
        
        delaySensor.reset();
        delay(1);
      }
      
      
    if (s == SwipeDetector::SWIPE_LEFT) {
    Serial.println("Swipe: Previous");
        

    Serial.println("Left Arrow");
    BT.write(10);

        
        reverseColorWipe(strip.Color(50, 0, 1), 10); // Red
        reverseColorWipe(strip.Color(0, 0, 0), 10); // Red
        
        delaySensor.reset();
        delay(1);
      }
   }    
     
    if (CursorModus == true && delayOn == false) {   
       
     if (distance2 != -1 && distance2 >= 9 && distance2 <= 20) {
        
        BT.write(14);
        Serial.println("Up Arrow");

        
  theaterChase(strip.Color(25, 50, 0), 1);
  theaterChase(strip.Color(0, 0, 0), 1);
        
        sensorRight.clear();
        delaySensor.reset();
        delay(10);

        MusicOff = false;
      }

      if (distance2 != -1 && distance2 >= 4 && distance2 <= 7) {
        
        BT.write(12);
        Serial.println("Down Arrow");
        
  theaterChase(strip.Color(50, 0, 1), 1);
  theaterChase(strip.Color(0, 0, 0), 1);

        sensorRight.clear();
        delaySensor.reset();         
        delay(10);
        
        MusicOff = false;
      }  
    }
    
//       // Mute & Play     
//   
//    if (distance2 != -1 && distance2 <= 3 && delayOn == false) {
//
//      
//      // PlayPause
//      consumerCommand(0x40,0x00);
//      consumerCommand(0x00,0x00);
//      Serial.println("Play/Pause");
//        
//      sensorRight.clear();
//
//      MusicOff = true;
//      MuteOn = true;
//      
//      delay(300);
//      delaySensor.reset();
//        
//    }
//
//    if (MuteOn == true && delayOn == true) {
//      
//      if (s == SwipeDetector::SWIPE_LEFT || s == SwipeDetector::SWIPE_RIGHT) {
//        
//        // PlayPause
//        consumerCommand(0x40,0x00);
//        consumerCommand(0x00,0x00);
//        Serial.println("Play/Pause");
//        
//        sensorRight.clear();
//
//        MusicOff = false;
//        MuteOn = false;
//      }
//    }
  
  
      
  }
  
  
void consumerCommand(uint8_t mask0,uint8_t mask1) {
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
  

void colorWipe(uint32_t c, uint8_t wait) {
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

