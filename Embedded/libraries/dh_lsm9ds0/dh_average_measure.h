

#ifndef DH_AVGMEASURE_H__
#define DH_AVGMEASURE_H__

#include <dh_logger.h>
#include <queue>

class AveragedMeasure{

public:
	
	AveragedMeasure(int16_t initVal=0, uint8_t mergeQueueLength=10){
		value = initVal;
		queue = new std::queue<int16_t> ();
		if (mergeQueueLength<1)
			mergeQueueLength = 1;
		else if (mergeQueueLength>100)
			mergeQueueLength = 100;
		length = mergeQueueLength;

	}

	void setValue(int16_t val){
		uint8_t useless = 0;
		queue = new std::queue<int16_t> ();
		value = val;
	}

	void addMeasure(int16_t measure){
		if (queue->size() == length)
			processQueue();
		
		queue->push(measure);


	}

	std::queue<int16_t> * dropQueue(){
	
		std::queue<int16_t>  *temp = queue;
		queue = new std::queue<int16_t> ();
		return temp;
	}

	int16_t getValue(){
		return value;
	}

private:

	void processQueue(){

		if (queue->empty())
			return;
		value = value + (queue->back()-value)/length;
		queue->pop();
	}

	int16_t value;

	std::queue<int16_t> *queue;

	uint8_t length = 0;
};

#endif