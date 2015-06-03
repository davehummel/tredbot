#ifndef DH_CONTROLLEDLIDAR_H__
#define DH_CONTROLLEDLIDAR_H__
#include "dh_controller.h"
#include <i2c_t3.h>


#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
#define    LIDARPIN            22

class ControlledLidar: public Controller::Controlled{
public:

	void begin(void){
		Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100 );
		pinMode(LIDARPIN, OUTPUT);
		digitalWrite(LIDARPIN, LOW);
	}

	void startScan(uint32_t id, uint16_t scanDelay, uint16_t scanInt, uint16_t scanCount, bool muting){
		scanInterval = scanInt;
		mute = muting;
		controller->schedule(id,scanDelay,scanInterval,false,scanCount,Controller::newString("SCAN"),'L',muting);
	}

	void execute(uint32_t time,uint32_t id,char command[], bool serializeOnComplete){

		uint8_t nackack = 0;
		uint16_t scanCount;
		uint16_t scanDelay;
		uint16_t pointer = 6;
		switch (command[0]){
			case 'B':
			if (!Controller::parse_uint16(scanDelay,pointer,command)){
				return;
			}

			pointer++;
	
			if (!Controller::parse_uint16(scanInterval,pointer,command)){
				return;
			}

			pointer++;

			if (!Controller::parse_uint16(scanCount,pointer,command)){
				return;
			}

			if (command[pointer]!='\0' && command[pointer+1]=='M')
				mute = true;
			else 
				mute = false;



			startScan(id,scanDelay,scanInterval,scanCount,mute);
			

			break;
			case 'O':
			 	digitalWrite(LIDARPIN, LOW);
			 break;	
			case 'S':
			 	Wire.beginTransmission(LIDARLite_ADDRESS);
			    Wire.write(RegisterMeasure);
			    Wire.write(MeasureValue);
			   
			    nackack = Wire.endTransmission(I2C_STOP,1000);
		
			    if (nackack!=0){
			    	Serial1.println("Lidar Range Failed!");
			    }
			     controller->schedule(id+1,scanInterval-1,0,false,1,Controller::newString("READ"),'L',!mute);
			break;
			case 'R':

			    lastReadTime = time;
				lastRead = 0;
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



				 lastRead = reading;

				// correct(lastRead);
				
			break;
		}
	}

	void serialize(Stream* output, uint32_t id, char command[]){
		if (command[0]=='R'){
			Serial1.print('<');
			Serial1.print(id);
			Serial1.print('@');
			Serial1.print(lastReadTime);
			Serial1.print(':');
	
			Serial1.println(lastRead);

		}
	}
	void startSchedule(char command[], uint32_t id){
		if (command[0]=='S'){
			digitalWrite(LIDARPIN, HIGH);
		}
	}
	void endSchedule(char command[], uint32_t id){
		if (command[0]=='S'){
			controller->schedule(id,scanInterval,0,false,1,Controller::newString("OFF"),'L',false);
		}

	}

	uint16_t lastRead;
	uint32_t lastReadTime;
private:
	uint16_t scanInterval;

	bool mute = false;

	void correct(uint16_t &data){
		if (data>50)
			data-=4;
		else if (data>40)
			data-=5;
		else if (data>30)
			data-=6;
		else if (data>20)
			data-=7;
		else 
			data-=8;
	}
};

	

#endif