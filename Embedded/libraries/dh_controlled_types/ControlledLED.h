#ifndef DH_CONTROLLEDLED_H__
#define DH_CONTROLLEDLED_H__
#include <sstream> 
#include "dh_controlled.h"
#include <stdint.h>
using namespace std;
class ControlledLED: public Controlled{

public:

	void begin(void){
		pinMode(13, OUTPUT);
		count = 0;
	}
	void execute(uint32_t time,char command[]){
		digitalWrite(13, flip);
		flip = ! flip;
		count++;
	}
	char* serialize(void){
		return "hi";
	}
	void startSchedule(char command[]){
		count = 0;
	}
	void endSchedule(char command[]){
		count = 0;
	}
	
private:
	bool flip;
	uint16_t count;
};

	

#endif