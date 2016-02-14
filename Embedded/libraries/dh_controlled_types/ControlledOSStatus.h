#ifndef DH_CONTROLLEDOSSTATUS_H__
#define DH_CONTROLLEDOSSTATUS_H__
#include "dh_controller.h"

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


	// uint16_t readU(ADDR1 addr,uint8_t addr2){
	//
	// }
	//
	// float readF(ADDR1 addr,uint8_t addr2){
	//
	// }
	//
	// double readD(ADDR1 addr,uint8_t addr2){
	//
	// }

	char* readS(ADDR1 addr,uint8_t addr2){
		switch (addr.addr%26+'A'){
			case 'B':  return controller->getInputBuffer();
		}
		return 0;
	}



private:


};



#endif
