#ifndef DH_CONTROLLER_H__
#define DH_CONTROLLER_H__

#include <vector>
#include <list>
#include <stdint.h>
#include "Stream.h"

using namespace std;

class Controller{
public:
	struct ControlledResponse{
 		uint32_t id;
 		char* content;
	};

	class Controlled{

	public:

		virtual void begin(void)=0;
		virtual void execute(uint32_t time,char command[],uint32_t id)=0;
		virtual char* serialize(char command[],uint32_t id)=0;
		virtual void startSchedule(char command[],uint32_t id)=0;
		virtual void endSchedule(char command[], uint32_t id)=0;
		
		char error[];

		Controller* controller;
		
	private:

	};


	void schedule(uint32_t id, uint16_t executeInterval,uint16_t serializeInterval, uint16_t runCount, char command[],Controlled* controlled,bool serializeOnComplete);
	
	void run(uint32_t id, char command[],Controlled *controlled,bool serializeOnComplete);
	
	vector<ControlledResponse>* execute(uint32_t time);
	
	void process(Stream* serial);
	
	Controller::Controlled* library[25];

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

	uint32_t lastProcessedMSTime;

	vector<ControlledResponse>* publishResponse(vector<ControlledResponse>* responses,uint32_t id, char* content);
	
};






#endif