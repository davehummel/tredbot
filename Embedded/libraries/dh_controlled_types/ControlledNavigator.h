#ifndef DH_CONTROLLEDNAV_H__
#define DH_CONTROLLEDNAV_H__
#include "dh_controller.h"

#define MIN_DIST=30;
#define SLOW_DIST = 50;


class ControlledNavigator: public Controller::Controlled{

public:

	void begin(void){

	
	}
	void execute(uint32_t time,uint32_t id,char command[]){
		switch (command[0]){
			case 'G':{
				break;
			}
			case 'S':{
				break;
			}
			case 'O':{
				if (command[1] == 'N'){

				}else{

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

	int16_t x=0,y=0,z=0;

	int16_t tx=0,ty=0,tz=0;

	bool isNavigating = false, isMoving = false, bool isBacking=false;

	bool canDodge;


	// quaternion orientation
	// quaternion tOrientation
	
private:
	void navEval(uint32_t time){

	}

};

	

#endif