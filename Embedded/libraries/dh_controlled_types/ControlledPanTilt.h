#ifndef DH_CONTROLLEDPAN_H__
#define DH_CONTROLLEDPAN_H__

#include "dh_controller.h"
#include <dh_ax-12a.h>
#include <ControlledLidar.h>
#include <ControlledI2CXL.h>

class ControlledPanTilt: public Controller::Controlled{
public:
	const uint16_t INITDELAY = 50;

	void begin(void){
		 Serial3.begin(400000); // change this to 1000000 if you havent changed default baud
		 if (!servo){
		 	servo = new ax_12a(&Serial3,2);
		 }
		 pinMode(3, OUTPUT);
		digitalWrite(3, LOW);
	}

	void execute(uint32_t time,uint32_t id,char command[]){
		uint16_t pointer = 0;

		switch (command[0]){
			case 'B':
				if (command[1]=='X'){
						if (runProgram){
							controller->schedule(id,INITDELAY,0,false,1,Controller::newString("EXEC"),'S',false);
						}
						controller->schedule(id,INITDELAY+stepDuration/4,stepDuration,false,steps*attempts,Controller::newString("ASK"),'S',false);
						controller->schedule(id,INITDELAY-40,0,false,1,Controller::newString("PUSH"),'S',false);
						controller->schedule(id,INITDELAY+stepDuration-stepDuration/4,stepDuration,false,steps*attempts,Controller::newString("SCAN"),'S',true);
					break;
				}
				pointer = 6;

				digitalWrite(3, HIGH);

				if (!Controller::parse_uint16(steps,pointer,command))
				 	return;
				 
				pointer++;

				if (!Controller::parse_uint16(stepDuration,pointer,command))
				 	return;

				pointer++;

				if (!Controller::parse_uint16(min,pointer,command))
				 	return;

				pointer++;

				if (!Controller::parse_uint16(max,pointer,command))
				 	return;

				pointer++;

				if (!Controller::parse_uint8(attempts,pointer,command))
				 	return;

				pointer++;

				if (!Controller::parse_uint8(programNum,pointer,command)){
					runProgram = false;
				}else{
					runProgram = true;
				}
				 
				
				pos = min;


				curAttempt = 0;
				currentStep = 0;


				position = 0;
				readtime = 0;
				goal = pos;

				firstDelay = true;

				controller->schedule(id+1,100,0,false,0,Controller::newString("MOVE 1"),'S',false);

				controller->schedule(id,400,100,false,50,Controller::newString("D 1 BX"),'S',false);
			
			break;
			case 'D':
				{
					pointer = 2;
					uint8_t wid;
					if (!Controller::parse_uint8(wid,pointer,command))
					 	return;

					servo->listen();

					if (firstDelay){
						Serial1.print('.');
						servo->clear();
						servo->waitingForRead = false;
						firstDelay = false;
						servo->readByte(wid,46);
						return;
					}

					if (servo->waitingForRead){
						Serial1.print("-");
						servo->waitingForRead= false;
						servo->readByte(wid,46);
						return;
					}

					if (servo->readAddr!=46){
						Serial1.println("Delay read is getting wrong addr????");
						servo->readByte(wid,46);
						return;
					}

					if (servo->readData == 1){
							servo->readByte(wid,46);
							Serial1.print("+");
						break;
					}

				
					

					pointer++;
					uint8_t start = pointer;
					for ( ; pointer < 256; pointer++){
					 	if (command[pointer]=='\0')
					 		break;
					}
					char* newCommand = new char[pointer-start];
					for (int i = 0; start<=pointer ; i++){
						newCommand[i] = command[start];
						start++;
					}
					controller->kill(); // kill self
					Serial1.println("Finished MOVE!!!");
					controller->run(id,newCommand,'S',true);
				}
			break;
			case 'E':
				controller->runProgram(programNum);
				break;
			case 'M':
				{
					pointer = 5;
					uint8_t sID;
					if (!Controller::parse_uint8(sID,pointer,command))
					 	return;
					if (command[pointer]=='\0'){
						servo->move(sID,pos);

						servo->clear();
						return;
					}
					pointer++;
					uint16_t newPos;
					if (!Controller::parse_uint16(newPos,pointer,command))
					 	return;
					 
					servo->move(sID,newPos);
				}	
			break;
			case 'A':
				servo->read2Bytes(1,36);
				readtime=time;
				// Serial1.print('A');
				// Serial1.print(time);
				break;
			case 'P':
				servo->move(1,pos+2);
				break;
			case 'S':
			{
				// Serial1.print('S');
				// Serial1.print(time);
				servo->listen();
				if (servo->waitingForRead){
					Serial1.println("Failed to get position of servo in scan!!!");
					servo->clear();
					servo->waitingForRead=false;
				} else if (servo->readAddr != 36){
					Serial1.println("Wrong address in servo in scan???");
					servo->clear();
					servo->waitingForRead=false;
				}
				position = servo->readData;
				goal = pos;
				currentStep++;
				if (currentStep==steps){
					currentStep = 0;
					curAttempt++;
				}
				bool reverse =  curAttempt %2 == 1 ;
				if (currentStep == 0){
					pos = reverse?max:min;
				}else if (currentStep == steps-1){
					pos = reverse?min:max;
				} else {
					uint32_t middle = max-min;
					middle = middle * currentStep;
					middle = middle / (steps-1);
					if (reverse){
						pos = max - middle;
					}else{
						pos = min + middle;
					}
				}

				servo->move(1,pos);
			}
			break;
			case 'O':
			if (command[1]=='F')
				digitalWrite(3, LOW);
			else
				digitalWrite(3, HIGH);
			break;
		}
	}
	void serialize(Stream* output, uint32_t id, char command[]){
		if (command[0] == 'S'){
			Serial1.print('<');
			Serial1.print(id);
			Serial1.print(':');
				Serial1.print('G');
				Serial1.print(goal);
				Serial1.print("A");
				Serial1.print(position);
				Serial1.print('@');
				Serial1.println(readtime);
		}

	}
	void startSchedule(char command[], uint32_t id){
	
	}
	void endSchedule(char command[], uint32_t id){
	if (command[0] == 'S')
		digitalWrite(3, LOW);
	}
	
private:

	uint8_t curAttempt;
	uint8_t attempts;

	uint16_t pos;

	uint16_t min = 0;
	uint16_t max = 0;

	uint16_t steps= 0;
	uint16_t currentStep = 0;

	uint16_t stepDuration = 0;

	uint16_t goal;
	uint16_t position;
	uint32_t readtime;

	uint8_t programNum;
	bool runProgram;

	bool firstDelay;

	ax_12a* servo;
};

	

#endif