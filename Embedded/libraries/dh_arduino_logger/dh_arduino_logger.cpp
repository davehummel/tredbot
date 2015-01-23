#include "dh_arduino_logger.h";

ArduinoLogger::ArduinoLogger(uint8_t sNum){
		serialNumber = sNum;
}

void ArduinoLogger::print(uint8_t val, uint8_t base){
	Serial.print(val,base);
}

void ArduinoLogger::print(int8_t val){
	Serial.print(val);
}

void ArduinoLogger::print(uint16_t val){
	Serial.print(val);
}

void ArduinoLogger::print(int16_t val){
	Serial.print(val);
}

void ArduinoLogger::print(uint32_t val){
	Serial.print(val);
}

void ArduinoLogger::print(int32_t val){
	Serial.print(val);
}

void ArduinoLogger::print(float val, uint8_t prec){
	Serial.print(val,prec);
}

void ArduinoLogger::print(double val, uint8_t prec){
	Serial.print(val,prec);
}

void ArduinoLogger::print(const char text[]){
	Serial.print(text);
}

void ArduinoLogger::println(void){
	Serial.println();
}