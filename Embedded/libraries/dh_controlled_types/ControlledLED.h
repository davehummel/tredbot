#ifndef DH_CONTROLLEDLED_H__
#define DH_CONTROLLEDLED_H__
#include <sstream> 
#include "dh_controller.h"
#include <stdint.h>
using namespace std;
class ControlledLED: public Controller::Controlled{

public:

	void begin(void){
		pinMode(13, OUTPUT);
		count = 0;
	}
	void execute(uint32_t time,uint32_t id,char command[]){
		digitalWrite(13, flip);
		flip = ! flip;
		count++;
	}
	void serialize(Stream* output, uint32_t id, char command[]){
	}
	void startSchedule(char command[], uint32_t id){
		count = 0;
	}
	void endSchedule(char command[], uint32_t id){
		count = 0;
	}
	
private:
	bool flip;
	uint16_t count;
};

	

#endif