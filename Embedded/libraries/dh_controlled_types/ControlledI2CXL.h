#ifndef DH_CONTROLLEDI2CXL_H__
#define DH_CONTROLLEDI2CXL_H__
#include "dh_controller.h"
#include <i2c_t3.h>

//The Arduino Wire library uses the 7-bit version of the address, so the code example uses 0x70 instead of the 8‑bit 0xE0
#define SensorAddress1 byte(0x70)
#define SensorAddress2 byte(0x71)
//The Sensor ranging command has a value of 0x51
#define RangeCommand byte(0x51)
//These are the two commands that need to be sent in sequence to change the sensor address
#define ChangeAddressCommand1 byte(0xAA)
#define ChangeAddressCommand2 byte(0xA5)


#define A_OFFSET 3
#define B_OFFSET 3

class ControlledI2CXL: public Controller::Controlled{
public:

	void begin(void){
		Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100 );
	}

	void startScan(uint32_t id, uint16_t scanDelay, uint16_t scanInt,uint8_t stereoDelay, uint32_t scanCount){

		if (scanning){
			stopScan();
		}

		scanInterval = scanInt;

		controller->schedule(id,scanDelay,scanInterval,false,scanCount,Controller::newString("SA"),'S',false);
		controller->schedule(id+1,scanDelay+stereoDelay,false,scanInterval,scanCount,Controller::newString("SB"),'S',false);
		scanningID = id;
		scanning = true;
	}

	void stopScan(){
		if (scanning){
			controller->kill(scanningID);
			controller->kill(scanningID+1);
			scanning = false;
		}
	}

	void execute(uint32_t time,uint32_t id,char* command){
		// Serial1.print(time);
		// Serial1.println(command);
		uint16_t scanInt;
		uint32_t scanCount;
		uint16_t scanDelay;
		uint8_t stereoDel;
		uint16_t pointer = 6;
		uint8_t addr;
		switch (command[0]){
			case 'B':
		
			if (!Controller::parse_uint16(scanDelay,pointer,command)){
				return;
			}

			pointer++;
	
			if (!Controller::parse_uint16(scanInt,pointer,command)){
				return;
			}

			pointer++;

			if (!Controller::parse_uint8(stereoDel,pointer,command)){
				return;
			}

			pointer++;

			if (!Controller::parse_uint32(scanCount,pointer,command)){
				return;
			}

			if (command[pointer]!='\0' && command[pointer+1]=='M')
				mute = true;
			else 
				mute = false;

			startScan(id,scanDelay,scanInt,stereoDel,scanCount);
			
			break;
			case 'E':
				stopScan();
				break;
			case 'S':
				addr = command[1]=='A'?SensorAddress1:SensorAddress2;

			 	Wire.beginTransmission(addr);             //Start addressing 
			    Wire.write(RangeCommand);                             //send range command 
			    Wire.endTransmission();     
			    if (command[1]=='A') // Only use the first scan as the trigger for the read task
			    	controller->schedule(id+2,scanInterval-1,0,false,1,Controller::newString("READ"),'S',!mute);
			break;
			case 'R':

				Wire.requestFrom(SensorAddress1, byte(2));
		        if(Wire.available() >= 2){                            //Sensor responded with the two bytes 
		            byte HighByte = Wire.read();                        //Read the high byte back 
		            byte LowByte = Wire.read();                        //Read the low byte back 
		            readingA  = word(HighByte, LowByte)+A_OFFSET;         //Make a 16-bit word out of the two bytes for the range  
		         }else {
		        	readingA = 0;
		         }  

		        Wire.requestFrom(SensorAddress2, byte(2));
		        if(Wire.available() >= 2){                            //Sensor responded with the two bytes 
		            byte HighByte = Wire.read();                        //Read the high byte back 
		            byte LowByte = Wire.read();                        //Read the low byte back 
		            readingB = word(HighByte, LowByte)+B_OFFSET;         //Make a 16-bit word out of the two bytes for the range  
		         }else {
		        	readingB = 0;
		         }    
			break;
		}
	}

	void serialize(Stream* output, uint32_t id, char command[]){
		if (command[0]=='R'){
			Serial1.print('<');
			Serial1.print(id);
			Serial1.print(':');
				Serial1.print(readingA);
				Serial1.print('|');
				Serial1.println(readingB);
		}
	}
	void startSchedule(char command[], uint32_t id){
		
	}
	void endSchedule(char command[], uint32_t id){
	
	}

	uint16_t readingA;
	uint16_t readingB;
	bool scanning = false;
	bool mute = false;


private:
	//uint8_t retryCount;
	uint32_t scanningID;
	uint16_t scanInterval;


};

	

#endif