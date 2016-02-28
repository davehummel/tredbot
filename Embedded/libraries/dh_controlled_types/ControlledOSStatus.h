#ifndef DH_CONTROLLEDOSSTATUS_H__
#define DH_CONTROLLEDOSSTATUS_H__
#include "dh_controller.h"

const uint16_t INP = ADDR1::solveADDR("INP");
const uint16_t ETX = ADDR1::solveADDR("ETX");
const uint16_t ETM = ADDR1::solveADDR("ETM");
const uint16_t ECD = ADDR1::solveADDR("ECD");

class ControlledOSStatus: public Controller::Controlled{

public:
	ControlledOSStatus(){
	}

	void begin(void){


	}
	void execute(uint32_t time,uint32_t id,char command[]){

	}
	void startSchedule(char command[], uint32_t id){

	}
	void endSchedule(char command[], uint32_t id){

	}


	uint8_t readB(ADDR1 addr,uint8_t addr2){
	//	if (addr.addr == ECD) return controller->getErrorCode();
		return 0 ;
	}
	//
	// float readF(ADDR1 addr,uint8_t addr2){
	//
	// }
	//
	uint32_t readT(ADDR1 addr,uint8_t addr2){
			//	if (addr.addr == ETM) return controller->getErrorTime();
				return 0;
	}

	char* readS(ADDR1 addr,uint8_t addr2){

			if (addr.addr == INP) return controller->getInputBuffer();

			if (addr.addr == ETX) return controller->getErrorLogger().getErrorText();


		return 0;
	}



private:


};



#endif
