#include "dh_controller.h"
#include <vector>
 #include <Arduino.h>
using namespace std;
void Controller::schedule(uint32_t id, uint16_t executeInterval,uint16_t serializeInterval, uint16_t runCount, char command[],Controlled* controlled,bool serializeOnComplete){

	if (timed){
	for (vector<Entry>::iterator iter=timed->begin();iter!=timed->end();iter++){
		if (iter->id == id)
			timed->erase(iter);
		}
	}else{
		timed = new vector<Entry>();
	}

	Entry entry;
	entry.id = id;
	entry.command = command;
	entry.controlled = controlled;
	entry.runCount = runCount;
	entry.executeInterval = executeInterval;
	entry.serializeInterval = serializeInterval;
	entry.serializeOnComplete = serializeOnComplete;

	controlled->controller = this;

	controlled->startSchedule(command, id);

	timed->push_back(entry); 
}

void Controller::run(uint32_t id, char command[],Controlled* controlled,bool serializeOnComplete){
	if (!immediate){
		immediate = new vector<Entry>();
	}

	Entry entry;
	entry.id = id;
	entry.command = command;
	entry.controlled = controlled;
	entry.serializeOnComplete = serializeOnComplete;

	controlled->controller = this;

	immediate->push_back(entry); 
}

vector<Controller::ControlledResponse>* Controller::execute(uint32_t time){
	Serial1.println("HEre1");
	// First check immediate run queue
	vector<ControlledResponse>* responses = 0;
	if (immediate && immediate->size()>0){
		for (vector<Entry>::iterator iter=immediate->begin();iter!=immediate->end();iter++){
			iter->controlled->execute(time,iter->command,iter->id);
			if (iter->serializeOnComplete){
				responses = publishResponse(responses,iter->id, iter->controlled->serialize(iter->command,iter->id));
			}
			iter->controlled->endSchedule(iter->command,iter->id);
		}
	}

	if (immediate)
		immediate->clear();

	if (!timed || timed->size()==0)
		return responses;

	if (lastProcessedMSTime == time)
		return responses;

	lastProcessedMSTime = time;

	Serial1.println("HEre2");
	Serial1.flush();

	for (vector<Entry>::iterator iter=timed->begin();iter!=timed->end();iter++){
			while (time > iter->nextExecuteTime){
				if (iter->nextExecuteTime == 0){
					if (iter->executeInterval == 0)
						break;
					iter->nextExecuteTime = time+iter->executeInterval;
				}else{
					iter->nextExecuteTime += iter->executeInterval;
				}
				iter->controlled->execute(time,iter->command,iter->id);
				if (iter->runCount>0){
					iter->runCount--;
					if (iter->runCount == 0){
						if (iter->serializeOnComplete || (time > iter->nextSerializeTime && iter->serializeInterval>0) ){
							responses = publishResponse(responses,iter->id, iter->controlled->serialize(iter->command,iter->id));
						}
						timed->erase(iter);
						continue;
					}
				}
			}

			if (iter->serializeInterval == 0)
				continue;

			while (time > iter->nextSerializeTime){
				responses = publishResponse(responses,iter->id, iter->controlled->serialize(iter->command,iter->id));
				iter->nextSerializeTime+=iter->serializeInterval;
			}
		
	}
	
	return responses;
}

vector<Controller::ControlledResponse>* Controller::publishResponse(vector<ControlledResponse>* responses,uint32_t id, char* content){
	if (!responses){
			responses = new vector<ControlledResponse>();
		}
	ControlledResponse response;
	response.content =content;
	response.id = id;
	responses->push_back(response);
	return responses;
}
