#ifndef DH_LOGGER_H__
#define DH_LOGGER_H__
#include <stdint.h>
class Logger{
public:
	virtual void print(uint8_t val, uint8_t base = 10)=0;
	virtual void print(int8_t val)=0;
	virtual void print(uint16_t val)=0;
	virtual void print(int16_t val)=0;
	virtual void print(uint32_t val)=0;
	virtual void print(int32_t val)=0;
	virtual void print(bool val)=0;

	virtual void print(float val, uint8_t prec=4)=0;
	virtual void print(double val, uint8_t prec=4)=0;
	virtual void print(const char text[])=0;

	virtual void println (void)=0;
};


#endif