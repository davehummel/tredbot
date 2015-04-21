#ifndef DH_AX12A_H__
#define DH_AX12A_H__
#include "Stream.h"


class ax_12a {
public:
	ax_12a(Stream* source, uint8_t deviceCount){
		stream = source;
		Serial1.println("Servo Started!");
		devCount = deviceCount;

		errors =new uint8_t[devCount+1];
		skipCount = 0;
		waitingForRead = false;
		lastReadAddr = 0;
		readAddr = 0;
		readData = false;
		listen();
	}

	// Cant really process the response of the ping, so why bother
	// void ping(uint8_t deviceNum){
	//   execute(deviceNum,1,0,0);
	// }

	void move(uint8_t deviceNum, uint16_t pos){
	   listen();
	   uint8_t parms[] = {30,(uint8_t)(pos%256),(uint8_t)(pos>>8)};
	   execute(deviceNum,3,3, parms);
	}

	void setCompliance(uint8_t deviceNum, uint8_t margin, uint8_t slope){
		write8(deviceNum,26,margin);
		write8(deviceNum,27,margin);
		write8(deviceNum,28,slope);
		write8(deviceNum,29,slope);
	}

	void setLED(uint8_t deviceNum,bool on){
		uint8_t val = on;
		write8(deviceNum,25,val);
	}

	// void askPos(uint8_t deviceNum){
	// 	read2Bytes(deviceNum,36);
	// }

	// void askMoving(uint8_t devNum){
	// 	readByte(devNum,46);
	// }

	bool readByte(uint8_t deviceNum, uint8_t addr){
		listen();
		if (waitingForRead)
			return false;
		uint8_t parms[] = {addr,1};
		execute(deviceNum,2,2,parms);
		waitingForRead = true;
		lastReadAddr = addr;
		return true;
	}

	bool read2Bytes(uint8_t deviceNum, uint8_t addr){
		listen();
		if (waitingForRead)
			return false;
		uint8_t parms[] = {addr,2};
		execute(deviceNum,2,2,parms);
		waitingForRead = true;
		lastReadAddr = addr;
		return true;
	}

	void write8(uint8_t deviceNum, uint8_t addr, uint8_t val ){
		listen();
		uint8_t parms[] = {addr,val};
   		execute(deviceNum,3,2, parms );
	}

	void write16(uint8_t deviceNum, uint8_t addr, uint16_t val ){
		listen();
 		uint8_t parms[] = {addr,(uint8_t)(val%256),(uint8_t)(val>>8)};
   		execute(deviceNum,3,3, parms );
	}

	void clear(void){
		while(stream->available())
			stream->read();
		skipCount = 0;
	}

	bool listen(void){

		uint16_t bytes = stream->available();

		// Serial1.print("listen bytes:");
		// Serial1.print(bytes);
		// Serial1.print(" skip bytes:");
		// Serial1.println(skipCount);

		if (bytes == 0){
			return false;
		}

		// if (bytes < skipCount)
		// 	return false;

		uint8_t data[bytes];

		if (!waitingForRead){
			stream->readBytes(data,bytes);
			// Serial1.print("No read Dumping:");
			// for (int i =0 ; i < bytes ; i++){
			// 	Serial1.print(data[i]);
			// 	Serial1.print(' ');
			// }
			// Serial1.println('|');
			if (bytes>=skipCount)
				skipCount = 0;
			else 
				skipCount-=bytes;

			// Serial1.print("Remaining skip = ");
			// Serial1.println(skipCount);
			return false;
		}

		
		if (skipCount>0){
			stream->readBytes(data,skipCount);
			// Serial1.print("Skipping:");
			// for (int i =0 ; i < skipCount ; i++){
			// 	Serial1.print(data[i]);
			// 	Serial1.print(' ');
			// }
			// Serial1.println('|');
			bytes -= skipCount;
			skipCount = 0;
		}

		if (bytes<6)
			return false;

		// This is probably not possible since the buffer is only 64 bytes?
		if (bytes>256){
			bytes = 256;
		}

		stream->readBytes(data,bytes);
		// Serial1.print("Processing ");
		// for (int i =0 ; i < bytes ; i++){
		// 	Serial1.print(data[i]);
		// 	Serial1.print(' ');
		// }
		// Serial1.println('|');

		uint16_t index=0;
		while(bytes-index>=6){
		
			if (data[index++]!=255){
			//	Serial1.println("Failed ParseBad Header 1");
				continue;
			}

			if (data[index++]!=255){
			//	Serial1.println("Failed Parse:Bad Header 2");
				continue;
			}

			uint8_t deviceNum = data[index++];
		

			uint8_t plength = data[index++];

			// Serial1.print("Device = ");
			// Serial1.print(deviceNum);
			// Serial1.print(" ");
			// Serial1.print(" plength = ");
			// Serial1.println(plength);
		
			if (plength+index>bytes){
				Serial1.println("Failed Parse:Incomplete message");
				break;
			}

			if (plength==2){
				uint8_t error = data[index++];
				if (error!=0){
					errors[deviceNum] = error;
					Serial1.print("!!Servo #");
					Serial1.print(deviceNum);
					Serial1.print(" reported error:");
					Serial1.print(error);
					Serial.println("!!");
					index++;
					newError = true;
					continue;
				}
			//	Serial1.println("Parsed Empty Message");
				index++;
				continue;
			}

			if (plength == 3){ // TODO we really should track the last command sent so we know its really a position read
				uint8_t inst = data[index++];
				if (inst!=0){
					Serial1.println("Failed Parse:Unknown 3 length message or error");
					// index+=1;
					// continue;
				}
	
				readData = data[index++];

				// Serial1.print("Read ");
				// Serial1.print(deviceNum);
				// Serial1.print(" addr = ");
				// Serial1.print(lastReadAddr);
				// Serial1.print(" val = ");
				// Serial1.println(readData);

				readAddr = lastReadAddr;
				waitingForRead = false;

				return true;
			}

			if (plength == 4){ // TODO we really should track the last command sent so we know its really a position read
				uint8_t inst = data[index++];
				if (inst!=0 ){
					Serial1.println("Failed Parse:Unknown 4 length message or errors");
					// index+=2;
					// continue;
				}
	
				readData = data[index++];
				readData|=data[index++]<<8;
			
				// Serial1.print("Read ");
				// Serial1.print(deviceNum);
				// Serial1.print(" addr = ");
				// Serial1.print(lastReadAddr);
				// Serial1.print(" val = ");
				// Serial1.println(readData);

				readAddr = lastReadAddr;
				waitingForRead = false;

				return true;
			}
		//	Serial1.println("Failed Parse:Just starting again");
		}

		return false;
	}


uint8_t devCount;

bool waitingForRead;
uint16_t readData;
uint8_t readAddr;
uint8_t* errors;
bool newError = false;

private:

void execute(uint8_t deviceNum,uint8_t command,uint16_t parmCount, uint8_t parms[]){
	  uint8_t data[parmCount+6];
	  uint16_t checksum=0; 
	  
	  data[0]=data[1]=0xff;
	  data[2]=deviceNum;
	  checksum+=data[2];
	  data[3]=parmCount+2;
	  checksum+=data[3];
	  data[4]=command;
	  checksum+=data[4];
	  for (int i = 0; i < parmCount ; i++){
	    data[5+i] = parms[i];  
	    checksum+=parms[i];
	  }
	  
	  data[parmCount+5]=(uint8_t)(255-(checksum%256));
	  //Serial3.transmitterEnable(8);
	 // Serial3.begin(400000);
	  stream->write(data,parmCount+6);
	  stream->flush();
	  //Serial3.transmitterEnable(13);
	  //pinMode(8, OUTPUT);

	  //digitalWrite(8, LOW);

	  skipCount=parmCount+6;
	  

}

Stream* stream;
uint8_t lastReadAddr;
uint8_t skipCount;


};

#endif