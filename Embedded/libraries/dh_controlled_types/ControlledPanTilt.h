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
		 Serial3.begin(117647); // change this to 1000000 if you havent changed default baud
		 if (!servo){
		 	servo = new ax_12a(&Serial3,2);
		 }
	}

	void execute(uint32_t time,uint32_t id,char command[]){
		uint16_t pointer = 0;

		switch (command[0]){
			case 'B':
				if (command[1]=='X'){
					Serial1.println("GO!");
					readCount = 0;
						controller->run(id+10,Controller::newString("BEGIN"),'L',false);
						controller->schedule(id,INITDELAY+10,stepDuration,false,steps*attempts,Controller::newString("ASK"),'S',false);
						controller->schedule(id,INITDELAY-40,0,false,1,Controller::newString("PUSH"),'S',false);
						controller->schedule(id,INITDELAY+stepDuration-stepDuration/4,stepDuration,false,steps*attempts,Controller::newString("SCAN"),'S',false);
					break;
				}
				pointer = 6;

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
				
				useLidar = false;
				useEcho = false;


				pointer++;


				if (command[pointer]=='2'||'3')
					useEcho = true;

				if (command[pointer]=='1'||'3')
					useLidar = true;
			

				pointer+=2;


				if (!Controller::parse_uint8(bufferSize,pointer,command)){
					return;
				}

				pos = min;

				servo->move(1,pos-5);

				curAttempt = 0;
				currentStep = 0;

				if (useLidar && lidar){
					lidar->_scanDelay = INITDELAY;
					lidar->_scanInt = stepDuration;
					lidar->_scanCount = steps*attempts;
					lidar->_dataPoints = bufferSize;
				}

				if (positions)
					delete positions;
				if (times)
					delete times;
				if (goals)
					delete goals;

				positions = new uint16_t[bufferSize];
				times = new uint32_t[bufferSize];
				goals = new uint16_t[bufferSize];

				servo->waitingForRead = false;
				servo->readByte(1,46);


				controller->schedule(id,100,100,false,50,Controller::newString("D 1 BX"),'S',false);
				
			break;
			case 'D':
				{
					pointer = 2;
					uint8_t wid;
					if (!Controller::parse_uint8(wid,pointer,command))
					 	return;

					servo->listen();

					if (servo->waitingForRead){
						Serial1.print("-");
					//	servo->waitingForRead= false;
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
					//Serial1.println("Finished MOVE!!!");
					controller->run(id,newCommand,'S',true);
				}
			break;
			case 'M':
				{
					pointer = 5;
					uint8_t sID;
					if (!Controller::parse_uint8(sID,pointer,command))
					 	return;
					 
					pointer++;
					uint16_t newPos;
					if (!Controller::parse_uint16(newPos,pointer,command))
					 	return;
					 
					servo->move(sID,newPos);
				}	
			break;
			case 'A':
				servo->read2Bytes(1,36);
				times[readCount]=time;
				// Serial1.print('A');
				// Serial1.print(time);
				break;
			case 'P':
				servo->move(1,pos);
				break;
			case 'S':
			{
				// Serial1.print('S');
				// Serial1.print(time);
				servo->listen();
				if (servo->waitingForRead){
					Serial1.println("Failed to get position of servo in scan!!!");
				} else if (servo->readAddr != 36){
					Serial1.println("Wrong address in servo in scan???");
				}
				positions[readCount] = servo->readData;
				goals[readCount] = pos;
				readCount++;
				if (readCount == bufferSize){
					controller->run(id+1,Controller::newString("DUMP"),'S',true);
				}
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

				if (pos>max || pos < min){
					Serial1.print("BLOCKING BAD POS:");
					Serial1.println(pos);
					return;
				}

				servo->move(1,pos);
			}
			break;
			case 'O':
			break;
		}
	}
	void serialize(Stream* output, uint32_t id, char command[]){
		if (command[0] == 'D'){
			Serial1.print('<');
			Serial1.print(id);
			Serial1.print(':');
			for (uint8_t i = 0 ; i < bufferSize ; i++){
				Serial1.print('G');
				Serial1.print(goals[i]);
				Serial1.print("A");
				Serial1.print(positions[i]);
				Serial1.print('@');
				Serial1.print(times[i]);
				Serial1.print(' ');
			}
			Serial1.println();
			readCount = 0;
		}
	}
	void startSchedule(char command[], uint32_t id){
	
	}
	void endSchedule(char command[], uint32_t id){

	}

	ControlledLidar* lidar;
	ControlledI2CXL* echo;
	
private:

	uint8_t curAttempt;
	uint8_t attempts;

	uint16_t pos;

	uint16_t min = 0;
	uint16_t max = 0;

	uint16_t steps= 0;
	uint16_t currentStep = 0;

	uint16_t stepDuration = 0;

	bool useLidar;
	bool useEcho;

	uint16_t* goals;
	uint16_t* positions;
	uint32_t* times;
	uint8_t readCount = 0;
	uint8_t bufferSize = 0;

	ax_12a* servo;
};

	

#endif