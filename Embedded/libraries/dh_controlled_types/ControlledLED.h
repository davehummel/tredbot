#ifndef DH_CONTROLLEDLED_H__
#define DH_CONTROLLEDLED_H__
#include "dh_controller.h"

class ControlledLED: public Controller::Controlled{

public:

	void begin(void){
		pinMode(13, OUTPUT);
		count = 0;
	}
	void execute(uint32_t time,uint32_t id,char command[], bool serializeOnComplete){
		digitalWrite(13, flip);
		flip = ! flip;
		count++;
	}
	void serialize(Logger* logger, uint32_t id, char command[]) {
		logger->setTime(millis());
		logger->sendTimeSync();
	}
	void serialize(Stream* output, uint32_t id, char command[]){
		//Serial.println('.');
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