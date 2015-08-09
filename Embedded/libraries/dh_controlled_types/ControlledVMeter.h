#ifndef DH_CONTROLLEDVMETER_H__
#define DH_CONTROLLEDVMETER_H__
#include "dh_controller.h"
#include <i2c_t3.h>


/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define INA219_ADDRESS                         (0x40)    // 1000000 (A0+A1=GND)
    #define INA219_READ                            (0x01)
/*=========================================================================*/

/*=========================================================================
    CONFIG REGISTER (R/W)
    -----------------------------------------------------------------------*/
    #define INA219_REG_CONFIG                      (0x00)
    /*---------------------------------------------------------------------*/
    #define INA219_CONFIG_RESET                    (0x8000)  // Reset Bit
	
    #define INA219_CONFIG_BVOLTAGERANGE_MASK       (0x2000)  // Bus Voltage Range Mask
    #define INA219_CONFIG_BVOLTAGERANGE_16V        (0x0000)  // 0-16V Range
    #define INA219_CONFIG_BVOLTAGERANGE_32V        (0x2000)  // 0-32V Range
	
    #define INA219_CONFIG_GAIN_MASK                (0x1800)  // Gain Mask
    #define INA219_CONFIG_GAIN_1_40MV              (0x0000)  // Gain 1, 40mV Range
    #define INA219_CONFIG_GAIN_2_80MV              (0x0800)  // Gain 2, 80mV Range
    #define INA219_CONFIG_GAIN_4_160MV             (0x1000)  // Gain 4, 160mV Range
    #define INA219_CONFIG_GAIN_8_320MV             (0x1800)  // Gain 8, 320mV Range
	
    #define INA219_CONFIG_BADCRES_MASK             (0x0780)  // Bus ADC Resolution Mask
    #define INA219_CONFIG_BADCRES_9BIT             (0x0080)  // 9-bit bus res = 0..511
    #define INA219_CONFIG_BADCRES_10BIT            (0x0100)  // 10-bit bus res = 0..1023
    #define INA219_CONFIG_BADCRES_11BIT            (0x0200)  // 11-bit bus res = 0..2047
    #define INA219_CONFIG_BADCRES_12BIT            (0x0400)  // 12-bit bus res = 0..4097
    
    #define INA219_CONFIG_SADCRES_MASK             (0x0078)  // Shunt ADC Resolution and Averaging Mask
    #define INA219_CONFIG_SADCRES_9BIT_1S_84US     (0x0000)  // 1 x 9-bit shunt sample
    #define INA219_CONFIG_SADCRES_10BIT_1S_148US   (0x0008)  // 1 x 10-bit shunt sample
    #define INA219_CONFIG_SADCRES_11BIT_1S_276US   (0x0010)  // 1 x 11-bit shunt sample
    #define INA219_CONFIG_SADCRES_12BIT_1S_532US   (0x0018)  // 1 x 12-bit shunt sample
    #define INA219_CONFIG_SADCRES_12BIT_2S_1060US  (0x0048)	 // 2 x 12-bit shunt samples averaged together
    #define INA219_CONFIG_SADCRES_12BIT_4S_2130US  (0x0050)  // 4 x 12-bit shunt samples averaged together
    #define INA219_CONFIG_SADCRES_12BIT_8S_4260US  (0x0058)  // 8 x 12-bit shunt samples averaged together
    #define INA219_CONFIG_SADCRES_12BIT_16S_8510US (0x0060)  // 16 x 12-bit shunt samples averaged together
    #define INA219_CONFIG_SADCRES_12BIT_32S_17MS   (0x0068)  // 32 x 12-bit shunt samples averaged together
    #define INA219_CONFIG_SADCRES_12BIT_64S_34MS   (0x0070)  // 64 x 12-bit shunt samples averaged together
    #define INA219_CONFIG_SADCRES_12BIT_128S_69MS  (0x0078)  // 128 x 12-bit shunt samples averaged together
	
    #define INA219_CONFIG_MODE_MASK                (0x0007)  // Operating Mode Mask
    #define INA219_CONFIG_MODE_POWERDOWN           (0x0000)
    #define INA219_CONFIG_MODE_SVOLT_TRIGGERED     (0x0001)
    #define INA219_CONFIG_MODE_BVOLT_TRIGGERED     (0x0002)
    #define INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED (0x0003)
    #define INA219_CONFIG_MODE_ADCOFF              (0x0004)
    #define INA219_CONFIG_MODE_SVOLT_CONTINUOUS    (0x0005)
    #define INA219_CONFIG_MODE_BVOLT_CONTINUOUS    (0x0006)
    #define INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS (0x0007)	
/*=========================================================================*/

/*=========================================================================
    SHUNT VOLTAGE REGISTER (R)
    -----------------------------------------------------------------------*/
    #define INA219_REG_SHUNTVOLTAGE                (0x01)
/*=========================================================================*/

/*=========================================================================1
    BUS VOLTAGE REGISTER (R)
    -----------------------------------------------------------------------*/
    #define INA219_REG_BUSVOLTAGE                  (0x02)
/*=========================================================================*/

/*=========================================================================
    POWER REGISTER (R)
    -----------------------------------------------------------------------*/
    #define INA219_REG_POWER                       (0x03)
/*=========================================================================*/

/*=========================================================================
    CURRENT REGISTER (R)
    -----------------------------------------------------------------------*/
    #define INA219_REG_CURRENT                     (0x04)
/*=========================================================================*/

/*=========================================================================
    CALIBRATION REGISTER (R/W)
    -----------------------------------------------------------------------*/
    #define INA219_REG_CALIBRATION                 (0x05)
/*=========================================================================*/

class ControlledVMeter: public Controller::Controlled{
public:

	void begin(void){
		if (!started){
			Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100 );
		
		  // Calibration which uses the widest range and low voltages for 
		  // current measurement (10mA), supporting 16V at 3200mA max.

		  // VBUS_MAX = 16V
		  // VSHUNT_MAX = 0.32          (Assumes Gain 8, 320mV)
		  // RSHUNT = 0.1               (Resistor value in ohms)
		  
		  // 1. Determine max possible current
		  // MaxPossible_I = VSHUNT_MAX / RSHUNT
		  // MaxPossible_I = 3.2A

		  // 2. Determine max expected current
		  // MaxExpected_I = 3.2A
		  
		  // 3. Calculate possible range of LSBs (Min = 15-bit, Max = 12-bit)
		  // MinimumLSB = MaxExpected_I/32767
		  // MinimumLSB = 0.0000976            
		  // MaximumLSB = MaxExpected_I/4096
		  // MaximumLSB = 0.000781              (98uA per bit)
		  
		  // 4. Choose an LSB between the min and max values
		  //    (Preferrably a roundish number close to MinLSB)
		  // CurrentLSB = 0.0001 (100uA per bit)
		  
		  // 5. Compute the calibration register
		  // Cal = trunc (0.04096 / (Current_LSB * RSHUNT))
		  // Cal = 4096 (0x1000)
		  
		  ina219_calValue = 4096;


		  ina219_currentMultiplier_mA = 10;  
		  ina219_powerDivider_mW = 2;     // Power LSB = 1mW per bit

		  // Set Calibration register to 'Cal' calculated above 
		  wireWriteRegister(INA219_REG_CALIBRATION, ina219_calValue);
		  
		  // Set Config register to take into account the settings above
		  uint16_t config = INA219_CONFIG_BVOLTAGERANGE_16V |
		                    INA219_CONFIG_GAIN_8_320MV |
		                    INA219_CONFIG_BADCRES_12BIT |
		                    INA219_CONFIG_SADCRES_12BIT_1S_532US |
		                    INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;
		  wireWriteRegister(INA219_REG_CONFIG, config);
			started = true;
		}
	}
	void execute(uint32_t time,uint32_t id,char command[], bool serializeOnComplete){
		uint16_t pointer = 0;
		uint16_t scanDelay,scanInt;
		uint32_t scanCount;
		switch (command[0]){
			case 'B':
				pointer+=6;
				if (!Controller::parse_uint16(scanDelay,pointer,command)){
					return;
				}

				pointer++;
		
				if (!Controller::parse_uint16(scanInt,pointer,command)){
					return;
				}

				pointer++;

				if (!Controller::parse_uint32(scanCount,pointer,command)){
					return;
				}
				controller->schedule(id+1,scanDelay,scanInt,false,scanCount,Controller::newString("1"),'V',false);
			break;
			case '1':
				wireAskRegister(INA219_REG_SHUNTVOLTAGE);
				controller->schedule(id,1,0,false,1,Controller::newString("2"),'V',false);
			break;
			case '2':
				shuntVoltage=wireReadRegister()*.01;
				current_mA = shuntVoltage*ina219_currentMultiplier_mA;
				wireAskRegister(INA219_REG_BUSVOLTAGE);
				controller->schedule(id,1,0,false,1,Controller::newString("3"),'V',true);
			break;
			case '3':
				busVoltage =  ((int16_t)((wireReadRegister() >> 3) * 4)) * 0.001;
				loadVoltage = busVoltage + (shuntVoltage/ 1000);
				readTime = time;
				// Force reset incase of crash
			break;
		}
	}
	void serialize(Stream* output, uint32_t id,char command[]){
		if (command[0] =='3'){
			output->print('<');
			output->print(id);
			output->print(":C=");
			output->print(current_mA);
			output->print(":V=");
			output->print(loadVoltage);
			output->print('@');
			output->println(readTime);
		}
	}
	void startSchedule(char command[], uint32_t id){
		
	}
	void endSchedule(char command[], uint32_t id){
		
	}
	
private:

	void wireWriteRegister (uint8_t reg, uint16_t value){
	  	Wire.beginTransmission(ina219_i2caddr);
	    Wire.write(reg);                       // Register
	    Wire.write((value >> 8) & 0xFF);       // Upper 8-bits
	    Wire.write(value & 0xFF);              // Lower 8-bits
	  	Wire.endTransmission();
	}

	void wireAskRegister (uint8_t reg){
	  	Wire.beginTransmission(ina219_i2caddr);
	    Wire.write(reg);                       // Register
	  	Wire.endTransmission();	
	}

	uint16_t wireReadRegister(){
	  Wire.requestFrom(ina219_i2caddr, (uint8_t)2);  
	  uint16_t temp =  ((Wire.read() << 8) | Wire.read());
	  return temp;
	}


	bool started = false;
	 
	float shuntVoltage ;
	float busVoltage; 
	float current_mA;
	float loadVoltage;
	uint32_t readTime;

 	uint8_t ina219_i2caddr = INA219_ADDRESS;
  	uint32_t ina219_calValue;
  	// The following multipliers are used to convert raw current and power
  	// values to mA and mW, taking into account the current config settings
 	uint32_t ina219_currentMultiplier_mA;
  	uint32_t ina219_powerDivider_mW;

};



	

#endif