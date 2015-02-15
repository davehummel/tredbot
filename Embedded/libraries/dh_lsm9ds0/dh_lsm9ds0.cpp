#include "dh_lsm9ds0.h"
#include <Wire.h> // Wire library is used for I2C

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include <dh_logger.h>

bool LSM9DS0::initAndVerify(bool gyroHigh,bool xmHigh){

	initI2C();

	gyroAddr = gyroHigh?LSM9DS0_GYR_I2C_SAD_H:LSM9DS0_GYR_I2C_SAD_L;
	xmAddr = xmHigh?LSM9DS0_GYR_I2C_SAD_H:LSM9DS0_GYR_I2C_SAD_L

	if (logger){
		logger->print("gyroAddr=");
		logger->print(gyroAddr,HEX);
	}

	uint8_t test = I2CreadByte(gyroAddr,WHO_AM_I);		// Read the gyro WHO_AM_I

	if (logger){
		logger->print("gyroID=");
		logger->print(test,HEX);
		logger->println();
	}


	if (test != LSM9DS0_G_ID){
		return false;
	}
	return true;
}

void LSM9DS0::updateSettings(){
	if (changedGyroRegisterMap&0b1 == 0b1){
		uint8_t regVal = 0;
		if (gyroXEnabled)
			regVal += 1;
		if (gyroYEnabled)
			regVal += 0b10;
		if (gyroZEnabled)
			regVal += 0b100;
		if (gyroPowered)
			regVal += 0b1000;
		regVal += gyroDrBW << 4;
		if (logger){
			logger->print("Updating Gyro Register 1:");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(gyroAddr, G_CTRL_REG1,regVal );
	}
	if (changedGyroRegisterMap&0b10 == 0b10){
		uint8_t regVal = 0;
	
		regVal += gyroHPmode << 4;
		regVal += gyroHPCutoff;
		if (logger){
			logger->print("Updating Gyro Register 2:");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(gyroAddr, G_CTRL_REG2,regVal );
	}
	if (changedGyroRegisterMap&0b100 == 0b100){
		uint8_t regVal = 0;
		if (gyroEnableFIFOEmpty_DRDY)
			regVal += 1;
		if (gyroEnableFIFOOverrun_DRDY)
			regVal += 0b10;
		if (gyroEnableWaterMark_DRDY)
			regVal += 0b100;
		if (gyroEnableDataInterup_DRDY)
			regVal += 0b1000;
		if (gyroPushPull_OpenDrain)
			regVal += 0b10000;
		if (gyroEnableActive_Int_G)
			regVal += 0b100000;
		if (gyroEnableBoot_Int_G)
			regVal += 0b1000000;
		if (gyroEnableInterput_Int_G)
			regVal += 0b10000000;
		if (logger){ 
			logger->print("Updating Gyro Register 3:");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(gyroAddr, G_CTRL_REG3,regVal );
	}
	if (changedGyroRegisterMap&0b1000 == 0b1000){
		uint8_t regVal = 0;
	
		regVal += gyroBlockDataUntilRead << 7;
		regVal += gyroBLE << 6;
		regVal += gyroScale << 4;
		regVal += gyroTestMode << 1;
		regVal += gyroSIM;

		if (logger){
			logger->print("Updating Gyro Register 4:");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(gyroAddr, G_CTRL_REG4,regVal );
		// to calculate DPS/(ADC tick) based on that 2-bit value:
	}
	if (changedGyroRegisterMap&0b10000 == 0b10000){
		uint8_t regVal = 0;
	
		regVal += gyroBoot << 7;
		regVal += gyroFIFOEnabled << 6;
		regVal += gyroHPEnabled << 4;
		regVal += gyroINT1Select << 2;
		regVal += gyroOUTSelect;

		if (logger){
			logger->print("Updating Gyro Register 5:");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(gyroAddr, G_CTRL_REG5,regVal );
	}
	if (changedMagRegister){
		uint8_t regVal = 0;
	
		regVal +=magEnableXInt << 7;
		regVal += magEnableYInt << 6;
		regVal += magEnableZInt << 5;
		regVal += magPushPull_OpenDrain << 4;
		regVal += magXMEnableActive_Int << 3;
		regVal += magXMLatch<<2;
		
		regVal += magInteruptEnabled;

		if (logger){
			logger->print("Updating Mag Register5:");
			logger->print(regVal,BIN);
			logger->println();
		}
		I2CwriteByte(XMAddr, M_INT_CTRL_REG,regVal );
	}
	if (changedXMRegisterMap&0b1 == 0b1){
		// TODO
	}
	changedGyroRegisterMap = 0;
	changedMagRegister = false;
}

void LSM9DS0::readRawGyro(){
	uint8_t temp[6]; // We'll read six bytes from the gyro into temp
	I2CreadBytes(gyroAddr,OUT_TEMP_L_XM, temp, 2); // Read 6 bytes, beginning at OUT_X_L_G
	gx = (temp[1] << 8) | temp[0]; // Store x-axis values into gx
	gy = (temp[3] << 8) | temp[2]; // Store y-axis values into gy
	gz = (temp[5] << 8) | temp[4]; // Store z-axis values into gz
}

void LSM9DS0::readRawTemp(){
	uint8_t temp[2]; // We'll read six bytes from the gyro into temp
	I2CreadBytes(xmAddr,G_OUT_X_L, temp, 2); // Read 6 bytes, beginning at OUT_X_L_G
	temperature = (((int16_t) temp[1] << 12) | temp[0] << 4 ) >> 4; // Temperature is a 12-bit signed integer
}

void LSM9DS0::readMag(){
	uint8_t temp[6]; // We'll read six bytes from the mag into temp	
	I2CreadBytes(xmAddr,OUT_X_L_M, temp, 6); // Read 6 bytes, beginning at OUT_X_L_M
	mx = (temp[1] << 8) | temp[0]; // Store x-axis values into mx
	my = (temp[3] << 8) | temp[2]; // Store y-axis values into my
	mz = (temp[5] << 8) | temp[4]; // Store z-axis values into mz
}

void LSM9DS0::readAccel(){
	uint8_t temp[6]; // We'll read six bytes from the accelerometer into temp	
	I2CreadBytes(xmAddr,OUT_X_L_A, temp, 6); // Read 6 bytes, beginning at OUT_X_L_A
	ax = (temp[1] << 8) | temp[0]; // Store x-axis values into ax
	ay = (temp[3] << 8) | temp[2]; // Store y-axis values into ay
	az = (temp[5] << 8) | temp[4]; // Store z-axis values into az
}


// PRIVATE


void LSM9DS0::initI2C()
{
	Wire.begin();	// Initialize I2C library
}

// Wire.h read and write protocols
void LSM9DS0::I2CwriteByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
	Wire.beginTransmission(address);  // Initialize the Tx buffer
	Wire.write(subAddress);           // Put slave register address in Tx buffer
	Wire.write(data);                 // Put data in Tx buffer
	Wire.endTransmission();           // Send the Tx buffer
}

uint8_t LSM9DS0::I2CreadByte(uint8_t address, uint8_t subAddress)
{
	uint8_t data; // `data` will store the register data	 
	Wire.beginTransmission(address);         // Initialize the Tx buffer
	Wire.write(subAddress);	                 // Put slave register address in Tx buffer
	Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
	Wire.requestFrom(address, (uint8_t) 1);  // Read one byte from slave register address 
	data = Wire.read();                      // Fill Rx buffer with result
	return data;                             // Return data read from slave register
}

void LSM9DS0::I2CreadBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count)
{  
	Wire.beginTransmission(address);   // Initialize the Tx buffer
	// Next send the register to be read. OR with 0x80 to indicate multi-read.
	Wire.write(subAddress | 0x80);     // Put slave register address in Tx buffer
	Wire.endTransmission(false);       // Send the Tx buffer, but send a restart to keep connection alive
	uint8_t i = 0;
	Wire.requestFrom(address, count);  // Read bytes from slave register address 
	while (Wire.available()) 
	{
		dest[i++] = Wire.read(); // Put read results in the Rx buffer
	}
}