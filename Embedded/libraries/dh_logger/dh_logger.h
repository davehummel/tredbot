#ifndef DH_LOGGER_H__
#define DH_LOGGER_H__
#include <stdint.h>
#include "Stream.h"
#include <Arduino.h>

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

	void setInstruction(char module,uint16_t id);
	void setTime(uint32_t time);
	void send();
	void sendError(const char error[], uint8_t len);
	void sendTimeSync();
	void setStream(Stream* in);

private:
	Stream* stream;
	uint8_t byteCount = 8;
	char module;
	uint16_t id;
	uint32_t time;
	char buffer[255];
};


#endif