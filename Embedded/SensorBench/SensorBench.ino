

#include "SPI.h"
#include "Arduino.h"

#include <dh_lsm9ds0.h> 
#include <dh_logger.h>
#include <dh_arduino_logger.h>
#include <dh_averaged_measure.h> 
#include <dh_movement.h>  
#include <dh_position.h>
#include <quaternion.h>
#include <dh_mpr121.h>
#include <dh_sensor_processor.h> 

#include "ILI9341_t3.h"
#include <Adafruit_INA219.h>

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
#define _touch 6

#define PINK 0xFB56
#define ORANGE 0xFA20

///////////////////////////////
// Interrupt Pin Definitions //
///////////////////////////////
const byte INT1XM = 3; // INT1XM tells us when accel data is ready
const byte INT2XM = 4; // INT2XM tells us when mag data is ready
const byte DRDYG1 = 2; // DRDYG  tells us when gyro data is ready
const byte RANGEIN  = A14;
const byte ENRANGE1 = 23;
const byte ENRANGE2 = 22;
const byte ENRANGE3 = 21;
const byte ENRANGE4 = 20;


ILI9341_t3 tft = ILI9341_t3(_cs, _dc, _rst); 
TouchPad touchPad = TouchPad(_touch);

LSM9DS0 dof1 = LSM9DS0(); 
///Adafruit_ADS1115 ads1115;
 
MovementEval* moveEval =new MovementEval(18,1.4,2,500,50);
SensorProcessor* sensorProcessor = new SensorProcessor(200);
uint8_t frame = 0;
 uint32_t reads = 0;  

 uint8_t movementAgro = 0;
 float shuntvoltage ;
 float busvoltage; 
 float current_mA;
 float loadvoltage;
 bool isMoving = false;

 elapsedMillis time;

 uint32_t readM=0,readA=0,readG=0,readD=0;

 float dpsX,dpsY,dpsZ,gNetX,gNetY,gNetZ,gMoveOffsetX,gMoveOffsetY,gMoveOffsetZ;
 int16_t gX,gY,gZ,aX,aY,aZ,mX,mY,mZ;
 double d1=0,d2=0,d3=0,d4=0,ed1=0,ed2=0,ed3=0,ed4=0;
 uint16_t dsamples=0;

 double vals [4][240] ;
 double distPoints [4][12];

 double pointCalc[4];
 uint16_t pointSamples[4];
 uint8_t activePoint=12;

Adafruit_INA219 *powerMonitor;

bool hasDOF = false;

volatile bool checkTouchPad,checkG,checkA,checkM,checkD1,checkD2,checkD3,checkD4,checkD1B,checkD2B,checkD3B,checkD4B,checkScreen;
bool partialDistCycle = true;

// Create an IntervalTimer object 
IntervalTimer distanceTimer;

void interruptDOF(){
   if (digitalRead(INT2XM)==HIGH){
    checkM = true;
   }

  if (digitalRead(INT1XM)==HIGH){
    checkA = true;
  }

  if (digitalRead(DRDYG1)==HIGH){
    checkG = true;
  }  
} 
 
void readGyro(){
    readG++;
    checkG = false;
    dof1.readRawGyro(); 

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

void readAccel(){
    readA++;
    checkA = false;
    dof1.readRawAccel();

    aX = dof1.ax;
    aY = dof1.ay;
    aZ = dof1.az;
}

void readMag(){
     readM++;
     checkM = false;
     dof1.readRawMag();
     mX = dof1.mx; 
     mY = dof1.my;
     mZ = dof1.mz;
}

double doAnalogRead(){
  return  ((double)analogRead(RANGEIN))/300;
}

void readDistance(){
  if (!partialDistCycle){
      if (checkD1B){
        checkD1B = false;
        d1 = doAnalogRead();
        vals[0][readD%240]=d1;
        if (activePoint<12){
          pointCalc[0]+=d1;
          pointSamples[0]++;
        }
        digitalWrite(ENRANGE1, LOW);
      }
      if (checkD2B){
        checkD2B = false;
        d2 = doAnalogRead();
        vals[1][readD%240]=d2;
        if (activePoint<12){
          pointCalc[1]+=d2;
          pointSamples[1]++;
        }
        digitalWrite(ENRANGE2, LOW);
      }
      if (checkD3B){
        checkD3B = false;
        d3 = doAnalogRead();
        vals[2][readD%240]=d3;
        if (activePoint<12){
          pointCalc[2]+=d3;
          pointSamples[2]++;
        }
        digitalWrite(ENRANGE3, LOW);
      }
      if (checkD4B){
        checkD4B = false;
        d4 = doAnalogRead();
        vals[3][readD%240]=d4;
        if (activePoint<12){
          pointCalc[3]+=d4;
          pointSamples[3]++;
        }
        digitalWrite(ENRANGE4, LOW);
      }
  } else {
    partialDistCycle = false;
    checkD1B = checkD2B = checkD3B = checkD4B = false;
  }

  if (checkD1){
    readD++;
    checkD1 = false;
    digitalWrite(ENRANGE1, HIGH);
  }
  if (checkD2){
    checkD2 = false;
    digitalWrite(ENRANGE2, HIGH);
  }

  if (checkD3){
    checkD3 = false;
    digitalWrite(ENRANGE3, HIGH);
  }

  if (checkD4){
    checkD4 = false;
    digitalWrite(ENRANGE4, HIGH);
  }



   

}

void updateDisplay(){

    checkScreen = false;

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
 


  tft.setTextColor(PINK,ILI9341_BLACK);
  tft.setCursor(0, 120);
  tft.print(d1);
  tft.print("  ");
  tft.setTextColor(ILI9341_YELLOW,ILI9341_BLACK);
  tft.setCursor(60, 120);
  tft.print(d2);
  tft.print("  ");
  tft.setTextColor(ILI9341_BLUE,ILI9341_BLACK);
  tft.setCursor(120, 120);
  tft.print(d3);
  tft.print("  ");
  tft.setTextColor(ORANGE,ILI9341_BLACK);
  tft.setCursor(180, 120);
  tft.print(d4);
  tft.print("  ");

  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
   tft.setTextSize(2);  
   tft.setCursor(0, 70);
   tft.print(gNetZ);
 
 if (activePoint==12){
    tft.fillRect(0,128,240,300,ILI9341_BLACK);

    for (int i = 0; i <240 ; i++){
      tft.drawPixel(i, 300-vals[0][i],  PINK);
      tft.drawPixel(i, 300-vals[1][i],  ILI9341_YELLOW);
      tft.drawPixel(i, 300-vals[2][i],  ILI9341_BLUE);
      tft.drawPixel(i, 300-vals[3][i],  ORANGE);
    }

    for (int i = 0 ; i < 12 ; i ++){
      Serial1.print(i);
      Serial1.print(" : ");
      for (int j = 0 ; j < 4 ; j++){
        Serial1.print(distPoints[j][i]);
        Serial1.print(",");
      }
      Serial1.println();
    }

  }


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

 



  //    //if (frame%20 == 0){
  //     Serial1.print("Reads:");
  //      Serial1.print(reads);
  //      Serial1.print(" G:");
  //      Serial1.print(readG);
  //      Serial1.print(" M:");
  //      Serial1.print(readM);
  //      Serial1.print(" A:");
  //      Serial1.print(readA);
  //      Serial1.print(" D:");
  //      Serial1.print(readD);
  //      Serial1.println();

  //     Serial1.print("E:");
  // Serial1.println(time);
  frame = 0;
  partialDistCycle = true;
    //    sensorProcessor->printState();
  //   }
}
 
 
void setup(){ 
  // for (int i = 0; i <240 ; i++){
  //   vals[0][i] = 0;
  // }

 
  pinMode(ENRANGE1,OUTPUT);
  digitalWrite(ENRANGE1, LOW);
    pinMode(ENRANGE2,OUTPUT);
  digitalWrite(ENRANGE2, LOW);
    pinMode(ENRANGE3,OUTPUT);
  digitalWrite(ENRANGE3, LOW);
    pinMode(ENRANGE4,OUTPUT);
  digitalWrite(ENRANGE4, LOW);

  analogReadAveraging(4);
  analogReadRes(16);
  analogReference(0);   

  // pinMode(DRDYG1, INPUT_PULLUP);
  // pinMode(INT1XM, INPUT_PULLUP);
  // pinMode(INT2XM, INPUT_PULLUP);
  Serial1.begin(115200); 
  Serial1.println("Starting...");;


  tft.begin(); 
  Serial1.println("tft started"); 
 
  tft.setTextSize(1);
   tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);
  tft.setCursor(0, 16);  
  tft.print("Starting Accel: ");
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


  tft.print("Starting Touch Pad: ");
  tft.setTextColor(ILI9341_GREEN,ILI9341_BLACK);
  touchPad.begin(touchPress);
  tft.println("Started!");
  tft.setTextColor(ILI9341_WHITE,ILI9341_BLACK);

  delay(500);
 


if (hasDOF){

  Serial1.println("Attching DOF interrupts");
    attachInterrupt(DRDYG1,interruptDOF, RISING);
    attachInterrupt(INT1XM,interruptDOF, RISING);
   attachInterrupt(INT2XM,interruptDOF, RISING);
 
   delay(100);

   interruptDOF();

 } 


  Serial1.print("Ready in ");
  Serial1.print((uint32_t)time); 
  Serial1.println(" -- Lift off!2");
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
}

void timer1(void){
    frame++;
  if (frame>120){
        digitalWrite(ENRANGE1, LOW);
        digitalWrite(ENRANGE2, LOW);
        digitalWrite(ENRANGE3, LOW);
        digitalWrite(ENRANGE4, LOW);
    checkScreen = true;
    return;
  }

  switch (frame % 8){
    case 0:  checkD4B = true;
    checkD1 = true;
    break;
    case 2:  checkD1B = true;
    checkD2 = true;
    break;
    case 4: checkD2B = true;
           checkD3 = true;
    break;
    case 6: checkD3B = true; 
        checkD4 = true;
    break;

  }

} 

void touchPress(){
  checkTouchPad = true;
}

void onTouch(uint8_t num){
    activePoint = num;
    if (activePoint<12){
      for (int i = 0; i < 4; i++){
        pointSamples[i] = 0;
        pointCalc[i] = 0;
      }
    }
}

void onRelease(uint8_t num){
    if (activePoint <12){

      for (int i = 0; i < 4; i++){
        if (pointSamples[i] == 0 ){
            distPoints[i][activePoint] = 0;
            continue;
        }

        distPoints[i][activePoint] = pointCalc[i]/pointSamples[i];
      }
    }
    activePoint = 12;
}

uint32_t lastTime = 0;

void loop(){
  if (time - lastTime>=8){
    lastTime = time;
    timer1(); 
  }

  if (checkG ){
    readGyro();
  }
  if (checkA){
    readAccel();
 
  }
  if (checkM){
    readMag();
  }

  if (checkD1 || checkD2 || checkD3 || checkD4 || checkD1B || checkD2B || checkD3B || checkD4B  ){
    readDistance();
  }
  if (checkScreen){
    updateDisplay();
  }

  if (checkTouchPad){
    checkTouchPad = false;
    touchPad.process(onTouch,onRelease);
  }
 
}

