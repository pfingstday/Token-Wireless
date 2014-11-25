#include "binary_const.h"
#include "Accelerometer.h"
#include <Wire.h>
#include <Arduino.h>

const byte DEVICE_ID = 0x53;

const int R_DEVID = 0;
const int R_THRESH_TAP = 29;
const int R_OFSX = 30;
const int R_OFSY = 31;
const int R_OFSZ = 32;
const int R_DUR = 33;
const int R_LATENT = 34;
const int R_WINDOW = 35;
const int R_THRESH_ACT = 36;
const int R_THRESH_INACT = 37;
const int R_TIME_INACT = 38;
const int R_ACT_INACT_CTL = 39;
const int R_TAP_AXES = 42;
const int R_ACT_TAP_STATUS = 43;
const int R_BW_RATE = 44;
const int R_POWER_CTL = 45;
const int R_INT_ENABLE = 46;
const int R_INT_MAP = 47;
const int R_INT_SOURCE = 48;
const int R_DATA_FORMAT = 49;
const int R_DATAX0 = 50;
const int R_DATAX1 = 51;
const int R_DATAY0 = 52;
const int R_DATAY1 = 53;
const int R_DATAZ0 = 54;
const int R_DATAZ1 = 55;
const int R_FIFO_CTL = 56;
const int R_FIFO_STATUS = 57;

template < uint8_t interrupt_pin >
void Accelerometer<interrupt_pin>::setup()
{
    Wire.begin();
    pinMode(interrupt_pin, INPUT);

    // resolution
    writeTo(R_DATA_FORMAT, B8(01)); // 4g

    // offset
    writeTo(R_OFSX, -3);
    writeTo(R_OFSY, 0);
    writeTo(R_OFSZ, 0);

    // interrupts setup
    writeTo(R_INT_MAP, 0); // send all interrupts to ADXL345's INT1 pin
    writeTo(R_INT_ENABLE, B8(1111100)); // enable signle and double tap, activity, inactivity and free fall detection

    // single tap configuration
    writeTo(R_DUR, 0x1F); // 625us/LSB
    writeTo(R_THRESH_TAP, 48); // 62.5mg/LSB  <==> 3000mg/62.5mg = 48 LSB as datasheet suggestion
    writeTo(R_TAP_AXES, B8(111)); // enable tap detection on x,y,z axes

    // double tap configuration
    writeTo(R_LATENT, 0x10);
    writeTo(R_WINDOW, 0xFF);

    // inactivity configuration
    writeTo(R_TIME_INACT, 10); // 1s / LSB
    writeTo(R_THRESH_INACT, 3); // 62.5mg / LSB
    // also working good with high movements: R_TIME_INACT=5, R_THRESH_INACT=16, R_ACT_INACT_CTL=B8(00000111)
    // but unusable for a quite slow movements

    // activity configuration
    writeTo(R_THRESH_ACT, 8); // 62.5mg / LSB

    // activity and inctivity control
    writeTo(R_ACT_INACT_CTL, B8(11111111)); // enable activity and inactivity detection on x,y,z using ac

    // set the ADXL345 in measurement and sleep Mode: this will save power while while we will still be able to detect activity
    // set the Link bit to 1 so that the activity and inactivity functions aren't concurrent but alternatively activated
    // set the AUTO_SLEEP bit to 1 so that the device automatically goes to sleep when it detects inactivity
    writeTo(R_POWER_CTL, B8(111100));
}

template < uint8_t interrupt_pin >
byte Accelerometer<interrupt_pin>::readByte(byte address)
{
    Wire.beginTransmission(DEVICE_ID); //start transmission to device
    Wire.send(address);        //sends address to read from
    Wire.endTransmission(); //end transmission

    Wire.beginTransmission(DEVICE_ID); //start transmission to device
    Wire.requestFrom(DEVICE_ID, (byte)1);    // request 1 byte from device

    byte readed = 0;
    if(Wire.available())
    {
        readed = Wire.receive(); // receive a byte
    }
    Wire.endTransmission(); //end transmission
    return readed;
}

template < uint8_t interrupt_pin >
void Accelerometer<interrupt_pin>::readAxis(int16_t &x, int16_t &y, int16_t &z)
{
    byte buff[6];
    readFrom(0x32, sizeof(buff), buff); //read the acceleration data from the ADXL345

    x = (((int)buff[1]) << 8) | buff[0];
    y = (((int)buff[3]) << 8) | buff[2];
    z = (((int)buff[5]) << 8) | buff[4];
}

template < uint8_t interrupt_pin >
byte Accelerometer<interrupt_pin>::update()
{
    if(!digitalRead(interrupt_pin))
        return Acc::NONE;

    byte interruptSource = readByte(R_INT_SOURCE);

    if(interruptSource & Acc::INACTIVITY) {
        // we don't need to put the device in sleep because we set the AUTO_SLEEP bit to 1 in R_POWER_CTL
        // set the LOW_POWER bit to 1 in R_BW_RATE: with this we get worst measurements but we save power
        byte bwRate = readByte(R_BW_RATE);
        writeTo(R_BW_RATE, bwRate | B8(10000));
    }

    if(interruptSource & Acc::ACTIVITY) {
        // get current power mode
        byte powerCTL = readByte(R_POWER_CTL);
        // set the device back in measurement mode
        // as suggested on the datasheet, we put it in standby then in measurement mode
        // we do this using a bitwise and (&) so that we keep the current R_POWER_CTL configuration
        writeTo(R_POWER_CTL, powerCTL & B8(11110011));
        delay(10); // let's give it some time (not sure if this is needed)
        writeTo(R_POWER_CTL, powerCTL & B8(11111011));

        // set the LOW_POWER bit to 0 in R_BW_RATE: get back to full accuracy measurement (we will consume more power)
        byte bwRate = readByte(R_BW_RATE);
        writeTo(R_BW_RATE, bwRate & B8(01111));
    }

    return interruptSource;
}

template < uint8_t interrupt_pin >
void Accelerometer<interrupt_pin>::writeTo(byte address, byte val)
{
    Wire.beginTransmission(DEVICE_ID); //start transmission to device
    Wire.send(address);        // send register address
    Wire.send(val);        // send value to write
    Wire.endTransmission(); //end transmission
}

template < uint8_t interrupt_pin >
void Accelerometer<interrupt_pin>::readFrom(byte address, uint8_t num, byte buff[])
{
    Wire.beginTransmission(DEVICE_ID); //start transmission to device
    Wire.send(address);        //sends address to read from
    Wire.endTransmission(); //end transmission

    Wire.beginTransmission(DEVICE_ID); //start transmission to device
    Wire.requestFrom(DEVICE_ID, num);    // request 6 bytes from device

    int i = 0;
    while(Wire.available())    //device may send less than requested (abnormal)
    {
        buff[i] = Wire.receive(); // receive a byte
        i++;
    }
    Wire.endTransmission(); //end transmission
}

template class Accelerometer<(unsigned char)10>;
