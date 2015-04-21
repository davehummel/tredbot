#include "dh_controller.h"
#include <stdint.h>
#include "Stream.h"
#include <Arduino.h>

void Controller::loadControlled(char id,Controlled* controlled){
	if (id>'Z' || id < 'A')
		return;
	library[id-'A'] = controlled;
	if (controlled){
		controlled->begin();
	}
}

Controlled* Controller::getControlled(char id){
	
}

void Controller::schedule(uint32_t id, uint16_t initialExecDelay,  uint16_t executeInterval,
bool additiveInterval, uint32_t runCount, char command[],char controlled,bool serializeOnComplete){

	if (timedSize == 255)
		return;
	//Serial1.print(millis);
	// Serial1.print(":Scheduling id:");
	// Serial1.print(id);
	// Serial1.print(" initExecDelay:");
	// Serial1.print(initialExecDelay);
	// Serial1.print(" execInterval:");
	// Serial1.print(executeInterval);
	// Serial1.print(" runCount:");
	// Serial1.print(runCount);
	// Serial1.print(" command:");
	// Serial1.print(command);
	// Serial1.print(" controlled:");
	// Serial1.print(controlled);
	// Serial1.print(" serONComp:");
	// Serial1.println(serializeOnComplete);

	Entry* entry = new Entry();
	entry->id = id;
	entry->command = command;

	uint8_t controlledIndex;

	if (controlled>='a' && controlled<='z')
		controlledIndex=controlled-'a';
	else if (controlled>='A' && controlled<='Z')
		controlledIndex= controlled-'A';
	else return;
	
	entry->controlled = library[controlledIndex];
	entry->runCount = runCount;
	entry->executeInterval = executeInterval;
	entry->serializeOnComplete = serializeOnComplete;

	entry->controlled->controller = this;

	entry->controlled->startSchedule(command,id);

	entry->nextExecuteTime = (uint32_t)millis;
	entry->nextExecuteTime = entry->nextExecuteTime+initialExecDelay;	

	entry->killed = false;

	// Serial1.print(command);
	// Serial1.println(entry->nextExecuteTime);	
	addTimedEntry(entry);
}

void Controller::loadProgram(uint8_t id, char program[]){
	if (programs[id]){
		delete programs[id];
	}
	programs[id]=program;
}

void Controller::deleteProgram(uint8_t id){
	delete programs[id];
	programs[id] = 0;
}

void Controller::runProgram(uint8_t id){
	bufferCount = 0;
	if (programs[id]){
		uint16_t offset = 0;
		char* program = programs[id];
		while(true){
			char c = program[offset];
			offset++;
			if (c ==';' || c =='\0'){
				inputbuffer[bufferCount] = '\0';
				parseBuffer();
				bufferCount = 0;
				if (c == '\0'){
					return;
				}
			} else {
				inputbuffer[bufferCount] = c;
				bufferCount++;
			}
		}
	}
}

void Controller::run(uint32_t id, char command[],uint8_t controlled,bool serializeOnComplete){
	
	if (immediateSize == 255)
		return;


	if (controlled>='a' && controlled<='z')
		controlled-='a';
	else if (controlled>='A' && controlled<='Z')
		controlled-='A';
	else return;
	
	Entry* entry = new Entry();
	entry->id = id;
	entry->command = command;
	entry->controlled = library[controlled];
	entry->serializeOnComplete = serializeOnComplete;

	entry->controlled->controller = this;

	immediate[immediateSize] = entry;
	immediateSize++;
}

void Controller::execute(Stream* output){
	// First check immediate run queue

	for (int i = 0; i < immediateSize; i ++){
			immediate[i]->controlled->execute((uint32_t)millis,immediate[i]->id,immediate[i]->command);
			if (immediate[i]->serializeOnComplete){
				immediate[i]->controlled->serialize(output,immediate[i]->id,immediate[i]->command);
			}

			delete immediate[i]->command;
			
			delete immediate[i];

	}
	immediateSize = 0;

	uint32_t offset = (uint32_t)millis - lastProcessedMSTime;

	if (offset == 0)
		return;

	if (offset > 1){
		Serial1.print('#');
		Serial1.print(offset);
		Serial1.print('#');
	}

	lastProcessedMSTime +=offset;

	bool doA = isANext;
	isANext = !isANext;
	uint8_t count = timedSize;

	timedSize = 0;

	for (int i =0 ; i < count; i ++){

		Entry* iter = doA?timedA[i]:timedB[i];
		currentlyRunning = iter;
		bool addBack = true;

		if (iter->killed){
			
			iter->controlled->endSchedule(iter->command, iter->id);

			delete iter->command;
				
			delete iter;

			continue;

		}

		if ((uint32_t)millis >= iter->nextExecuteTime){

			if (iter->executeInterval == 0 ){
				iter->runCount = 1;
			}
				
			iter->controlled->execute((uint32_t)millis,iter->id,iter->command);
			if (iter->serializeOnComplete){
				iter->controlled->serialize(output,iter->id,iter->command);
			}
			
			if (iter->runCount>0){
				iter->runCount--;
				if (iter->runCount == 0){
	
					iter->controlled->endSchedule(iter->command, iter->id);

					delete iter->command;
				
					delete iter;
				
					addBack = false;
				}
			}
			if (iter->additiveInterval)
				iter->nextExecuteTime =millis + iter->executeInterval;
			else
				iter->nextExecuteTime += iter->executeInterval;
		}

		if (addBack){
			addTimedEntry(iter);
		}
	}

	currentlyRunning = 0;
		
}

void Controller::processInput(Stream* stream){
	while (stream->available()){
		char next = stream->read();
		Serial1.print(next);
		if (next == '\n'||next == '\r'){
			inputbuffer[bufferCount]='\0';
			parseBuffer();
			bufferCount = 0;
		}else{
			inputbuffer[bufferCount] = next;
			if (bufferCount == 511){
				bufferCount = 0;
			}else{
				bufferCount++;
			}
		}
	}
	
}


void Controller::parseBuffer(){

	if (inputbuffer[0]=='K'){
		if (bufferCount<3)
			return;
		uint16_t offset = 2;
		uint32_t id=0;
		if (!parse_uint32(id,offset,inputbuffer))
			return;

	Serial1.print(">K>");
	Serial1.print(id);
	Serial1.print('@');
	Serial1.println(millis);

		kill(id);
		return;
	}

	if (inputbuffer[0]=='P'){
		if (bufferCount<3)
			return;
		uint8_t id=0;
		uint16_t offset = 2;

		if (!parse_uint8(id,offset,inputbuffer))
			return;

		offset++;

		if (bufferCount<=offset){
			Serial1.print(">P->");
			Serial1.print(id);
			Serial1.print('@');
			Serial1.println(millis);
			deleteProgram(id);
			return;
		}

		char* program = new char[bufferCount-offset];

		for (uint16_t i = 0; i < bufferCount-offset; i++){
			program[i] = inputbuffer[i+offset];
		}

		Serial1.print(">P+>");
		Serial1.print(id);
		Serial1.print('@');
		Serial1.println(millis);

		loadProgram(id,program);
		return;
	}

	if (inputbuffer[0]=='R'){
		if (bufferCount<3)
			return;
		uint8_t id=0;
		uint16_t offset = 2;

		if (!parse_uint8(id,offset,inputbuffer))
			return;

		Serial1.print(">R>");
		Serial1.print(id);
		Serial1.print('@');
		Serial1.println(millis);

		runProgram(id);

		return;
	}

	if (bufferCount<6)
		return;

	if (inputbuffer[0]!='C'||inputbuffer[1]!=' '||inputbuffer[3]!=' '){
		return;
	}
	char commandID;
	commandID = inputbuffer[2];
	//todo parse id;

	uint32_t id = 0;
	uint16_t offset = 4;

	if (!parse_uint32(id,offset,inputbuffer))
		return;
	
	offset++;

	char* command =new char[bufferCount-offset+1];
	//command[bufferCount-offset]='\0'; // this shouldnt be needed anymore
	for (uint16_t i = 0; i <= bufferCount-offset; i++){
		command[i] = inputbuffer[i+offset];
	}
	if (command[bufferCount-offset]!='\0'){
		Serial1.println("Oh fuck");
	}
// TIME sync feedback
	Serial1.print(">C>");
	Serial1.print(id);
	Serial1.print('@');
	Serial1.println(millis);

// DEBUG CODE
	// Serial1.print("Command processed:");
	// Serial1.print(" id =");
	// Serial1.print(id);
	// Serial1.print(" command ");
	// Serial1.print(command);
	// Serial1.print(" Controlled ID:");
	// Serial1.println((char)commandID);
	
	run(id,command,commandID,true);
	
}

void Controller::addTimedEntry(Entry* entry){
	if (isANext){
		timedA[timedSize] = entry;
	}else{
		timedB[timedSize] = entry;
	}
	timedSize++;
}


void Controller::kill(uint32_t id){
	for (int i = 0 ; i < timedSize ; i++){
		Entry* entry = isANext?timedA[i]:timedB[i];
		if (entry->id == id){
			entry->killed = true;
		} 
	}
}

void Controller::kill(){
	if (currentlyRunning!=0)
		currentlyRunning->killed =true;
}

bool Controller::parse_uint8(uint8_t &val, uint16_t &pointer,char* text){
	val = 0;

	uint8_t valChars;

	for (valChars=0;valChars<=3;valChars++){
		char c = text[pointer+valChars];
		if (c<'0' || c>'9')
			break;
	}

	if (valChars == 0 || valChars == 4)
		return false;

	uint8_t multiplier = 1;

	for (int i = valChars-1;i>=0;i--){
		val+= multiplier*(text[pointer+i]-'0');
		multiplier*=10;
	}

	pointer+=valChars;

	return true;

}

bool Controller::parse_uint16(uint16_t &val, uint16_t &pointer,char* text){
	val = 0;

	uint8_t valChars;

	for (valChars=0;valChars<=5;valChars++){
		char c = text[pointer+valChars];
		if (c<'0' || c>'9')
			break;
	}

	if (valChars == 0 || valChars == 6)
		return false;

	uint16_t multiplier = 1;

	for (int i = valChars-1;i>=0;i--){
		val+= multiplier*(text[pointer+i]-'0');
		multiplier*=10;
	}

	pointer+=valChars;

	return true;

}

bool Controller::parse_int16(int16_t &val, uint16_t &pointer,char* text){
	val = 0;

	bool negative = false;
	if (text[pointer]=='-'){
		negative = true;
		pointer++;
	}

	uint8_t valChars;

	for (valChars=0;valChars<=5;valChars++){
		char c = text[pointer+valChars];
		if (c<'0' || c>'9')
			break;
	}

	if (valChars == 0 || valChars == 6)
		return false;

	int16_t multiplier = negative?-1:1;

	for (int i = valChars-1;i>=0;i--){
		val+= multiplier*(text[pointer+i]-'0');
		multiplier*=10;
	}

	pointer+=valChars;

	return true;

}

bool Controller::parse_uint32(uint32_t &val, uint16_t &pointer,char* text){
	val = 0;

	uint8_t valChars;

	for (valChars=0;valChars<=10;valChars++){
		char c = text[pointer+valChars];
		if (c<'0' || c>'9')
			break;
	}

	if (valChars == 0 || valChars == 11)
		return false;

	uint32_t multiplier = 1;

	for (int i = valChars-1;i>=0;i--){
		val+= multiplier*(text[pointer+i]-'0');
		multiplier*=10;
	}

	pointer+=valChars;

	return true;

}

char* Controller::newString(const char original[]){
	int size;
	for (size = 0 ; size < 256 ; size++){
		if (original[size]=='\0')
			break;
	}

	char* data = new char[size+1];
	data[size] = '\0';
	for (int i =0 ; i < size ; i++)
		data[i] = original[i];

	return data;
}