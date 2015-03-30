#include "dh_controller.h"
#include <vector>
 #include <Arduino.h>
 #include "Stream.h"
using namespace std;
void Controller::schedule(uint32_t id, uint16_t executeInterval,uint16_t serializeInterval, uint16_t runCount, char command[],char controlled,bool serializeOnComplete){

	Entry entry;
	entry.id = id;
	entry.command = command;
	entry.controlled = library[controlled-'A'];;
	entry.runCount = runCount;
	entry.executeInterval = executeInterval;
	entry.serializeInterval = serializeInterval;
	entry.serializeOnComplete = serializeOnComplete;

	controlled->controller = this;


	timedCache.push_back(entry); 
}

void Controller::run(uint32_t id, char command[],uint8_t controlled,bool serializeOnComplete){
	if (!immediate){
		immediate = new vector<Entry>();
	}

	Entry entry;
	entry.id = id;
	entry.command = command;
	entry.controlled = library[controlled-'A'];
	entry.serializeOnComplete = serializeOnComplete;

	controlled->controller = this;

	immediate->push_back(entry); 
}

void Controller::execute(uint32_t time,Stream* output){
	// First check immediate run queue
	if (immediate && immediate->size()>0){
		for (vector<Entry>::iterator iter=immediate->begin();iter!=immediate->end();iter++){
			iter->controlled->execute(time,iter->command,iter->id);
			if (iter->serializeOnComplete){
				iter->controlled->serialize(output,iter->id,iter->command));
			}
			iter->controlled->endSchedule(iter->command,iter->id);
		}
	}

	if (immediate)
		immediate->clear();


	if (lastProcessedMSTime == time)
		return;

	lastProcessedMSTime = time;

	if (timedCache.size()>0){
		if (!timed)
			timed = new vector<Entry>();
		for (vector<Entry>::iterator iter=timedCache.begin();iter!=timedCache.end();iter++){
			Entry entry;
			entry.id = iter->id;
			entry.command = iter->command;
			entry.controlled = iter->controlled;
			entry.runCount = iter->runCount;
			entry.executeInterval = iter->executeInterval;
			entry.serializeInterval = iter->serializeInterval;
			entry.serializeOnComplete = iter->serializeOnComplete;
			entry.controlled->startSchedule(entry.command, entry.id);
			timed->push_back(entry); 
		}
		timedCache.clear();
	}

	if (!timed || timed->size()==0)
		return;

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
							iter->controlled->serialize(output,iter->id,iter->command));
						}
						timed->erase(iter);
						continue;
					}
				}
			}

			if (iter->serializeInterval == 0)
				continue;

			while (time > iter->nextSerializeTime){
				iter->controlled->serialize(output,iter->id,iter->command));
				iter->nextSerializeTime+=iter->serializeInterval;
			}
		
	}
	
	return;
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

void Controller::processInput(Stream* stream){
	while (stream->available()){
		char next = stream->read();
		if (next == '\n'||next == '\r'){
			parseBuffer();
			bufferCount = 0;
		}else{
			if (bufferCount == 255){
				bufferCount = 0;
			}else{
				bufferCount++;
			}
			inputbuffer[bufferCount] = next;
		}
	}
	
}

void Controller::parseBuffer(){
	if (bufferCount<6)
		return;
	if (inputbuffer[0]!='C'||inputbuffer[1]!=' '||inputbuffer[3]!=' '){
		return;
	}
	uint8_t commandID;
	if (inputbuffer[2]>='a' && inputbuffer[2]<='z')
		commandID = inputbuffer[2]-'a';
	else if (inputbuffer[2]>='A' && inputbuffer[2]<='Z')
		commandID = inputbuffer[2]-'A';
	else return;
	
	//todo parse id;
	
	char rawID[16];
	uint32_t id = 0;
	int offset = 4;
	for (int i = 0;i<17;i++){
		if (i==16)
			return; // bad id .. too many characters
		if (inputbuffer[offset]==' '){
			uint8_t multiplier = 1;
			for (int j = 0;j< i;j++){
				id += (rawID[i]-'0') * multiplier;
				multiplier*=10;
			}				
			break;
		}
		if (inputbuffer[offset]>='0' && inputbuffer[offset]<='9'){
			rawID[i] = inputbuffer[offset];
		}else{
			return; // non number character in id
		}
		offset++;
	}
	
	char command[bufferCount-offset+1];
	command[bufferCount-offset]='\0';
	
	for (int i = 0; i < bufferCount-offset; i++){
		command[i] = inputbuffer[i+offset];
	}
	
	run(id,command,library[commandID],true);
	
}