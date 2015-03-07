
#include <i2c_t3.h>
#include "SPI.h"
#include "Arduino.h"

#include <dh_mpr121.h>
#include <Adafruit_INA219.h>
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

 
const byte TOUCHIRQ = 6;

ILI9341_t3 tft = ILI9341_t3(_cs, _dc, _rst); 
TouchPad touchPad = TouchPad(TOUCHIRQ);
 elapsedMillis timeMS;
 elapsedMicros timeMC;


 char padInput[13];
 uint8_t inputKeyCount = 0;


 Adafruit_INA219 *powerMonitor;

 float shuntvoltage ;
 float busvoltage; 
 float current_mA;
 float loadvoltage;



volatile bool checkTouchPad=true,checkScreen=false;
void setup(){ 

  Serial1.begin(460800); 

  Serial1.println("Starting...");

  Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100);

  tft.begin(); 

  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLUE);
  touchPad.begin(touchPress);

  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLUE);
  tft.setCursor(0, 0);


  Serial3.begin(115200); 


  Serial1.println("Serial 3 Ready.");

  powerMonitor = new Adafruit_INA219();
  powerMonitor->begin();

  Serial1.println("Power Monitor Ready.");  


    tft.print("GO:");
}

void touchPress(){
  checkTouchPad = true;
}


void onTouch(uint8_t num){
  Serial1.println(num);
  tft.print(num,HEX);
}

void onRelease(uint8_t num){
 
}



uint32_t lastTime = 0;
uint8_t convertHex(uint8_t in){
if (in == ' ')
  return 255;

  if (in <= '9' )
    return in - '0';
  if (in <= 'F')
    return in - 'A'+10;
  if (in <= 'f')
    return in - 'a'+10;

  return 0;
}

void prepScreen(){
  if (tft.getCursorY()>=30){
      tft.fillScreen(ILI9341_BLUE);
       tft.setCursor(0, 0);
  }

}

void readPower(){
      shuntvoltage = powerMonitor->getShuntVoltage_mV();
      busvoltage = powerMonitor->getBusVoltage_V();
      current_mA =powerMonitor->getCurrent_mA();
      loadvoltage = busvoltage + (shuntvoltage / 1000);
}


void loop(){

  if (Serial1.available()>1){
    prepScreen();
    tft.print(" >");
    uint8_t inByte1 = convertHex(Serial1.read());
   
    tft.print(inByte1,HEX);
    uint8_t inByte2 =  convertHex(Serial1.read());
    tft.print(inByte2,HEX);
    if (inByte1 == 255|| inByte2 == 255){
      readPower();
      Serial1.print("Power : ");
      Serial1.print(loadvoltage);
      Serial1.print("v ");
      Serial1.print(current_mA);
      Serial1.println("ma");
      return;
    }
    Serial3.write(inByte1*16+inByte2);
    while(true){
      if (Serial1.available()>1){
          uint8_t inByte1 = convertHex(Serial1.read());
          if (inByte1 == 255){
            tft.print(":");
            Serial1.read();
            break;
          }
          tft.print(inByte1,HEX);
          uint8_t inByte2 = convertHex(Serial1.read());
          if (inByte2 == 255){
            Serial3.write(inByte1);
            tft.print(":");
            break;
          }
          tft.print(inByte2,HEX);
          Serial3.write(inByte1*16+inByte2);
      }
    }

  }

  if (Serial3.available()){
    prepScreen();
    tft.print(" <");
    while (Serial3.available()){
      uint8_t inByte1 = Serial3.read();
      Serial1.print(inByte1); 
      tft.print(inByte1);
    }
    Serial1.println();
  }
 

  if (checkTouchPad){
    checkTouchPad = false;
    touchPad.process(onTouch,onRelease);
  }
 
}

