#include "dh_arduino_logger.h";



ArduinoLogger::ArduinoLogger(uint8_t sNum){
		serialNumber = sNum;
		switch (sNum){
			case 0:
			serial = &Serial;
			break;
			case 1:
			serial = &Serial1;
		}
}

void ArduinoLogger::print(uint8_t val, uint8_t base){
	serial->print(val,base);
}

void ArduinoLogger::print(int8_t val){
	serial->print(val);
}

void ArduinoLogger::print(uint16_t val){
	serial->print(val);
}

void ArduinoLogger::print(int16_t val){
	serial->print(val);
}

void ArduinoLogger::print(uint32_t val){
	serial->print(val);
}

void ArduinoLogger::print(int32_t val){
	serial->print(val);
}

void ArduinoLogger::print(float val, uint8_t prec){
	serial->print(val,prec);
}

void ArduinoLogger::print(double val, uint8_t prec){
	serial->print(val,prec);
}

void ArduinoLogger::print(const char text[]){
	serial->print(text);
}

void ArduinoLogger::println(void){
	serial->println();
}