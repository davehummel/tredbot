#include <SPI.h> // Included for SFE_LSM9DS0 library
#include <Wire.h>
#include <SFE_LSM9DS0.h>
#include "Arduino.h"

///////////////////////
// Example I2C Setup //
///////////////////////
// Comment out this section if you're using SPI
// SDO_XM and SDO_G are both grounded, so our addresses are:
#define LSM9DS0_XM  0x1D // Would be 0x1E if SDO_XM is LOW
#define LSM9DS0_G   0x6B // Would be 0x6A if SDO_G is LOW

///////////////////////////////
// Interrupt Pin Definitions //
///////////////////////////////
const byte INT1XM = 3; // INT1XM tells us when accel data is ready
const byte INT2XM = 2; // INT2XM tells us when mag data is ready
const byte DRDYG  = 4; // DRDYG  tells us when gyro data is ready

// Create an instance of the LSM9DS0 library called `dof` the
// parameters for this constructor are:
// [SPI or I2C Mode declaration],[gyro I2C address],[xm I2C add.]
LSM9DS0 dof(MODE_I2C, LSM9DS0_G, LSM9DS0_XM);
float abias[3] = {0, 0, 0}, gbias[3] = {0, 0, 0};

float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values 
float temperature;

uint32_t count = 0;  // used to control display output rate
uint32_t delt_t = 0; // used to control display output rate
float pitch, yaw, roll, heading;
float deltat = 0.0f;        // integration interval for both filter schemes
uint32_t lastUpdate = 0;    // used to calculate integration interval
uint32_t Now = 0;           // used to calculate integration interval

void setup()
{
  Serial1.begin(115200);

  // Set up interrupt pins as inputs:
  pinMode(INT1XM, INPUT);
  pinMode(INT2XM, INPUT);
  pinMode(DRDYG,  INPUT);
           
  // begin() returns a 16-bit value which includes both the gyro 
  // and accelerometers WHO_AM_I response. You can check this to
  // make sure communication was successful.
  uint32_t status = dof.begin();
 
  Serial1.print("LSM9DS0 WHO_AM_I's returned: 0x");
  Serial1.println(status, HEX);
  Serial1.println("Should be 0x49D4");
  Serial1.println();

 // Set data output ranges; choose lowest ranges for maximum resolution
 // Accelerometer scale can be: A_SCALE_2G, A_SCALE_4G, A_SCALE_6G, A_SCALE_8G, or A_SCALE_16G   
    dof.setAccelScale(dof.A_SCALE_2G);
 // Gyro scale can be:  G_SCALE__245, G_SCALE__500, or G_SCALE__2000DPS
    dof.setGyroScale(dof.G_SCALE_245DPS);
 // Magnetometer scale can be: M_SCALE_2GS, M_SCALE_4GS, M_SCALE_8GS, M_SCALE_12GS   
    dof.setMagScale(dof.M_SCALE_2GS);


    dof.setAccelODR(dof.A_ODR_100); // Set accelerometer update rate at 100 Hz
 // Accelerometer anti-aliasing filter rate can be 50, 194, 362, or 763 Hz
 // Anti-aliasing acts like a low-pass filter allowing oversampling of accelerometer and rejection of high-frequency spurious noise.
 // Strategy here is to effectively oversample accelerometer at 100 Hz and use a 50 Hz anti-aliasing (low-pass) filter frequency
 // to get a smooth ~150 Hz filter update rate
    dof.setAccelABW(dof.A_ABW_50); // Choose lowest filter setting for low noise
 
 // Gyro output data rates can be: 95 Hz (bandwidth 12.5 or 25 Hz), 190 Hz (bandwidth 12.5, 25, 50, or 70 Hz)
 //                                 380 Hz (bandwidth 20, 25, 50, 100 Hz), or 760 Hz (bandwidth 30, 35, 50, 100 Hz)
    dof.setGyroODR(dof.G_ODR_190_BW_125);  // Set gyro update rate to 190 Hz with the smallest bandwidth for low noise

 // Magnetometer output data rate can be: 3.125 (ODR_3125), 6.25 (ODR_625), 12.5 (ODR_125), 25, 50, or 100 Hz
    dof.setMagODR(dof.M_ODR_125); // Set magnetometer to update every 80 ms
    
 // Use the FIFO mode to average accelerometer and gyro readings to calculate the biases, which can then be removed from
 // all subsequent measurements.
    dof.calLSM9DS0(gbias, abias);
  Serial1.println("REadey");
  Serial1.println();
 }

 void loop()
{
	bool doGyro=true,doAccel=true,doMag=true;
	uint32_t startTime =micros();
	uint16_t countGyro=0,countAccel=0,countMag = 0, countTotal =0;
	while (true){
		countTotal++;
		bool updatedGyro = false,updatedAccel=false,updatedMag=false;
		if(doGyro && digitalRead(DRDYG)) {  // When new gyro data is ready
			countGyro++;
	 		dof.readGyro();           // Read raw gyro data
	    	gx = dof.calcGyro(dof.gx) - gbias[0];   // Convert to degrees per seconds, remove gyro biases
	    	gy = dof.calcGyro(dof.gy) - gbias[1];
	    	gz = dof.calcGyro(dof.gz) - gbias[2];
	  	}
	  
	  if(doAccel && digitalRead(INT1XM)) {  // When new accelerometer data is ready
	  	countAccel++;
	    dof.readAccel();         // Read raw accelerometer data
	    ax = dof.calcAccel(dof.ax) - abias[0];   // Convert to g's, remove accelerometer biases
	    ay = dof.calcAccel(dof.ay) - abias[1];
	    az = dof.calcAccel(dof.az) - abias[2];
	  }
	  
	  if(doMag && digitalRead(INT2XM)) {  // When new magnetometer data is ready
	    dof.readMag();           // Read raw magnetometer data
	    countMag++;
	    mx = dof.calcMag(dof.mx);     // Convert to Gauss and correct for calibration
	    my = dof.calcMag(dof.my);
	    mz = dof.calcMag(dof.mz);
	    
	    dof.readTemp();
	    temperature = 21.0 + (float) dof.temperature/8.; // slope is 8 LSB per degree C, just guessing at the intercept
	  }

	  Now = micros();
	  deltat = ((Now - lastUpdate)/1000000.0f); // set integration time by time elapsed since last filter update
	  lastUpdate = Now;


	  if (Serial1.available()){
	  	char c = Serial.read();
	  	switch (c){
	  		case 'g':
	  			doGyro= !doGyro;
	  			break;
	  		case 'a':
	  			doAccel = !doAccel;
	  			break;
	  		case 'm':
	  			doMag = !doMag;
	  			break;
	  		case 'd':
	  			 double delta = ((Now - startTime)/1000000.0f);
	  			 startTime = Now;
	  			 Serial1.print("Avg Cycle:");
	  			 Serial1.println(delta/countTotal,8);
	  			 Serial1.print("Avg Gyro:");
	  			 Serial1.println(delta/countGyro,8);
	  			 Serial1.print("Avg Accel:");
	  			 Serial1.println(delta/countAccel,8);
	  			 Serial1.print("Avg Mag:");
	  			 Serial1.println(delta/countMag,8);
	  			 countTotal = countGyro = countAccel = countMag = 0;
	  			break;
	  		}
	  	}else{
	  		
	  	}



	 

	}
}