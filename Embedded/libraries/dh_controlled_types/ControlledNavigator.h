#ifndef DH_CONTROLLEDNAV_H__
#define DH_CONTROLLEDNAV_H__
#include "dh_controller.h"
 #include <ControlledPanTilt.h>
 #include <ControlledMotor.h>
 #include <ControlledI2CXL.h>

#define MIN_DIST 30;
#define SLOW_DIST 50;


class ControlledNavigator: public Controller::Controlled{

public:

	void begin(void){
		echo = (ControlledI2CXL*) controller->getControlled('E');
		motor = (ControlledMotor*) controller->getControlled('M');
		panTilt = (ControlledPanTilt*) controller->getControlled('P');
	
	}
	void execute(uint32_t time,uint32_t id,char command[]){
		uint16_t pointer = 0;
		switch (command[0]){
			case 'G':{ // GETO
				break;
			}
			case 'S':{ //SET
				if (command[3]=='O'){   //SETO

				}else{					//SETD

				}
				break;
			}
			case 'T':{ // TURN

				break;
			}
			case 'M':{ // MOVE
				pointer = 5;
				if (!Controller::parse_uint16(targetMPerS,pointer,command)){
					return;
				}

				pointer++;

				if (!Controller::parse_uint16(targetDist,pointer,command)){
					return;
				}

				if (isMoving||isAccelerating||isTurning){
					stop();
				}

				if (panTilt->getHeight() != 512)
					controller->run(CMDRANGE+2,Controller::newString("MOVE 2 512"),'P',false);
				if (panTilt->getGoal() != 512)
					controller->run(CMDRANGE+2,Controller::newString("MOVE 1 512"),'P',false);

				break;
			}
			case 'O':{
				if (command[1] == 'N'){ // ON
					on = true;
				}else{					// OFF
					on = false;
				}
			}
			case 'N':{				
				navEval(time);
				break;
			}
		}
	}
	void serialize(Stream* output, uint32_t id, char command[]){

	}
	void startSchedule(char command[], uint32_t id){
	
	}
	void endSchedule(char command[], uint32_t id){
	
	}


	bool on = false, isAccelerating=false, isMoving = false, isTurning=false, isBlocked = false;

	uint16_t actualMS;
	uint16_t actualDist;
	uint16_t targetMPerS;
	uint16_t targetDist;

	static const uint32_t CMDRANGE = 200;

	// quaternion targetO
	// quaternion actualO
	
private:

	ControlledMotor* motor;
	ControlledI2CXL* echo;
	ControlledPanTilt* panTilt;

	void navEval(uint32_t time){

	}

	void stop(){

	}

};

	

#endif