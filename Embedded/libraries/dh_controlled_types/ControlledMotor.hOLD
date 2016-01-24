#ifndef DH_CONTROLLEDMOTOR_H__
#define DH_CONTROLLEDMOTOR_H__
#include "dh_controller.h"
#include "dh_qik2s12v10.h"


class ControlledMotor: public Controller::Controlled{
public:

	void begin(void){
		if (!motor){
			Serial2.begin(115200);
			motor = new qik2s12v10(&Serial2);
		}
	}

	void execute(uint32_t time,uint32_t id,char command[], bool serializeOnComplete){
		uint16_t pointer = 0;
		switch (command[0]){
			case 'B':
			{
				pointer=6;


				uint8_t m0break,m1break;

				if (!Controller::parse_uint8(m0break,pointer,command)){
					return;
				}

				pointer++;
		
				if (!Controller::parse_uint8(m1break,pointer,command)){
					return;
				}

				motor->breaks(m0break,m1break);
				m0Speed=m1Speed=0;
				break;
			}
			case 'M':
			{

				pointer = 5;
				
				if (command[pointer]=='R')
					m0F = false;
				else
					m0F = true;

				pointer++;

				if (!Controller::parse_uint8(m0Speed,pointer,command)){
					return;
				}

				pointer++;

				if (command[pointer]=='R')
					m1F = false;
				else
					m1F = true;

				pointer++;

				if (!Controller::parse_uint8(m1Speed,pointer,command)){
					return;
				}
				updateMotor();

				break;
			}
			case 'C':
				readTime = time;
				motor->getCurrent_150ma(m0Current,m1Current);
			break;
			case 'L':{
				pointer = 5;
				readTime = time;
				uint8_t value=0;
				if (command[pointer]=='R'){
					pointer+=2;
					if (!Controller::parse_uint8(value,pointer,command)){
						return;
					}
					motor->setCurrentLimitResponse(value,value);
				}else{
					pointer++;
					if (!Controller::parse_uint8(value,pointer,command)){
						return;
					}
					motor->setCurrentLimit_300ma(value,value);
				}
				break;}
			case 'P':{
				readTime = time;
				uint8_t value=0;
				pointer=4;
				if (!Controller::parse_uint8(value,pointer,command)){
					return;
				}
				motor->setPWMParm(value);
				break;
			}
			case 'S':
				readTime = time;
				motor->getSpeed(m0Speed,m1Speed);
			break;
			case 'E':{
				readTime = time;
				error = motor->getErrorByte();
				break;
			}
			case 'A':{
				readTime = time;
				uint8_t value=0;
				pointer=6;
				if (!Controller::parse_uint8(value,pointer,command)){
					return;
				}
				motor->setMotorAccel(value,value);
				break;
			}
			case 'T':{
				readTime = time;
				uint8_t value1=0,value2=0;
				pointer=8;
				if (!Controller::parse_uint8(value1,pointer,command)){
					return;
				}
				pointer++;
				Controller::parse_uint8(value2,pointer,command);
				motor->setTimeOut(value1,value2);
				break;
			}
			case 'R':{
				readTime = time;
				pointer = 5;


				if (!Controller::parse_uint8(readConfigNum,pointer,command)){
					return;
				}
	
				readConfigVal = motor->getConfigValue(readConfigNum);
			}

		}
	}
	void serialize(Stream* output, uint32_t id,char command[]){
		if (command[0] =='C'){
			output->print('<');
			output->print(id);
			output->print(":M0C=");
			output->print(m0Current*.150);
			output->print(":M1C=");
			output->print(m1Current*.150);
			output->print('@');
			output->println(readTime);
			return;
		}
		if (command[0] =='S'){
			output->print('<');
			output->print(id);
			output->print(":M0S=");
			output->print(m0Speed);
			output->print(":M1S=");
			output->print(m1Speed);
			output->print('@');
			output->println(readTime);
			return;
		}
		if (command[0] == 'E'){
			if (error != 0){
				output->print('<');
				output->print(id);
				output->print("Error#");
				output->println(error);
			}
			return;
		}
		if (command[0] =='R'){
			output->print('<');
			output->print(id);
			output->print("Config#");
			output->print(readConfigNum);
			output->print('=');
			output->println(readConfigVal);
			return;
		}
	}

	void serialize(Logger* logger, uint32_t id, char command[]) {
		switch (command[0]) {
		case 'E':
			if (error > 0) {
				logger->setTime(millis());
				logger->print(error);
				logger->send();
			}
			break;
		}
	}


	void startSchedule(char command[], uint32_t id){
		
	}
	void endSchedule(char command[], uint32_t id){
		
	}

	void setMotor(int16_t basethrottle, int16_t differential){
		if (basethrottle > 255)
			basethrottle = 255;
		else if (basethrottle < -255)
			basethrottle = 255;

		if (differential>512)
			differential = 512;
		else if (differential < -512)
			differential = -512;


		int16_t a, b;
		if (differential == 1){
			a = basethrottle - 1;
			b = basethrottle;
		}
		else if (differential == -1){
			a = basethrottle;
			b = basethrottle - 1;
		}
		else {
			a = basethrottle - differential / 2;
			b = basethrottle + differential / 2;
		}

		if (a > 255){
			b = b + 255 - a;
			a = 255;
		}
		else if (b > 255) {
			a = a + 255 - b;
			b = 255;
		}
		else if (a < -255){
			b = b - a - 255;
			a = -255;
		}
		else if (b < -255){
			a = a - b - 255;
			b = -255;
		}

		if (a > 255)
			a = 255;
		else if (a < -255)
			a = -255;
		if (b > 255)
			b = 255;
		else if (b < -255)
			b = -255;

		if (a < 0){
			m0Speed = -a;
			m0F = false; 
		}
		else {
			m0Speed = a; 
			m0F = true;
		}

		if (b < 0){
			m1Speed = -b;
			m1F = false;
		}
		else {
			m1Speed = b;
			m1F = true;
		}
	}

	void setMotorDirect(int16_t m0, int16_t m1){
		if (m0 > 255)
			m0 = 255;
		else if (m0 < -255)
			m0 = -255;

		if (m1 > 255)
			m1 = 255;
		else if (m1 < -255)
			m1 = -255;

		if (m0 < 0){
			m0Speed = m0*-1;
			m0F = false;
		}
		else{
			m0Speed = m0;
			m0F = true;
		}

		if (m1 < 0){
			m1Speed = m1*-1;
			m1F = false; 
		}
		else{
			m1Speed = m1;
			m1F = true;
		}
	}

	void updateMotor(){
		motor->move(m0F,m0Speed,m1F,m1Speed);
		Serial.print("Motor:");
		Serial.print(m0F);
		Serial.print(",");
		Serial.print(m0Speed);
		Serial.print(",");
		Serial.print(m1F);
		Serial.print(",");
		Serial.println(m1Speed);
	}

	void stopMotor(){
		motor->breaks(255,255);
		m0Speed=m1Speed=0;
	}

	uint8_t m0Current,m1Current;
	uint8_t m0Speed,m1Speed;
	bool m0F,m1F;

	uint8_t error;

	uint8_t readConfigNum,readConfigVal;

	uint32_t readTime;
	
private:


	qik2s12v10 *motor;

};



	

#endif