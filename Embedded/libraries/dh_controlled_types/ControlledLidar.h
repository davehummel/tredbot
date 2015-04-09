#ifndef DH_CONTROLLEDLIDAR_H__
#define DH_CONTROLLEDLIDAR_H__
#include "dh_controller.h"
#include <i2c_t3.h>


#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

class ControlledLidar: public Controller::Controlled{
public:

	void begin(void){
		Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100 );
		pinMode(2, OUTPUT);
		digitalWrite(2, LOW);
	}

	void startScan(uint32_t id, uint16_t scanDelay, uint16_t scanInt,uint32_t scanCount,uint8_t dataPoints){

		scanInterval = scanInt;

		bufferSize = dataPoints;

		readCount = 0;

		if (readings)
			delete readings;


		readings = new uint16_t[bufferSize];

		controller->schedule(id,scanDelay,scanInterval,false,scanCount,Controller::newString("SCAN"),'L',false);
	}

	void execute(uint32_t time,uint32_t id,char* command){
		// Serial1.print(time);
		// Serial1.println(command);
		uint8_t nackack = 0;
		uint16_t scanInt;
		uint32_t scanCount;
		uint8_t dataPoints;
		uint16_t scanDelay;
		uint16_t pointer = 6;
		switch (command[0]){
			case 'B':
			if (command[5]=='\0'){
				startScan(id,_scanDelay,_scanInt,_scanCount,_dataPoints);
			}
			if (!Controller::parse_uint16(scanDelay,pointer,command)){
				return;
			}

			pointer++;
	
			if (!Controller::parse_uint16(scanInt,pointer,command)){
				return;
			}

			pointer++;

			if (!Controller::parse_uint32(scanCount,pointer,command)){
				return;
			}

			pointer++;

			if (!Controller::parse_uint8(dataPoints,pointer,command)){
				return;
			}

			startScan(id,scanDelay,scanInt,scanCount,dataPoints);
			
			break;
			case 'O':
			 	digitalWrite(2, LOW);
			 break;	
			case 'S':
			 	Wire.beginTransmission(LIDARLite_ADDRESS);
			    Wire.write(RegisterMeasure);
			    Wire.write(MeasureValue);
			   
			    nackack = Wire.endTransmission(I2C_STOP,1000);
		
			    if (nackack!=0){
			    	Serial1.println("Lidar Range Failed!");
			    }
			     controller->schedule(id+1,scanInterval-1,0,false,1,Controller::newString("READ"),'L',true);
			break;
			case 'R':
				readings[readCount]=0;
				readCount++;
				Wire.beginTransmission(LIDARLite_ADDRESS);
				Wire.write(RegisterHighLowB);
				nackack = Wire.endTransmission(I2C_NOSTOP,1000);
				
				if (nackack!=0){
					Serial1.println("Read Failed!");
				    return;
				}
				    
			     Wire.requestFrom(LIDARLite_ADDRESS,2,I2C_STOP,1000);
			     	
				 uint16_t reading = Wire.readByte() ;
				 reading = reading << 8;
				 reading |= Wire.readByte();

				 readings[readCount-1] = reading;	
				
			break;
		}
	}

	void serialize(Stream* output, uint32_t id, char command[]){
		if (command[0]=='R' && readCount >= bufferSize){
			Serial1.print('<');
			Serial1.print(id);
			Serial1.print(':');
			for (uint8_t i = 0 ; i < bufferSize ; i++){
				Serial1.print(readings[i]);
				Serial1.print(' ');
			}
			Serial1.println();
			readCount = 0;
		}
	}
	void startSchedule(char command[], uint32_t id){
		if (command[0]=='S'){
			digitalWrite(2, HIGH);
		}
	}
	void endSchedule(char command[], uint32_t id){
		if (command[0]=='S'){
			controller->schedule(id,scanInterval,0,false,1,Controller::newString("OFF"),'L',false);
		}

	}

	uint16_t _scanDelay;
	uint16_t _scanInt;
	uint32_t _scanCount;
	uint8_t _dataPoints;

	
private:
	//uint8_t retryCount;
	uint16_t scanInterval;
	uint16_t* readings;
	uint8_t readCount = 0;
	uint8_t bufferSize = 0;


};

	

#endif