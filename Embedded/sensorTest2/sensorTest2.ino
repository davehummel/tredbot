#define DEBUG


#include <Wire.h>
#include <dh_lsm9ds0.h>
#include "Arduino.h"
#include <dh_logger.h>
#include <dh_arduino_logger.h>
#include <dh_trimfilter.h>
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9340.h"
#include <Adafruit_INA219.h>

#if defined(__SAM3X8E__)
    #undef __FlashStringHelper::F(string_literal)
    #define F(string_literal) string_literal
#endif


// These are the pins used for the UNO
// for Due/Mega/Leonardo use the hardware SPI pins (which are different)
#define _sclk 13
#define _miso 12
#define _mosi 11
#define _cs 10
#define _dc 9
#define _rst 8

///////////////////////////////
// Interrupt Pin Definitions //
///////////////////////////////
const byte INT1XM = 3; // INT1XM tells us when accel data is ready
const byte INT2XM = 4; // INT2XM tells us when mag data is ready
const byte DRDYG1  = 2; // DRDYG  tells us when gyro data is ready


Adafruit_ILI9340 tft = Adafruit_ILI9340(_cs, _dc, _rst);
//TouchPad touchPad = TouchPad(13);

LSM9DS0 dof1 = LSM9DS0();

TrimFilter* gyroTrim1 =new TrimFilter(67,-15,95);

volatile uint32_t reads = 0;
volatile  float shuntvoltage ;
volatile   float busvoltage;
volatile  float current_mA ;
volatile  float loadvoltage;

Adafruit_INA219 *powerMonitor;


void readGyro(){
  if (digitalRead(DRDYG1)==HIGH){
    reads++;
    dof1.readRawGyro();
    gyroTrim1->addStoppedGyroData(dof1.gx,dof1.gy,dof1.gz);
    gyroTrim1->filterGyro(dof1.gx,dof1.gy,dof1.gz);
    if (reads%100==0){
      shuntvoltage = powerMonitor->getShuntVoltage_mV();
      busvoltage = powerMonitor->getBusVoltage_V();
      current_mA =powerMonitor->getCurrent_mA();
      loadvoltage = busvoltage + (shuntvoltage / 1000);
    }
  } 
}


void setup(){

  
  pinMode(2, INPUT_PULLUP);
  Serial1.begin(115200);
  Serial1.println("Starting");

  tft.begin();
  Serial1.println("tft started");
 
  tft.setTextSize(1);
   tft.setTextColor(ILI9340_WHITE,ILI9340_BLACK);
  tft.setCursor(0, 16);  
  tft.print("Starting Gyros: ");
  dof1.logger = new  ArduinoLogger(0);
  
  if (!dof1.initAndVerify(true,true)){
    Serial1.println("Failed LSM9DS0 init1");
    tft.setTextColor(ILI9340_RED,ILI9340_BLACK);
    tft.println("Failed!");
    return;
  }
 

  dof1.setGyroPowered(true);
  dof1.setGyroAxisEnabled(true,true,true);
  dof1.setPins(false,false,false,false,true,false,false,false);
  dof1.updateSettings();
  delay(100);
 


  tft.setTextColor(ILI9340_GREEN,ILI9340_BLACK);
  tft.println("Started!");
  tft.setTextColor(ILI9340_WHITE,ILI9340_BLACK);

 
  tft.print("Starting Power Monitor: ");
  powerMonitor = new Adafruit_INA219();
  powerMonitor->begin();
  tft.setTextColor(ILI9340_GREEN,ILI9340_BLACK);
  tft.println("Started!");
  tft.setTextColor(ILI9340_WHITE,ILI9340_BLACK);






  delay(500);
  tft.fillScreen(ILI9340_BLACK);

  tft.setTextColor(ILI9340_RED); 
  tft.setTextSize(2);

  tft.setCursor(0, 0);  
  tft.print("X");
  tft.setCursor(80, 0);  
  tft.print("|Y");
  tft.setCursor(160, 0);
  tft.print("|Z");

  tft.setTextColor(ILI9340_YELLOW); 
  tft.setCursor(0, 300);  
  tft.print("V:       mA:");
  tft.setTextSize(1);
  attachInterrupt(2,readGyro, RISING);

}


void loop(){
  
  delay(500);
  readGyro();
  Serial1.print("reads:");
  Serial1.println(reads);
   tft.setTextSize(1);
  tft.setTextColor(ILI9340_BLUE,ILI9340_BLACK);
 tft.setCursor(0, 16);  
 dof1.convertRawGyro();
 tft.print(dof1.dpsX,4);
 tft.print("  ");
 tft.setCursor(80, 16);  
 tft.print(dof1.dpsY,4);
   tft.print("  ");
 tft.setCursor(160, 16);
 tft.print(dof1.dpsZ,4);
   tft.print("  ");

 float dist1 = analogRead(9);
 float dist2 = analogRead(8);

 tft.setTextSize(3);
  tft.setCursor(0, 200);
    tft.setTextColor(ILI9340_WHITE,ILI9340_BLACK);
   tft.print(dist1);
     tft.print("  ");
      tft.setCursor(0, 240);
    tft.setTextColor(ILI9340_WHITE,ILI9340_BLACK);
   tft.print(dist2);
     tft.print("  ");
       tft.setTextColor(ILI9340_RED,ILI9340_BLACK);
 tft.setTextSize(2);
 tft.setCursor(25, 305);
   tft.print(loadvoltage);
     tft.print("  ");
 tft.setCursor(145, 305);
   tft.print(current_mA);
     tft.print("  ");
     Serial1.println(current_mA);
}

