#ifndef DH_CONTROLLEDVMETER_H__
#define DH_CONTROLLEDVMETER_H__
#include "dh_controller.h"
#include <i2c_t3.h>


using namespace std;
class ControlledVMeter: public Controller::Controlled{
public:

	void begin(void){
		Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100 );
	}
	void execute(uint32_t time,uint32_t id,char command[]){
	    shuntvoltage = powerMonitor->getShuntVoltage_mV();
        busvoltage = powerMonitor->getBusVoltage_V();
        current_mA =powerMonitor->getCurrent_mA();
        loadvoltage = busvoltage + (shuntvoltage / 1000);
	}
	void serialize(Stream* output, uint32_t id,char command[]){
		
	}
	void startSchedule(char command[], uint32_t id){
		
	}
	void endSchedule(char command[], uint32_t id){
		
	}
	
private:
	Adafruit_INA219 *powerMonitor;
	 
	float shuntvoltage ;
	float busvoltage; 
	float current_mA;
	float loadvoltage;
};

	

#endif