#ifndef DH_CONTROLLEDLED_H__
#define DH_CONTROLLEDLED_H__
#include "dh_controller.h"

class ControlledLED: public Controller::Controlled{

public:
	ControlledLED(){

	}

	void begin(void){

	}
	void execute(uint32_t time,uint32_t id,char command[]){
		bool isPWM = true;
		uint16_t temp;
		switch (command[0]){
			case 'B':
				isPWM = false;
			case 'P':{
				uint8_t letter;
				uint8_t pID;
				if (command[4] >= 'a' && command[4] <= 'z'){
					letter = command[4] - 'a';
				}else if (command[4] >= 'A' && command[4] <= 'Z'){
					letter = command[4] - 'A';
				} else {
					controller->getErrorLogger().println("PWM/Bin must be assigned to 'a' to 'z' var.");
					return;
				}
				temp = 6;
				if (!Controller::parse_uint8(pID,temp,command)){
					controller->getErrorLogger().println("PWM/Bin must be set to a pin");
					return;
				}
					pinID[letter] = pID;
					pinVal[letter] = 0;
					pinPWM[letter] = isPWM;
				break;}
			case 'F':{
			  uint32_t sampleRate;
				temp = 4;
				if (!Controller::parse_uint32(sampleRate,temp,command)){
					controller->getErrorLogger().println("FRQ must be set to a number.");
					return;
				}
				setFreq(sampleRate);
			break;}
			case 'R':{
				uint8_t resolution;
				temp = 4;
				if (!Controller::parse_uint8(resolution,temp,command)){
					controller->getErrorLogger().println("RES must be set to a number.");
					return;
				}
				analogWriteResolution(resolution);
			break;}
			default:
				controller->getErrorLogger().print("Bad ControlledLED command:");
				controller->getErrorLogger().println(command);
		}
	}
	void startSchedule(char command[], uint32_t id){

	}
	void endSchedule(char command[], uint32_t id){

	}

	uint8_t readB(ADDR1 addr,uint8_t addr2){
			return pinVal[addr.addr%26];
	}


	void write(ADDR1 addr,uint8_t val){
		uint8_t letter = addr.addr%26;
		if (pinID[letter] == 0)
			return;
		if (pinPWM[letter]) {
			analogWrite(pinID[letter],pinVal[letter]=val);
		}else{
			digitalWrite(pinID[letter],pinVal[letter]=val);
		}

	}

private:
	void setFreq(uint32_t freq){
		analogWriteFrequency(5, freq);
		analogWriteFrequency(3, freq);
		analogWriteFrequency(25, freq);
	}

	uint8_t pinID[26] = {0};
	uint8_t pinVal[26] = {0};
	bool pinPWM[26] = {false};


};



#endif
