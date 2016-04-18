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
	memcpy(buffer+byteCount,&val,1);
	byteCount++;
	return true;
}
bool Logger::print(int8_t val) {
	if (!validate(1))
		return false;
		memcpy(buffer+byteCount,&val,1);
		byteCount++;
	return true;
}
bool Logger::print(uint16_t val) {
	if (!validate(2))
		return false;
		memcpy(buffer+byteCount,&val,2);
		byteCount+=2;
	return true;
}
bool Logger::print(int16_t val) {
	if (!validate(2))
		return false;
		memcpy(buffer+byteCount,&val,2);
		byteCount+=2;
	return true;
}
bool Logger::print(uint32_t val) {
	if (!validate(4))
		return false;
		memcpy(buffer+byteCount,&val,4);
		byteCount+=4;
	return true;
}
bool Logger::print(int32_t val) {
	if (!validate(4))
		return false;
		memcpy(buffer+byteCount,&val,4);
		byteCount+=4;
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
	if (!validate(4))
		return false;
	memcpy(buffer+byteCount,&val,4);
	byteCount+=4;
	return true;
}
bool Logger::print(double val) {
	if (!validate(8))
		return false;
	memcpy(buffer+byteCount,&val,8);
	byteCount+=8;
	return true;
}
bool Logger::print(const char text[], uint8_t len) {
	if (!validate(len))
		return false;
		memcpy(buffer+byteCount,text,len);
		byteCount+=len;
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

void Logger::sendTimeSync(uint32_t time) {
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

bool Logger::startBatchSend( char mod, uint32_t instID){
	if (inStreamSend || inBatchSend)
		return false;

	module = mod;
	id = instID;
	byteCount = HEADER_SIZE;
	inBatchSend = true;
	return true;
}

bool Logger::startStreamSend(uint16_t sendSize, char mod, uint32_t instID) {
	if (inStreamSend || inBatchSend)
		return false;
	module = mod;
	id = instID;

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

void ErrorLogger::clearError(){
	errBufCount = 0;
	errorCode = NONE;
	errorTime = 0;
	errorComplete = false;
}

char* ErrorLogger::getErrorText(){
	errorBuffer[errBufCount] = '\0';
	return errorBuffer;
}

uint32_t ErrorLogger::getErrorTime(){
	return errorTime;
}

ErrorLogger::ERROR_CODE ErrorLogger::getErrorCode(){
	return errorCode;
}

size_t ErrorLogger::write(uint8_t ch){
	if (errorComplete)
		return 0;
	if (errBufCount == ERR_BUFF-1){
		return 0;
	}
	errorBuffer[errBufCount] = ch;
	errBufCount++;
	return 1;
}

void ErrorLogger::finished(uint32_t time, ERROR_CODE code){
	errorComplete = true;
	errorTime = time;
	errorCode = code;
}
