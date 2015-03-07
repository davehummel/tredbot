
#include <i2c_t3.h>
#include "SPI.h"
#include "Arduino.h"

#include <dh_mpr121.h>

#include "ILI9341_t3.h" 


// #if defined(__SAM3X8E__)
//     #undef __FlashStringHelper::F(string_literal)
//     #define F(string_literal) string_literal
// #endif 
  
// These are the pins used for the UNO
// for Due/Mega/Leonardo use the hardware SPI pins (which are different)
#define _sclk 13 
#define _miso 12 
#define _mosi 11
#define _cs 10 
#define _dc 9 
#define _rst 5
#define _bl 4

//The Arduino Wire library uses the 7-bit version of the address, so the code example uses 0x70 instead of the 8‑bit 0xE0
#define SensorAddress byte(0x70)
//The Sensor ranging command has a value of 0x51
#define RangeCommand byte(0x51)
//These are the two commands that need to be sent in sequence to change the sensor address
#define ChangeAddressCommand1 byte(0xAA)
#define ChangeAddressCommand2 byte(0xA5)
    
/////////////////////////////// 
// Interrupt Pin Definitions //
///////////////////////////////
const byte IN0 = A0; // Anlog in 0
const byte IN1 = A1; // Anlog in 1
const byte IN2 = A2; // Anlog in 2
const byte IN3 = A3; // Anlog in 3
 
const byte TOUCHIRQ = 6;

ILI9341_t3 tft = ILI9341_t3(_cs, _dc, _rst); 
TouchPad touchPad = TouchPad(TOUCHIRQ);

 elapsedMillis timeMS;
 elapsedMicros timeMC;


 uint16_t d0=0,d1=0,d2=0,d3=0,d4=0;
 
uint8_t vals [5][320] ;
uint8_t oldVals [5][320] ;

 uint16_t readCount = 0;

 uint16_t ANFACTOR = 4096/200;

 uint8_t inMap=1;

 uint32_t sampleInterval = 500;


 uint16_t millisPerFrame = 500;

 char padInput[13];
 uint8_t inputKeyCount = 0;
 bool enteringMSPerFrame= false;
 bool enteringSampleInt = false;


volatile bool checkTouchPad=true,checkScreen=false;
bool partialDistCycle = true;

void updateDisplay(){

//  tft.fillScreen(ILI9341_WHITE);

  checkScreen = false;

  partialDistCycle = true;
  
  tft.setTextSize(2);
  if ((inMap & 1) == 1 ){
    tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
    tft.setCursor(0, 0);
    tft.print(d0);
    tft.print("      ");
  }
  if ((inMap & 2)  == 2){
      tft.setTextColor(ILI9341_RED,ILI9341_WHITE);
      tft.setCursor(80, 0);
      tft.print(d1);
      tft.print("      ");
  }
  if ((inMap & 4) == 4){
      tft.setTextColor(ILI9341_GREEN,ILI9341_WHITE);
      tft.setCursor(160, 0);
      tft.print(d2);
      tft.print("      ");
  }
  if ((inMap & 8) == 8){
    tft.setTextColor(ILI9341_BLUE,ILI9341_WHITE);
    tft.setCursor(240, 0);
    tft.print(d3);
    tft.print("      ");
  } else if ((inMap & 16) == 16){
    tft.setTextColor(ILI9341_MAGENTA,ILI9341_WHITE);
    tft.setCursor(240, 0);
    tft.print(d4);
    tft.print("      ");
  }
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);
  tft.setCursor(0, 24);

  tft.print("Refresh:");
  tft.print(millisPerFrame);
  tft.print("      ");

  tft.setCursor(85, 24);

  tft.print("Inputs:");
  if ((inMap & 1) == 1){
    tft.print("1 ");
  }else{
    tft.print("  ");
  }
  if ((inMap & 2) == 2){
    tft.print("2 ");
  }else{
    tft.print("  ");
  }

  if ((inMap & 4) == 4){
    tft.print("3 ");
  }else{
    tft.print("  ");
  }

  if ((inMap & 8) == 8){
    tft.print("4 ");
  }else{
    tft.print("  ");
  }

  if ((inMap & 16) == 16){
    tft.print("5 ");
  }else{
    tft.print("  ");
  }



  tft.setCursor(210, 24);

  tft.print("Smp Int:");
  tft.print(sampleInterval);
  tft.print("      ");
 

  drawData(0,320);

   if (readCount == 320)
    readCount = 0;

}
 
void drawData (uint16_t from, uint16_t len){

  for (int i = 0 ; i < len-1 ; i ++){
    if ((inMap & 1) == 1){
        tft.drawLine(from+i,239-oldVals[0][i],from+i+1,239-oldVals[0][i+1],ILI9341_WHITE);
    }
    if  ((inMap & 2) == 2){
      tft.drawLine(from+i,239-oldVals[1][i],from+i+1,239-oldVals[1][i+1],ILI9341_WHITE);
    }  
    if  ((inMap & 4) == 4){
       tft.drawLine(from+i,239-oldVals[2][i],from+i+1,239-oldVals[2][i+1],ILI9341_WHITE);
    }  
    if  ((inMap & 8) == 8){
       tft.drawLine(from+i,239-oldVals[3][i],from+i+1,239-oldVals[3][i+1],ILI9341_WHITE);
    }
    if  ((inMap & 16) == 16){
       tft.drawLine(from+i,239-oldVals[4][i],from+i+1,239-oldVals[4][i+1],ILI9341_WHITE);
    }

    if  ((inMap & 1) == 1){
      tft.drawLine(from+i,239-vals[0][i],from+i+1,239-vals[0][i+1],ILI9341_BLACK);
      oldVals[0][i] = vals[0][i];
    }
    if  ((inMap & 2) == 2){
      tft.drawLine(from+i,239-vals[1][i],from+i+1,239-vals[1][i+1],ILI9341_RED);
      oldVals[1][i] = vals[1][i];
    }
    if  ((inMap & 4) == 4){
       tft.drawLine(from+i,239-vals[2][i],from+i+1,239-vals[2][i+1],ILI9341_GREEN);
       oldVals[2][i] = vals[2][i];
    }
    if ((inMap & 8) == 8){
       tft.drawLine(from+i,239-vals[3][i],from+i+1,239-vals[3][i+1],ILI9341_BLUE);
       oldVals[3][i] = vals[3][i];
    }
    if  ((inMap & 16) == 16){
       tft.drawLine(from+i,239-vals[4][i],from+i+1,239-vals[4][i+1],ILI9341_MAGENTA);
       oldVals[4][i] = vals[4][i];
    }
  }
}
 
void setup(){ 

  pinMode(20,OUTPUT);

  Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100);
  analogReadAveraging(1);
  analogReadRes(12);
  analogReference(0);   

  Serial1.begin(115200); 

  Serial1.println("Starting...");;
 
  tft.begin(); 
  Serial1.println("tft started"); 
  tft.setRotation(3);
  tft.fillScreen(ILI9341_WHITE);
  touchPad.begin(touchPress);
   takeRangeReading();
} 

void touchPress(){
  checkTouchPad = true;
}



void onRead(){

  if  ((inMap & 1) == 1){
      vals[0][readCount] = (d0 = analogRead(IN0)) / ANFACTOR;
  }
  if  ((inMap & 2) == 2){
    vals[1][readCount] = (d1 = analogRead(IN1)) / ANFACTOR;
  }
  if  ((inMap & 4) == 4){
    vals[2][readCount] = (d2 = analogRead(IN2)) / ANFACTOR;
  }
  if  ((inMap & 8) == 8){
    vals[3][readCount] = (d3 = analogRead(IN3)) / ANFACTOR;
  }
  if  ((inMap & 16) == 16){
    vals[4][readCount] = (d4 = onOtherSensorRead()) / 2;
  }  
  readCount++;

}

//Commands the sensor to take a range reading
void takeRangeReading(){
       Wire.beginTransmission(SensorAddress);             //Start addressing 
       Wire.write(RangeCommand);                             //send range command 
       Wire.endTransmission();                                  //Stop and do something else now
}    

//Returns the last range that the sensor determined in its last ranging cycle in centimeters. Returns 0 if there is no communication. 
uint16_t requestRange(){ 
    Wire.requestFrom(SensorAddress, byte(2));
          if(Wire.available() >= 2){                            //Sensor responded with the two bytes 
           byte HighByte = Wire.read();                        //Read the high byte back 
           byte LowByte = Wire.read();                        //Read the low byte back 
           uint16_t range = word(HighByte, LowByte);         //Make a 16-bit word out of the two bytes for the range 
           return range; 
        }
        else { 
        return uint16_t(0);                                             //Else nothing was received, return 0 
    }
}

uint16_t onOtherSensorRead(){
      int16_t val = requestRange();
      if (val == 0){
        val = d4;
      } else {
        takeRangeReading();
      }
    return val;
}

void onTouch(uint8_t num){
   if (num == 0 || num == 8){
      tft.fillScreen(ILI9341_BLUE);
      tft.setTextSize(2);
      tft.setTextColor(ILI9341_BLUE,ILI9341_WHITE);
      tft.setCursor(0, 100);

      if (num == 0 ){
        enteringSampleInt = true;
        tft.print("Microseconds per sample:");
      }else{
        enteringMSPerFrame = true;
        tft.print("Milliseconds per screen:");
      }
      inputKeyCount = 0;

   } else if (enteringMSPerFrame || enteringSampleInt){
      if (inputKeyCount >= 12)
          return;

      char key = touchPad.numToKey(num);
      padInput[inputKeyCount] = key;
      inputKeyCount++;
      padInput[inputKeyCount] = '\0';

       tft.setCursor(10, 124);
      tft.print(padInput);

   } else {
    Serial1.print("Before ");
    Serial1.print(inMap,BIN);
        if (num == 3) {
          inMap = inMap ^ 1 ;
        } else if (num == 7) {
             inMap = inMap ^ 2 ;
        }
        else if (num == 11) {
            inMap = inMap ^ 4 ;
        }
        else if (num == 2) {
            inMap = inMap ^ 8 ;
        }
        else if (num == 6) {
            inMap = inMap ^ 16;
        }

            Serial1.print(" After ");
    Serial1.println(inMap,BIN);

        tft.fillScreen(ILI9341_WHITE);
   }
 

}

void onRelease(uint8_t num){
  if (num == 0 || num == 8){
      if (inputKeyCount > 0){
        if (enteringSampleInt){
          sampleInterval = atoi(padInput);
          enteringSampleInt = false;
        }else if (enteringMSPerFrame){
          millisPerFrame = atoi(padInput);
          enteringMSPerFrame = false;
        }
        inputKeyCount = 0;
      }else{
        enteringSampleInt = false;
        enteringMSPerFrame = false;
      }


        tft.fillScreen(ILI9341_WHITE);
  }

}

uint32_t lastTime = 0, lastMicros = 0;;
uint32_t msFlipTime;
bool flipflop = true;
void loop(){
  if (timeMS - msFlipTime>500){
    digitalWrite(20, flipflop?HIGH:LOW);
    flipflop = !flipflop;
    msFlipTime = timeMS;
  }

  if (timeMS - lastTime > millisPerFrame){
    lastTime = timeMS;
    checkScreen=true;
  }

  if (readCount<320 && timeMC - lastMicros> sampleInterval){
    timeMC = lastMicros;
    onRead();
  }

  if (!enteringSampleInt && !enteringMSPerFrame && checkScreen){
    updateDisplay();

  }

  if (checkTouchPad){
    checkTouchPad = false;
    touchPad.process(onTouch,onRelease);
  }
 
}

