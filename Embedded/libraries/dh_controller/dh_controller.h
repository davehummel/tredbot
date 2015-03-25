#ifndef DH_CONTROLLED_H__
#define DH_CONTROLLED_H__

#include <list>
using namespace std;

class Controller{
public:

	void schedule(uint32_t id, uint16_t executeInterval,uint16_t serializeInterval, uint16_t totalRunTime, char[] command,Controlled controlled,bool serializeOnComplete);
	
	void run(uint32_t id, char[] command,Controlled controlled,bool serializeOnComplete);
	
	void bindInterrupt(uint32_t id,uint8_t pin,char[] command, Controlled controlled);
	
	void unbindInterrupt(uint32_t id);
	
	list<ControlledResponse> execute(uint32_t time);
	
	list<ControlledResponse> getErrors;
	
private:

};

struct ControlledResponse{
 uint32_t id;
 char[] content;
};

#endif