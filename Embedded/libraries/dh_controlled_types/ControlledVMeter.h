#ifndef DH_CONTROLLEDVMETER_H__
#define DH_CONTROLLEDVMETER_H__
#include "dh_controller.h"

class ControlledVMeter: public Controller::Controlled{

public:
	ControlledVMeter(uint8_t _vPin, uint8_t _cPin){
		vPin=_vPin;
		cPin=_cPin;
	}

	void begin(void){
		analogReadAveraging(32);
		analogReadResolution(12);
		pinMode(vPin, INPUT);
		pinMode(cPin, INPUT);

	}
	void execute(uint32_t time,uint32_t id,char command[]){

	}
	void startSchedule(char command[], uint32_t id){
	
	}
	void endSchedule(char command[], uint32_t id){
	
	}


	int16_t readI(ADDR1 addr,uint8_t addr2){
			switch(addr.addr%26+'A'){
				case 'C': return analogRead(cPin);
				case 'V': return analogRead(vPin);
				default: return -1;
			}
	}

	float readF(ADDR1 addr,uint8_t addr2){
			switch(addr.addr%26+'A'){
				case 'C': return ((float)readI(addr,addr2))*0.006511-4.1265;
				case 'V': return ((float)readI(addr,addr2))*0.002164+0.1937;
				default: return -1;
			}
	}



	
private:
	uint8_t vPin,cPin;

};

	

#endif