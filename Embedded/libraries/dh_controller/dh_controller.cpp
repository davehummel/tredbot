#include "dh_controller.h"
#include "dh_controlled.h"
#include <list>

void Controller::schedule(uint32_t id, uint16_t executeInterval,uint16_t serializeInterval, uint16_t totalRunTime, char[] command,Controlled controlled,bool serializeOnComplete){
	
