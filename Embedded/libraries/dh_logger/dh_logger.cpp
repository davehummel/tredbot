#include "dh_logger.h"
#include <stdint.h>
#include "Stream.h"
#include <Arduino.h>

void Logger::setStream(Stream* in){
	stream = in;
}


bool Logger::print(uint8_t val) {
	if (byteCount > 254)
		return false;
	buffer[byteCount] = val;
	byteCount++;
	return true;
}
bool Logger::print(int8_t val) {
	if (byteCount > 254)
		return false;
	buffer[byteCount] = val;
	byteCount++;
	return true;
}
bool Logger::print(uint16_t val) {
	if (byteCount > 253)
		return false;
	buffer[byteCount] = (byte)(val & 0xff);
	byteCount++;
	buffer[byteCount] = (byte)((val >> 8) & 0xff);
	byteCount++;
	return true;
}
bool Logger::print(int16_t val) {
	if (byteCount > 253)
		return false;
	buffer[byteCount] = (byte)(val & 0xff);
	byteCount++;
	buffer[byteCount] = (byte)((val >> 8) & 0xff);
	byteCount++;
	return true;
}
bool Logger::print(uint32_t val) {
	if (byteCount > 251)
		return false;
	buffer[byteCount] = (byte)(val & 0xff);
	byteCount++;
	buffer[byteCount] = (byte)((val >> 8) & 0xff);
	byteCount++;
	buffer[byteCount] = (byte)((val >> 16) & 0xff);
	byteCount++;
	buffer[byteCount] = (byte)((val >> 24) & 0xff);
	byteCount++;
	return true;
}
bool Logger::print(int32_t val) {
	if (byteCount > 251)
		return false;
	buffer[byteCount] = (byte)(val & 0xff);
	byteCount++;
	buffer[byteCount] = (byte)((val >> 8) & 0xff);
	byteCount++;
	buffer[byteCount] = (byte)((val >> 16) & 0xff);
	byteCount++;
	buffer[byteCount] = (byte)((val >> 24) & 0xff);
	byteCount++;
	return true;
}
bool Logger::print(bool val) {
	if (byteCount > 246)
		return false;
	buffer[byteCount] = val;
	byteCount++;
	return true;
}

bool Logger::print(float val) {
	return false;
}
bool Logger::print(double val) {
	return false;
}
bool Logger::print(const char text[], uint8_t len) {
	if (byteCount + len > 255)
		return false;
	memcpy(buffer + byteCount, text, len);
	byteCount += len;
	return true;
}
void Logger::setInstruction(char mod, uint16_t instID) {
	module = mod;
	id = instID;
}
void Logger::setTime(uint32_t in) {
	time = in;
}

void Logger::send() {
	buffer[0] = byteCount-1; // dont count this byte
	buffer[1] = module;
	buffer[2] = (byte)(id & 0xff);
	buffer[3] = (byte)((id >> 8) & 0xff);
	buffer[4] = (byte)(time & 0xff);
	buffer[5] = (byte)((time >> 8) & 0xff);
	buffer[6] = (byte)((time >> 16) & 0xff);
	buffer[7] = (byte)((time >> 24) & 0xff);

	stream->write(buffer, byteCount);
	byteCount = 8;
}

void Logger::sendTimeSync() {


	byte tmp[6];
	tmp[0] = 5;

	tmp[1] = 1;

	tmp[2] = (byte)(time & 0xff);

	tmp[3] = (byte)((time >> 8) & 0xff);

	tmp[4] = (byte)((time >> 16) & 0xff);

	tmp[5] = (byte)((time >> 24) & 0xff);

	stream->write(tmp, 6);
}

void Logger::sendError(const char error[], uint8_t len) {
	stream->write(len + 1);
	stream->write((uint8_t)0);
	stream->write(error, len);
}