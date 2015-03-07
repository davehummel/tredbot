
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
 
MovementEval* moveEval =new MovementEval(18,1.4,2,500,50);
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

volatile uint32_t readM=0,readA=0,readG=0,readD=0;

volatile float dpsX,dpsY,dpsZ,gNetX,gNetY,gNetZ,gMoveOffsetX,gMoveOffsetY,gMoveOffsetZ;
volatile int16_t gX,gY,gZ,aX,aY,aZ,mX,mY,mZ;

Adafruit_INA219 *powerMonitor;

bool hasDOF = false;

// Create an IntervalTimer object 
IntervalTimer distanceTimer;

volatile uint8_t frame=0;

 
void readGyro(){
  frame = 0;
  time = millis();   // WARNING - might not be able to call millis(/micros() here in an interupt?
    reads++;

   if (digitalRead(INT2XM)==HIGH){
     readM++;
     dof1.readRawMag();
     mX = dof1.mx; 
     mY = dof1.my;
     mZ = dof1.mz;
   }

  if (digitalRead(INT1XM)==HIGH){
    readA++;
  //  dof1.readRawAccel();

    aX = dof1.ax;
    aY = dof1.ay;
    aZ = dof1.az;
  }

  if (digitalRead(DRDYG1)==HIGH){

    readG++;

  //  dof1.readRawGyro(); 

    gX = dof1.gx;
    gY = dof1.gy;
    gZ = dof1.gz;

     sensorProcessor->processGyro(dof1.gx,dof1.gy,dof1.gz,time);
     isMoving = moveEval->getMoving();
  

 
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

    dpsX = sensorProcessor->dpsX;
    dpsY = sensorProcessor->dpsY;
    dpsZ = sensorProcessor->dpsZ;


  
  }  
} 

void readDistance(void){
  readD++;

}
 
void setup(){ 

  pinMode(DRDYG1, INPUT_PULLUP);
  pinMode(INT1XM, INPUT_PULLUP);
  pinMode(INT2XM, INPUT_PULLUP);
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
    hasDOF = false;
    Serial1.println("Failed LSM9DS0 init1");
    tft.setTextColor(ILI9341_RED,ILI9341_BLACK); 
    tft.println("Failed!");
  } else{ 
   hasDOF = true;
  Serial1.println("Starting Sensor Filters");


 // moveEval->logger = dof1.logger;
 
 //  sensorProcessor->logger = dof1.logger;
  sensorProcessor->movement = moveEval;

 
  dof1.setGyroDrBw(LSM9DS0::G_DR_380_BW_20);
  dof1.setGyroScale(LSM9DS0::G_SCALE_500DPS);
  dof1.setGyroPowered(true);
  dof1.setGyroAxisEnabled(true,true,true);
  dof1.setGyroPins(false,false,false,false,true,false,false ,false);
  dof1.setAccelAxisEnabled(true,true,true);
  dof1.setAccelRate(LSM9DS0::A_ODR_400);
  dof1.setMagRate(LSM9DS0::M_ODR_100);
  dof1.setAccelAAFilterBW(LSM9DS0::A_ABW_362);
 // dof1.setTemperatureEnabled(true);
  dof1.setMagSensorMode(LSM9DS0::M_S_CONT);
  dof1.setMagXMPins(false,false,false,false,false,false,false,true);
  dof1.setXMIntPin1(false,false,false,false,false,true,false,false);
  dof1.setXMIntPin2(false,false,false,false,false,true,false,false);
  dof1.updateSettings(); 
  delay(100);



  tft.setTextColor(ILI9341_GREEN,ILI9341_BLACK);
  tft.println("Started!");
}
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


if (hasDOF){

  Serial1.println("Attching DOF interrupts");
    attachInterrupt(DRDYG1,readGyro, RISING);
    attachInterrupt(INT1XM,readGyro, RISING);
   attachInterrupt(INT2XM,readGyro, RISING);

       delay(500);

 }
  Serial1.println("start Timer");


    distanceTimer.begin(readDistance,100000);

  Serial1.println("Lift off!@!");
}


void loop(){
  Serial1.println("in loop");
  if (hasDOF){
      Serial1.println("with hasDOF");
    if (frame >=2){
      Serial1.println("Forced Read!");
      readGyro();
    }
  frame++;
  }
  delay(1000);

      shuntvoltage = powerMonitor->getShuntVoltage_mV();
      busvoltage = powerMonitor->getBusVoltage_V();
      current_mA =powerMonitor->getCurrent_mA();
      loadvoltage = busvoltage + (shuntvoltage / 1000);
 
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

   tft.setTextSize(1);
  tft.setTextColor(ILI9341_GREEN,ILI9341_BLACK);
  tft.setCursor(0, 90);  

  tft.print(aX);
  tft.print("  ");
  tft.setCursor(80, 90);  
  tft.print(aY);
  tft.print("  ");
  tft.setCursor(160, 90);
  tft.print(aZ);
  tft.print("  ");

     tft.setTextSize(1);
  tft.setTextColor(ILI9341_GREEN,ILI9341_BLACK);
  tft.setCursor(0,110);  

  tft.print(mX);
  tft.print("  ");
  tft.setCursor(80,110);  
  tft.print(mY);
  tft.print("  ");
  tft.setCursor(160,110);
  tft.print(mZ);
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
   tft.setCursor(0, 70);
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

     //if (frame%20 == 0){
      Serial1.print("Reads:");
       Serial1.print(reads);
       Serial1.print(" G:");
       Serial1.print(readG);
       Serial1.print(" M:");
       Serial1.print(readM);
       Serial1.print(" A:");
       Serial1.print(readA);
       Serial1.print(" D:");
       Serial1.print(readD);
       Serial1.println();

    
    //    sensorProcessor->printState();
  //   }
 
}

