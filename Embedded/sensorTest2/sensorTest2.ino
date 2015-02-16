
#include <Wire.h>
#include <dh_lsm9ds0.h> 
#include "Arduino.h"
#include <dh_logger.h>
#include <dh_arduino_logger.h>
#include <dh_averaged_measure.h> 
#include <dh_movement.h>  
#include <dh_position.h>
#include <quaternion.h>

#include <dh_sensor_processor.h> 
#include "SPI.h"
#include "ILI9341_t3.h"
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
  
 
ILI9341_t3 tft = ILI9341_t3(_cs, _dc, _rst); 
//TouchPad touchPad = TouchPad(13);

LSM9DS0 dof1 = LSM9DS0(); 
 
MovementEval* moveEval =new MovementEval(15,1.2,1.6,500,50);
SensorProcessor* sensorProcessor = new SensorProcessor(200);

volatile uint32_t reads = 0;  
volatile uint32_t time = 0;
uint32_t lastTime = 0;
volatile uint8_t movementAgro = 0;
volatile float shuntvoltage ;
volatile float busvoltage; 
volatile float current_mA;
volatile float loadvoltage;
volatile bool isMoving = false;

volatile uint32_t step1=0,step2=0,step3=0,step4=0,step5=0;

volatile float dpsX,dpsY,dpsZ,gNetX,gNetY,gNetZ,gMoveOffsetX,gMoveOffsetY,gMoveOffsetZ;
volatile int16_t gX,gY,gZ;

Adafruit_INA219 *powerMonitor;

boolean failed = true; 

 
void readGyro(){
  if (digitalRead(DRDYG1)==HIGH){
    reads++;
    time = millis();   // WARNING - might not be able to call millis(/micros() here in an interupt?
      step1++;
    dof1.readRawGyro(); 
     step2++; 
    gX = dof1.gx;
    gY = dof1.gy;
    gZ = dof1.gz;

     sensorProcessor->processGyro(dof1.gx,dof1.gy,dof1.gz,time);
     step3++;
     isMoving = moveEval->getMoving();
    step4++;
  

 
    if (isMoving){
      if (movementAgro<255-10)
        movementAgro+=10;
      else 
        movementAgro=255;
    } 
 


    gNetX = sensorProcessor->netX;
    gNetY = sensorProcessor->netY;
    gNetZ = sensorProcessor->netZ;

    gMoveOffsetX = sensorProcessor->gTrimValue[0];
    gMoveOffsetY = sensorProcessor->gTrimValue[1];
    gMoveOffsetZ = sensorProcessor->gTrimValue[2];

    step5++;

    dpsX = sensorProcessor->dpsX;
    dpsY = sensorProcessor->dpsY;
    dpsZ = sensorProcessor->dpsZ;


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
  Serial1.println("Starting");;


  tft.begin();
  Serial1.println("tft started"); 
 
  tft.setTextSize(1);
   tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
  tft.setCursor(0, 16);  
  tft.print("Starting Gyros: ");
  Serial1.println("Starting Gyros");
dof1.logger = new  ArduinoLogger(1); 

  if (!dof1.initAndVerify(true,true)){
    Serial1.println("Failed LSM9DS0 init1");
    tft.setTextColor(ILI9341_RED,ILI9341_BLACK);
    tft.println("Failed!");
    return;
  }

  Serial1.println("Starting Sensor Filters");


 // moveEval->logger = dof1.logger;

 //  sensorProcessor->logger = dof1.logger;
  sensorProcessor->movement = moveEval;

 
  dof1.setGyroDrBw(LSM9DS0::G_DR_380_BW_20);
  dof1.setGyroScale(LSM9DS0::G_SCALE_500DPS);
  dof1.setGyroPowered(true);
  dof1.setGyroAxisEnabled(true,true,true);
  dof1.setGyroPins(false,false,false,false,true,false,false ,false);
  dof1.updateSettings();
  delay(100);



  tft.setTextColor(ILI9341_GREEN,ILI9341_BLACK);
  tft.println("Started!");
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);

 
  tft.print("Starting Power Monitor: ");
  powerMonitor = new Adafruit_INA219();
  powerMonitor->begin();
  tft.setTextColor(ILI9341_GREEN,ILI9341_BLACK);
  tft.println("Started!");
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);

  delay(500);
  tft.fillScreen(ILI9341_BLACK);
 
  tft.setTextColor(ILI9341_YELLOW); 
  tft.setTextSize(2);
  
  tft.setCursor(0, 0);   
  tft.print("X");
  tft.setCursor(80, 0);  
  tft.print("|Y");
  tft.setCursor(160, 0);
  tft.print("|Z");

  tft.setTextColor(ILI9341_YELLOW); 
  tft.setCursor(0, 300);  
  tft.print("V:       mA:");
  tft.setTextSize(1);

  Serial1.println("Lift off!@!");

  attachInterrupt(2,readGyro, RISING);

  failed = false;
    readGyro();
}

uint8_t frame=0;
void loop(){
  frame++;
  delay(500);
  if (failed)
    return;
//  readGyro();
 
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_GREEN,ILI9341_BLACK);
  tft.setCursor(0, 18);  

  tft.print(gX);
  tft.print("  ");
  tft.setCursor(80, 18);  
  tft.print(gY);
  tft.print("  ");
  tft.setCursor(160, 18);
  tft.print(gZ);
  tft.print("  ");

  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
  tft.setCursor(0, 30);  
  tft.setTextSize(2);
  tft.print(dpsX,3);
  tft.print("  ");
  tft.setCursor(80, 30);  
  tft.print(dpsY,3);
  tft.print("  ");
  tft.setCursor(160, 30);
  tft.print(dpsZ,3); 
  tft.print("  ");

  tft.setTextSize(1);
  tft.setTextColor(isMoving?ILI9341_RED:ILI9341_BLUE,ILI9341_BLACK);
  tft.setCursor(0, 48);  

  tft.print(gNetX,4);
  tft.print("  ");
  tft.setCursor(80, 48);  
  tft.print(gNetY,4);
  tft.print("  ");
  tft.setCursor(160, 48);
  tft.print(gNetZ,4);
  tft.print("  ");

  tft.setCursor(0, 60);  
  tft.print(gMoveOffsetX,3);
  tft.print("  ");
  tft.setCursor(80, 60);  
  tft.print(gMoveOffsetY,3);
  tft.print("  ");
  tft.setCursor(160, 60);
  tft.print(gMoveOffsetZ,3);
  tft.print("  ");
 

  tft.fillRect(220,0,240,20,tft.color565(255,255 - movementAgro, 255 -movementAgro));
  if(movementAgro>20)
    movementAgro -=20;
  else  
    movementAgro = 0; 

 // float dist1 = analogRead(9);  
 // float dist2 = analogRead(8);
 
 // tft.setTextSize(3);
 //  tft.setCursor(0, 200);
 //    tft.setTextColor(ILI9340_WHITE,ILI9340_BLACK);
 //   tft.print(dist1);
 //     tft.print("  ");
 //      tft.setCursor(0, 240);
 //    tft.setTextColor(ILI9340_WHITE,ILI9340_BLACK);
 //   tft.print(dist2);
 //     tft.print("  ");

  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
   tft.setTextSize(2);  
   tft.setCursor(0, 90);
   tft.print(gNetZ);
  // tft.print("  ");
  //   tft.setTextColor(ILI9340_WHITE,ILI9340_BLACK);

  //  tft.setCursor(0, 110);
  //  tft.print(pos.yRot);
  // tft.print("  ");
  //   tft.setTextColor(ILI9340_WHITE,ILI9340_BLACK);
  //  tft.setCursor(0, 130);
  //  tft.print(pos.zRot);
  // tft.print("  ");  
  //   tft.setTextColor(ILI9340_WHITE,ILI9340_BLACK);
  //  tft.setCursor(0, 150);
  //  tft.print(positionState->getHeading()*57.2957795131);
  // tft.print("  ");
  // if (frame == 10){ 
  //   frame = 0;
  //     tft.fillRect(70,130,210,250,ILI9340_BLACK);
  // }
  // double pointer[3];


  //  tft.drawLine(80,180-pointer[2]*50,80,180,ILI9340_YELLOW);

  //  tft.drawLine(140+pointer[0]*50,200-pointer[1]*50,140,200,ILI9340_GREEN);
  
       tft.setTextColor(ILI9341_RED,ILI9341_BLACK);
 tft.setTextSize(2);
 tft.setCursor(25, 305);
   tft.print(loadvoltage);
     tft.print("  "); 
 tft.setCursor(145, 305);
   tft.print(current_mA);
     tft.print("  ");

     if (frame%20 == 0){
      Serial.print(".");
       Serial1.print(step1);
       Serial1.print(",");
       Serial1.print(step2);
       Serial1.print(",");
       Serial1.print(step3);
       Serial1.print(",");
       Serial1.print(step4);
       Serial1.print(",");
       Serial1.print(step5);
       Serial1.println();
        sensorProcessor->printState();
     }
 
}

