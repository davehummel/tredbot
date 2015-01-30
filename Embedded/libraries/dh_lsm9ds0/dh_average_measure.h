

#ifndef DH_AVGMEASURE_H__
#define DH_AVGMEASURE_H__

#include <dh_logger.h>
#include <queue>


class AveragedMeasure{

public:
	
	AveragedMeasure(int16_t initVal=0, uint16_t rewindWindowDur = 100);

	void setValue(float val);

	void addMeasure(int16_t measure, uint32_t time);

	void dropQueue();

	float getValue(){
		return value;
	}

	void setRewindWindow(uint16_t rewindWindowDur){
		windowDur = rewindWindowDur;
	}

	uint16_t MAX_QUEUE_SIZE = 200;
	float SENSITIVITY = 10;

private:
	typedef struct vector_am{
		int16_t val=0;
		uint8_t dur=0;
	}amEntry;

	void processQueue(uint8_t dif);

	float value;
	uint16_t totalDur=0;
	uint32_t lastWriteTime = 0;

	std::queue<amEntry> queue;

	uint16_t windowDur;
};

#endif