#include "dh_controller.h"
#include "dh_controlled.h"
#include <vector>
using namespace std;
void Controller::schedule(uint32_t id, uint16_t executeInterval,uint16_t serializeInterval, uint16_t runCount, char command[],Controlled* controlled,bool serializeOnComplete){
	for (vector<Entry>::iterator iter=entries.begin();iter!=entries.end();iter++){
		if (iter->id == id)
			entries.erase(iter);
	}
	Entry entry;
	entry.id = id;
	entry.command = command;
	entry.controlled = controlled;
	entry.runCount = runCount;
	entry.executeInterval = executeInterval;
	entry.serializeInterval = serializeInterval;

	entries.push(entry);
}

void Controller::run(uint32_t id, char command[],Controlled* controlled,bool serializeOnComplete){
	schedule(id,0,0,0,command,controlled,serializeOnComplete);
}

vector<Controller::ControlledResponse>* Controller::execute(uint32_t time){
	vector<Entry> finishedItems;
	for (vector<Entry>::iterator iter=entries.begin();iter!=entries.end();iter++){
		if (iter.executeInterval == 0){
			//todo add instant exec logic here
			finishedItems.push(iter);
		}else{
			// todo check exec interval and add exec logic
			// todo then check serialize interval and add serialize logic
		}
	}

	// todo loop through finished items, check serialize on complete and remove

	return 0;
}
