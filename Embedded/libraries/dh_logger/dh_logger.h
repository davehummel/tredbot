#ifndef DH_LOGGER_H__
#define DH_LOGGER_H__
#include <stdint.h>
#include "Stream.h"
#include <Arduino.h>

#define HEADER_SIZE 11

enum ADDRTYPE
{
	A_BYTE,A_UINT,A_INT,A_TIME,A_LONG,A_FLOAT,A_DOUBLE,A_STRING
};



class ADDR1{
public:
	static uint16_t solveADDR(const char* name){
		uint addr = 0;
		uint16_t mult = 1;
		for (uint8_t i = 0; i < 3 ; i++){
			if (name[i] < 'A' ){
				continue;
			}else if (name[i] <= 'Z'){
				addr+= (name[i] - 'A')*mult;
			}else if (name[i] >= 'a' && name [i] <= 'z'){
				addr+= (name[i] - 'a')*mult;
			}
			mult*=26;
		}
		return addr;
	}
	static bool parseType(ADDRTYPE &type,char val){
		switch (val){
			case 'B': type = A_BYTE;return true;
			case 'U': type = A_UINT;return true;
			case 'I': type = A_INT;return true;
			case 'L': type = A_LONG;return true;
			case 'F': type = A_FLOAT;return true;
			case 'D': type = A_DOUBLE;return true;
			case 'T': type = A_TIME;return true;
			case 'S': type = A_STRING;return true;
		}
		return false;
	}

	ADDR1(){

	}

	ADDR1(const char* name , ADDRTYPE intype){
		addr = 0;
		addr = solveADDR(name);
		type = intype;
	} 

	ADDR1(uint16_t &offset, const char* text){
		if (! parseType(type,text[offset])){
			type = A_BYTE;
		}
		offset++;
		modID = text[offset];
		if (modID<'A' || modID > 'Z')
			modID = 'A';
		offset+=2;
		addr = 0;
		uint16_t mult = 1;
		for (uint8_t i = 0; i < 3 ; i++){
			char c = text[i+offset] ;
			if ( c < 'A' ){
				continue;
			}else if (c <= 'Z'){
				addr+= (c- 'A')*mult;
			}else if (c>= 'a' && c <= 'z'){
				addr+= (c- 'a')*mult;
			}
			mult*=26;
		}
		offset+=3;

	} 

	void getChars(char* chars){
		uint16_t temp = addr;
		chars[0] = 'A'+ temp%26;
		temp/=26;
		chars[1] = 'A' +temp%26;
		temp/=26;
		chars[2] = 'A' +temp%26;
	}

	uint16_t addr = 0;
	ADDRTYPE type = A_BYTE;
	char modID = 0;
};

class Logger{
public:
	bool print(int8_t val);
	bool print(uint8_t val);
	bool print(uint16_t val);
	bool print(int16_t val);
	bool print(uint32_t val);
	bool print(int32_t val);
	bool print(bool val);

	bool print(float val);
	bool print(double val);
	bool print(const char text[], uint8_t len);

	bool startStreamSend(uint16_t sendSize, uint32_t timein, char mod, uint32_t instID);
	bool startBatchSend(uint32_t timein, char mod, uint32_t instID);
	uint16_t streamSend();
	bool batchSend();
	void sendError(const char error[], uint8_t len);
	void sendTimeSync();
	void setStream(Stream* in);
	void abortSend();

private:
	bool validate(uint8_t size);
	void flushBuffer();
	void setHeader(uint16_t otherbytes);
	Stream* stream;
	uint8_t byteCount = HEADER_SIZE;
	uint16_t streamRemainder;
	char module;
	uint32_t id;
	uint32_t time;
	char buffer[255];
	bool inStreamSend = false;
	bool inBatchSend = false;
};


#endif