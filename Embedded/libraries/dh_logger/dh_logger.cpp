#include "dh_logger.h"
#include <stdint.h>
#include "Stream.h"
#include <Arduino.h>



void Logger::setStream(Stream* in){
	stream = in;
}


bool Logger::print(uint8_t val) {
	if (!validate(1))
		return false;
	buffer[byteCount] = val;
	byteCount++;
	return true;
}
bool Logger::print(int8_t val) {
	if (!validate(1))
		return false;
	buffer[byteCount] = val;
	byteCount++;
	return true;
}
bool Logger::print(uint16_t val) {
	if (!validate(2))
		return false;
	buffer[byteCount] = (byte)(val & 0xff);
	byteCount++;
	buffer[byteCount] = (byte)((val >> 8) & 0xff);
	byteCount++;
	return true;
}
bool Logger::print(int16_t val) {
	if (!validate(2))
		return false;
	buffer[byteCount] = (byte)(val & 0xff);
	byteCount++;
	buffer[byteCount] = (byte)((val >> 8) & 0xff);
	byteCount++;
	return true;
}
bool Logger::print(uint32_t val) {
	if (!validate(4))
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
	if (!validate(4))
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
	if (!validate(1))
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
	if (!validate(len))
		return false;
	memcpy(buffer + byteCount, text, len);
	byteCount += len;
	return true;
}

bool Logger::batchSend() {
	if (!inBatchSend)
		return false;
	setHeader(byteCount-2);
	flushBuffer();
	inBatchSend = false;
	return true;
}

void Logger::sendTimeSync() {
	if (inStreamSend || inBatchSend)
		return;

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
	if (inStreamSend || inBatchSend)
		return;
	stream->write(len + 1);
	stream->write((uint8_t)0);
	stream->write(error, len);
}

bool Logger::startBatchSend(uint32_t timein, char mod, uint32_t instID){
	if (inStreamSend || inBatchSend)
		return false;
	time = timein;
	module = mod;
	id = instID;
	byteCount = HEADER_SIZE;
	inBatchSend = true;
	return true;
}

bool Logger::startStreamSend(uint16_t sendSize, uint32_t timein, char mod, uint32_t instID) {
	if (inStreamSend || inBatchSend)
		return false;
	module = mod;
	id = instID;
	time = timein;
	byteCount = HEADER_SIZE;
	setHeader(sendSize+HEADER_SIZE-2);
	inStreamSend = true;
	streamRemainder = sendSize;
	return true;
}

uint16_t Logger::streamSend() {
	if (!inStreamSend)
		return 999;
	if (streamRemainder > 0)
		return streamRemainder;
	flushBuffer();
	inStreamSend = false;
	return 0;
}

bool Logger::validate(uint8_t size) {
	if (!inStreamSend && !inBatchSend)
		return false;

	if (255 - byteCount < size) {
		if (inStreamSend) {
			flushBuffer();
		}
		else {
			return false;
		}
	}
	if (inStreamSend) {
		if (streamRemainder < size)
			return false;
		else
			streamRemainder -= size;
	}
	return true;
}

void Logger::setHeader(uint16_t length) {
	buffer[0] = (byte)(length & 0xff);
	buffer[1] = (byte)((length >> 8) & 0xff);
	buffer[2] = (byte)module;
	buffer[3] = (byte)(id & 0xff);
	buffer[4] = (byte)((id >> 8) & 0xff);
	buffer[5] = (byte)((id >> 16) & 0xff);
	buffer[6] = (byte)((id >> 24) & 0xff);
	buffer[7] = (byte)(time & 0xff);
	buffer[8] = (byte)((time >> 8) & 0xff);
	buffer[9] = (byte)((time >> 16) & 0xff);
	buffer[10] = (byte)((time >> 24) & 0xff);
}
void Logger::flushBuffer() {
	stream->write(buffer, byteCount);
	byteCount = 0;
}

void Logger::abortSend(){
	if (inBatchSend){
		inBatchSend = false;
		return;
	}
	if (inStreamSend){
		flushBuffer();
		for (uint16_t i = 0; i < streamRemainder; i++){
			stream->write((byte)0);
		}
		inStreamSend = false;
		return;
	}
}