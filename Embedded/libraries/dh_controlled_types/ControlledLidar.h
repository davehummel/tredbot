#ifndef DH_CONTROLLEDLIDAR_H__
#define DH_CONTROLLEDLIDAR_H__
#include "dh_controller.h"
#include <i2c_t3.h>


#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
using namespace std;
class ControlledLidar: public Controller::Controlled{

public:

	void begin(void){
		Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100 );
	}
	void execute(uint32_t time,char command[], uint32_t id){
		 uint8_t nackack = 0;
		switch (command[0]){
			case 'S':Serial1.println("Go for it");
			 	Wire.beginTransmission(LIDARLite_ADDRESS);
			    Wire.write(RegisterMeasure);
			    Wire.write(MeasureValue);
			    nackack = Wire.endTransmission(I2C_STOP,1000);
			    if (nackack!=0){
			    	Serial1.println("Failed!");
			      return;
			    }
  				 Serial1.println("Go for it2");
			     controller->schedule(id+1,19,0,1,"R",this,true);
			 	 Serial1.println("Done S");
			 	 Serial1.flush();
			break;
			case 'R':
			Serial1.println("Go for it3");
			Serial1.flush();
				Wire.beginTransmission(LIDARLite_ADDRESS);
			    Wire.write(RegisterHighLowB);
			    nackack = Wire.endTransmission(I2C_NOSTOP,1000);
			    if (nackack!=0)
			    	return;
			     Wire.requestFrom(LIDARLite_ADDRESS,2,I2C_STOP,1000);
				 reading = Wire.readByte() ;
				 reading = reading << 8;
				 reading |= Wire.readByte();
				 Serial1.println("Go for it4");
			break;
		}
		
	}
	char* serialize(char command[], uint32_t id){
		char buffer [8];
		itoa(reading,buffer,10);
		return buffer;
	}
	void startSchedule(char command[], uint32_t id){
		
	}
	void endSchedule(char command[], uint32_t id){
		
	}
	
private:
	uint8_t retryCount;
	uint16_t reading;
	
};

	

#endif