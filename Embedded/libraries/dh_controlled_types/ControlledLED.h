#ifndef DH_CONTROLLEDLED_H__
#define DH_CONTROLLEDLED_H__
#include "dh_controller.h"

class ControlledLED: public Controller::Controlled{

public:
	ControlledLED(uint8_t _rPin, uint8_t _gPin, uint8_t _bPin,uint8_t _blinkPin=13){
		rPin=_rPin;
		gPin=_gPin;
		bPin =_bPin;
		blinkPin=_blinkPin;
		r=g=b=blink =0;
	}

	void begin(void){
		pinMode(rPin, OUTPUT);
		pinMode(gPin, OUTPUT);
		pinMode(bPin, OUTPUT);

		pinMode(blinkPin, OUTPUT);
	}
	void execute(uint32_t time,uint32_t id,char command[]){

	}
	void startSchedule(char command[], uint32_t id){
	
	}
	void endSchedule(char command[], uint32_t id){
	
	}

	uint8_t readB(ADDR1 addr,uint8_t addr2){
			switch(addr.addr%26+'A'){
				case 'R': return r;
				case 'G': return g;
				case 'B': return b;
				default: return blink;
			}
	}


	void write(ADDR1 addr,uint8_t val){
			switch(addr.addr%26+'A'){
				case 'R': r= val; analogWrite(rPin,r);break;
				case 'G': g = val; analogWrite(gPin,g);break;
				case 'B': b = val; analogWrite(bPin,b);break;
				default:  blink = val; digitalWrite(blinkPin,blink);break;
			}

	}
	
private:
	uint8_t rPin,gPin,bPin,blinkPin;
	bool blink;
	uint8_t r,g,b;
};

	

#endif