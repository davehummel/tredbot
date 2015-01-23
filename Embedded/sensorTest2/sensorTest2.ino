#define DEBUG


#include <Wire.h>
#include <dh_lsm9ds0.h>
#include "Arduino.h"
#include <dh_logger.h>
#include <dh_arduino_logger.h>


///////////////////////////////
// Interrupt Pin Definitions //
///////////////////////////////
const byte INT1XM = 3; // INT1XM tells us when accel data is ready
const byte INT2XM = 2; // INT2XM tells us when mag data is ready
const byte DRDYG1  = 4; // DRDYG  tells us when gyro data is ready
const byte DRDYG2  = 7; // DRDYG  tells us when gyro data is ready


LSM9DS0 dof = LSM9DS0();
LSM9DS0 dof2 = LSM9DS0();

void readGyro(){
  if (digitalRead(DRDYG1)){
  dof.processGyroUpdate(micros());
  Serial.write("Gyro 1:(");
  Serial.print(dof.gx);
  Serial.write(",");
  Serial.print(dof.gy);
  Serial.write(",");
  Serial.print(dof.gz);
  Serial.println(")");
  }
  if (digitalRead(DRDYG2)){
    dof2.processGyroUpdate(micros());
  Serial.write("Gyro 2:(");
  Serial.print(dof2.gx);
  Serial.write(",");
  Serial.print(dof2.gy);
  Serial.write(",");
  Serial.print(dof2.gz);
  Serial.println(")");
  }
    
}


void setup(){
  Serial.begin(115200);
  Serial.println("Starting");
  dof.logger = new  ArduinoLogger(0);
  dof2.logger = new  ArduinoLogger(0);
  if (!dof.initAndVerify(true,true)){
    Serial.println("Failed LSM9DS0 init1");
  }
  if (!dof2.initAndVerify(false,false)){
    Serial.println("Failed LSM9DS0 init2");
  }
  dof.setGyroPowered(true);
  dof.setGyroAxisEnabled(true,true,true);
  dof.setPins(false,false,false,false,true,false,false,false);
  dof.updateSettings();
  delay(2000);
  dof2.setGyroPowered(true);
  dof2.setGyroAxisEnabled(true,true,true);
  dof2.setPins(false,false,false,false,true,false,false,false);
  dof2.updateSettings();

  pinMode(DRDYG1, INPUT);
  pinMode(DRDYG2, INPUT);
 
  attachInterrupt(DRDYG1,readGyro, RISING);
  attachInterrupt(DRDYG2,readGyro, RISING);

}


void loop(){

}
// // Create an instance of the LSM9DS0 library called `dof` the
// // parameters for this constructor are:
// // [SPI or I2C Mode declaration],[gyro I2C address],[xm I2C add.]
// Adafruit_LSM9DS0 dof2 = Adafruit_LSM9DS0();   // Use I2C, ID #1000
 

// volatile int32_t abias[3] = {0, 0, 0}, gbias[3] = {0, 0, 0};
// volatile bool zeroSensors = false;
// volatile int16_t ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 
// volatile double headX=0,headY=0,headZ=0;
// volatile float temperature;

// volatile uint32_t gyroReadTime =0;



// volatile uint32_t count = 0;  // used to control display output rate
// uint32_t delt_t = 0; // used to control display output rate
// double pitch, yaw, roll, heading;
// double deltat = 0.0f;        // integration interval for both filter schemes
// uint32_t lastUpdate = 0;    // used to calculate integration interval
// uint32_t Now = 0;           // used to calculate integration interval
// boolean highPassOn = false;

// void setup()
// {
//   Serial.begin(115200);
//   Serial.println("Starting");
//   // Set up interrupt pins as inputs:
//   pinMode(INT1XM, INPUT);
//   pinMode(INT2XM, INPUT);
//   pinMode(DRDYG,  INPUT);
//  // pinMode(13, OUTPUT);   
//  // digitalWrite(13, HIGH);      
//   // begin() returns a 16-bit value which includes both the gyro 
//   // and accelerometers WHO_AM_I response. You can check this to
//   // make sure communication was successful.

 
//   Serial.print("LSM9DS0 Init = ");
//   Serial.println(dof2.begin());
//  // digitalWrite(13, LOW); 
//  }
 
//  bool doingGyroZero = false;
//  uint8_t gsamples;
//  void gyroUpdate(){
//           count++;
//          uint32_t temp = micros();
//          temp = temp - gyroReadTime;
//          gyroReadTime+=temp;
         
//          dof2.readGyro();           // Read raw gyro data
         
        
//          gx = dof2.gyroData.x; //dof.calcGyro(dof.gx) - gbias[0];   // Convert to degrees per seconds, remove gyro biases
//          gy = dof2.gyroData.y;//dof.calcGyro(dof.gy) - gbias[1];
//          gz = dof2.gyroData.z;//dof.calcGyro(dof.gz) - gbias[2];
         
//         if (zeroSensors){

//           if (!doingGyroZero){
//             headX=headY=headZ = 0;
//             gbias[0]=gbias[1]=gbias[2]=0;
//             gsamples = 0;
//             doingGyroZero = true;
//           }

//           gbias[0]+=gx;
//           gbias[1]+=gy;
//           gbias[2]+=gz;

//           gsamples++;

//         }else{

//           if (doingGyroZero){
//             gbias[0] = gbias[0]/gsamples;
//             gbias[1] = gbias[1]/gsamples;
//             gbias[2] = gbias[2]/gsamples;
//             doingGyroZero = false;
//           }

//           headX+=((gx-gbias[0]));//*temp)/1000000.0d;
//           headY+=((gy-gbias[1]));//*temp)/1000000.0d;
//           headZ+=((gz-gbias[2]));//*temp)/1000000.0d;
//         }
//  }

//  void loop()
// {
//   attachInterrupt(DRDYG,gyroUpdate,RISING);
//   int seconds = 0;
//   while (true){

//     Now = millis();
//     deltat = ((Now - lastUpdate)/1000.0f); // set integration time by time elapsed since last filter update
//     if (Serial.available()){
//       char c = Serial.read();
//       switch(c){
//         case 'z':
//           zeroSensors = true;
//           delay(2000);
//           zeroSensors = false;
//           Serial.write("calibration complete:");
//           Serial.print(gsamples);
//           Serial.write(",");
//           Serial.print(gbias[0]);
//           Serial.write(",");
//           Serial.print(gbias[1]);
//           Serial.write(",");
//           Serial.println(gbias[2]);
//           break;
//         case 'h':
//           highPassOn = ! highPassOn;
//           dof2.highPassOn(highPassOn);
//           Serial.write("High pass is ");
//           Serial.println(highPassOn);
//           break;
//         case 'p':
//           dof2.printRegisters();
//           break;
//       }
//     }
//     if (deltat<1)
//       continue;
//     lastUpdate = Now;

//           seconds ++;
//           Serial.print(count);
//           Serial.write(",");
//           Serial.print(headX,4);
//           Serial.write(",");
//           Serial.print(headY,4);
//           Serial.write(",");
//           Serial.print(headZ,4);
//           Serial.write(",");
//           Serial.println(seconds);

//           count = 0;

   
//   }
// }

