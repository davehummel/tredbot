#ifndef DH_CONTROLLER_H__
#define DH_CONTROLLER_H__

#include <vector>
#include <list>
#include <stdint.h>
#include "Stream.h"

using namespace std;

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


	void schedule(uint32_t id, uint16_t executeInterval,uint16_t serializeInterval, uint16_t runCount, char command[],char controlled,bool serializeOnComplete);
	
	void run(uint32_t id, char command[],uint8_t controlled,bool serializeOnComplete);
	
	void execute(uint32_t time,Stream* output);
	
	void processInput(Stream* serial);
	
	Controller::Controlled* library[26];

private:

	struct Entry {
		uint32_t id;
		char* command;
		Controlled *controlled;
		bool serializeOnComplete;
		uint16_t executeInterval;
		uint16_t serializeInterval;
		uint16_t runCount;
		uint32_t nextExecuteTime=0;
		uint32_t nextSerializeTime=0;
	};
	


	vector<Entry>* timed;
	vector<Entry>* immediate;
	vector<Entry> timedCache;
	
	char inputbuffer[256];
	uint8_t bufferCount;

	uint32_t lastProcessedMSTime;
	
	void parseBuffer();
	
};






#endif