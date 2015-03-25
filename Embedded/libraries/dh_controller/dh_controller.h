#ifndef DH_CONTROLLER_H__
#define DH_CONTROLLER_H__

#include <vector>
#include <list>
#include <dh_controlled.h>
#include <stdint.h>

using namespace std;

class Controller{
public:
	struct ControlledResponse{
 		uint32_t id;
 		char content[];
	};


	void schedule(uint32_t id, uint16_t executeInterval,uint16_t serializeInterval, uint16_t runCount, char command[],Controlled* controlled,bool serializeOnComplete);
	
	void run(uint32_t id, char command[],Controlled *controlled,bool serializeOnComplete);
	
	vector<ControlledResponse>* execute(uint32_t time);

private:

	struct Entry {
		uint32_t id;
		char command[];
		Controlled *controlled;
		bool serializeOnComplete;
		uint16_t executeInterval;
		uint16_t serializeInterval;
		uint16_t runCount;
	};

	vector<Entry> entries;

};


#endif