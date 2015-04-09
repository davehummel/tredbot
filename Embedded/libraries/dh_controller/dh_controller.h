#ifndef DH_CONTROLLER_H__
#define DH_CONTROLLER_H__

#include <stdint.h>
#include "Stream.h"
#include <Arduino.h>

class Controller{
public:

	class Controlled{

	public:

		virtual void begin(void)=0;
		virtual void execute(uint32_t time,uint32_t id,char command[])=0;
		virtual void serialize(Stream* output,uint32_t id,char command[])=0;
		virtual void startSchedule(char command[],uint32_t id)=0;
		virtual void endSchedule(char command[], uint32_t id)=0;
		
		char error[];

		Controller* controller;
		
	private:  

	};


	void schedule(uint32_t id, uint16_t initialExecDelay, uint16_t executeInterval,bool additiveInterval, uint32_t runCount,char command[],char controlled,bool serializeOnComplete);
	
	void run(uint32_t id,char command[],uint8_t controlled,bool serializeOnComplete);
	
	void execute(Stream* output);

	void kill(uint32_t id);

	void kill(void);
	
	void processInput(Stream* serial);

	static bool parse_uint8(uint8_t &val, uint16_t &pointer,char* text);

	static bool parse_uint16(uint16_t &val,uint16_t &pointer,char* text);

	static bool parse_int16(int16_t &val,uint16_t &pointer,char* text);

	static bool parse_uint32(uint32_t &val,uint16_t &pointer,char* text);

	static char* newString(const char original[]);
	
	Controller::Controlled* library[26];

private:

	struct Entry {
		uint32_t id;
		char* command;
		Controlled *controlled;
		bool serializeOnComplete;
		uint16_t executeInterval;
		uint32_t runCount;
		uint32_t nextExecuteTime;
		uint32_t additiveInterval;
		bool killed;
	};

	void addTimedEntry(Entry* entry);
	
	Entry* currentlyRunning;
	bool isANext = true;
	Entry* timedA[256];
	Entry* timedB[256];
	Entry* immediate[256];

	uint8_t immediateSize = 0;
	uint8_t timedSize = 0;
	
	char inputbuffer[256];
	uint8_t bufferCount;

	uint32_t lastProcessedMSTime;

	elapsedMillis millis;
	
	void parseBuffer();


	
};



#endif