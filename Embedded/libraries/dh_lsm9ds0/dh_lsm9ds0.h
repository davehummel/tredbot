
#ifndef DH_LSM9DS0_H__
#define DH_LSM9DS0_H__

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
#endif
#include <dh_logger.h>


#define LSM9DS0_SAD0L_ACC_MAG		(0x02)
#define LSM9DS0_SAD0H_ACC_MAG		(0x01)
#define LSM9DS0_SAD0L_GYR		(0x00)
#define LSM9DS0_SAD0H_GYR		(0x01)


#define LSM9DS0_GYR_I2C_SADROOT	(0x35)

/* I2C address if gyr SA0 pin to GND */
#define LSM9DS0_GYR_I2C_SAD_L		((LSM9DS0_GYR_I2C_SADROOT<<1)| \
							LSM9DS0_SAD0L_GYR)
/* I2C address if gyr SA0 pin to Vdd */
#define LSM9DS0_GYR_I2C_SAD_H		((LSM9DS0_GYR_I2C_SADROOT<<1)| \
							LSM9DS0_SAD0H_GYR)

#define LSM9DS0_G_ID                (0xD4)

#define G_CTRL_REG1	(0x20)    /* CTRL REG1 */
#define G_CTRL_REG2	(0x21)    /* CTRL REG2 */
#define G_CTRL_REG3	(0x22)    /* CTRL_REG3 */
#define G_CTRL_REG4	(0x23)    /* CTRL_REG4 */
#define G_CTRL_REG5	(0x24)    /* CTRL_REG5 */
#define	G_REFERENCE	(0x25)    /* REFERENCE REG */
#define	G_OUT_X_L		(0x28)    /* 1st AXIS OUT REG of 6 */
#define	G_FIFO_CTRL_REG	(0x2E)    /* FIFO CONTROL REGISTER */
#define G_FIFO_SRC_REG	(0x2F)    /* FIFO SOURCE REGISTER */


#define WHO_AM_I	(0x0F)


class LSM9DS0{
public:
	enum gyro_scale
	{
		G_SCALE_245DPS,		// 00:  245 degrees per second
		G_SCALE_500DPS,		// 01:  500 dps
		G_SCALE_2000DPS,	// 10:  2000 dps
	};

		enum gyro_dr_bw   // Data Rate and Cut off (possibly for low pass filter???)
	{
		G_DR_95_BW_125,//  95         12.5
		G_DR_95_BW_25, //  95          25
		G_DR_95_BW_25ALT, // 95          25 same as other?
		G_DR_95_BW_25ALT2, // 95          25 same as other?
		G_DR_190_BW_125 , //   190        12.5
		G_DR_190_BW_25 ,//   190         25
		G_DR_190_BW_50 , //   190         50
		G_DR_190_BW_70 , //   190         70
		G_DR_380_BW_20 , //   380         20
		G_DR_380_BW_25 , //   380         25
		G_DR_380_BW_50 , //   380         50
		G_DR_380_BW_100 , //   380         100
		G_DR_760_BW_30 , //   760         30
		G_DR_760_BW_35 , //   760         35
		G_DR_760_BW_50 , //   760         50
		G_DR_760_BW_100 , //   760         100
	};



	enum gyro_hpmode
	{
		G_HP_Normal_HP_RESET_FILTER,		
		G_HP_Reference_Signal,		
		G_HP_Normal,		
		G_HP_Autoreset_On_Interpupt,
	};


	bool initAndVerify(bool gyroHigh,bool xmHigh);

	void setGyroDrBw(gyro_dr_bw dr_bw){
			gyroDrBW = dr_bw;
			changedGyroRegisterMap |= 1;
	}

	void setGyroPowered(bool isPowered){
			gyroPowered = isPowered;
			changedGyroRegisterMap |= 1;
	}

	void setGyroAxisEnabled(bool x, bool y , bool z){
			gyroXEnabled = x;
			gyroYEnabled =y;
			gyroZEnabled = z;
			changedGyroRegisterMap |= 1;
	}

	void setHPMode(gyro_hpmode mode){
		gyroHPmode = mode;
		changedGyroRegisterMap |= 2;
	}

	void setHPCutoff(uint8_t cutoff){  // the actual cutoff depends on ODR, but 0 = highest and 9 = lowest
		gyroHPCutoff = cutoff;
		if (gyroHPCutoff>9)
			gyroHPCutoff = 9;
		changedGyroRegisterMap |= 0b10;
	}

	void setPins(bool _enableInterput_Int_G ,bool _enableBoot_Int_G ,bool _enableActive_Int_G, bool _pushPull_OpenDrain, bool _enableDataInterup_DRDY,bool _enableWaterMark_DRDY,bool _enableFIFOOverrun_DRDY,bool _enableFIFOEmpty_DRDY){
		gyroEnableInterput_Int_G =_enableInterput_Int_G;
		gyroEnableBoot_Int_G = _enableBoot_Int_G;
		gyroEnableActive_Int_G = _enableActive_Int_G; 
		gyroPushPull_OpenDrain = _pushPull_OpenDrain; 
		gyroEnableDataInterup_DRDY = _enableDataInterup_DRDY;
		gyroEnableWaterMark_DRDY = _enableWaterMark_DRDY; 
		gyroEnableFIFOOverrun_DRDY = _enableFIFOOverrun_DRDY; 
		gyroEnableFIFOEmpty_DRDY = _enableFIFOEmpty_DRDY;
		changedGyroRegisterMap |= 0b100;
	}

	void setGyroScale(gyro_scale scale){
		gyroScale = scale;
		changedGyroRegisterMap |= 0b1000;
	}

	void setGyroBlockMode(bool blockUntilRead){
		gyroBlockDataUntilRead = blockUntilRead;
		changedGyroRegisterMap |= 0b1000;
	}

	void setGyroBoot(bool boot){
		gyroBoot = boot;
		changedGyroRegisterMap |= 0b10000;
	}

	void setGyroFIFOEnabled(bool enabled){
		gyroFIFOEnabled = enabled;
		changedGyroRegisterMap |= 0b10000;
	}

	void setGyroHPEnabled(bool enabled){
		gyroHPEnabled = enabled;
		changedGyroRegisterMap |= 0b10000;
	}

	void setGyroSelectionVals(uint8_t interupt, uint8_t output){
		gyroINT1Select = interupt;

		if (gyroINT1Select > 3)
			gyroINT1Select = 3;

		gyroOUTSelect=output;

		if (gyroOUTSelect > 3)
			gyroOUTSelect = 3;

		changedGyroRegisterMap |= 0b10000;
	}

	void convertRawGyro(){
		dpsX = gyroScaleFactor*gx;
		dpsY = gyroScaleFactor*gy;
		dpsZ = gyroScaleFactor*gz;
	}

	void updateSettings();

	Logger *logger = NULL;

	void readRawGyro();

		// Gyro raw values
	int16_t gx=0,gy=0,gz=0;

	float dpsX=0,dpsY=0,dpsZ=0;

private:

	bool isMoving = true;

	uint8_t gyroAddr = 0; // I2C address for Gyro .. either 

	float gyroScaleFactor = 1;

	uint8_t changedGyroRegisterMap=0b00011111;


	// Gyro Register 1
	gyro_dr_bw gyroDrBW=G_DR_95_BW_125;

	bool gyroPowered=false;

	bool gyroZEnabled=true,gyroXEnabled=true,gyroYEnabled=true;
	
	// Gyro Register 2
	gyro_hpmode gyroHPmode=G_HP_Normal_HP_RESET_FILTER;
	uint8_t gyroHPCutoff=0000;

	// Gyro Register 3
	bool gyroEnableInterput_Int_G = false; //Interrupt enable on INT_G pin. Default value 0. (0: disable; 1: enable)
	bool gyroEnableBoot_Int_G = false; //Boot status available on INT_G. Default value 0. (0: disable; 1: enable)
	bool gyroEnableActive_Int_G = false; //Interrupt active configuration on INT_G. Default value 0. (0: high; 1:low)
	bool gyroPushPull_OpenDrain = false; //Push-pull / Open drain. Default value: 0. (0: push- pull; 1: open drain)
	bool gyroEnableDataInterup_DRDY = true; //Data-ready on DRDY_G. Default value 1. (0: disable; 1: enable)
	bool gyroEnableWaterMark_DRDY = false; // FIFO watermark interrupt on DRDY_G. Default value: 0. (0: disable; 1: enable)
	bool gyroEnableFIFOOverrun_DRDY = false; // FIFO overrun interrupt on DRDY_G. Default value: 0. (0: disable; 1: enable)
	bool gyroEnableFIFOEmpty_DRDY = false; //FIFO empty interrupt on DRDY_G. Default value: 0. (0: disable; 1: enable)
	//Gyro Register 4
	bool gyroBlockDataUntilRead = false; // continuous mode
	bool gyroBLE = false;
	gyro_scale gyroScale = G_SCALE_245DPS;
	uint8_t gyroTestMode = 0;
	bool gyroSIM = 0;
	//Gyro Register 5
	bool gyroBoot = false;
	bool gyroFIFOEnabled = false;
	bool gyroHPEnabled = false;
	uint8_t gyroINT1Select = 0; //0-3
	uint8_t gyroOUTSelect = 0; //0-3


	///////////////////
	// I2C Functions //
	///////////////////
	// initI2C() -- Initialize the I2C hardware.
	// This function will setup all I2C pins and related hardware.
	static void initI2C();
	
	// I2CwriteByte() -- Write a byte out of I2C to a register in the device
	// Input:
	//	- address = The 7-bit I2C address of the slave device.
	//	- subAddress = The register to be written to.
	//	- data = Byte to be written to the register.
	static void I2CwriteByte(uint8_t address, uint8_t subAddress, uint8_t data);
	
	// I2CreadByte() -- Read a single byte from a register over I2C.
	// Input:
	//	- address = The 7-bit I2C address of the slave device.
	//	- subAddress = The register to be read from.
	// Output:
	//	- The byte read from the requested address.
	static uint8_t I2CreadByte(uint8_t address, uint8_t subAddress);
	
	// I2CreadBytes() -- Read a series of bytes, starting at a register via SPI
	// Input:
	//	- address = The 7-bit I2C address of the slave device.
	//	- subAddress = The register to begin reading.
	// 	- * dest = Pointer to an array where we'll store the readings.
	//	- count = Number of registers to be read.
	// Output: No value is returned by the function, but the registers read are
	// 		all stored in the *dest array given.
	static void I2CreadBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count);

};

#endif