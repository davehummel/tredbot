#include <dh_logger.h>
#include <queue>
#include "dh_average_measure.h"
	
	AveragedMeasure::AveragedMeasure(int16_t initVal, uint16_t rewindWindowDur){
		value = initVal;
		if (rewindWindowDur<1)
			rewindWindowDur = 1;
		else if (rewindWindowDur>1000)
			rewindWindowDur = 1000;
		windowDur = rewindWindowDur;
	}

	void AveragedMeasure::setValue(float val){
		dropQueue();
		value = val;
	}

	void AveragedMeasure::addMeasure(int16_t measure, uint32_t time){
		uint16_t dif = 0;
		if (lastWriteTime == 0){
			lastWriteTime = time;
		}else{
			dif = time - lastWriteTime;

			if (dif>255)
				dif = 255;

			lastWriteTime = time;
			totalDur += dif;

			processQueue(dif);
		}
		amEntry entry;
		entry.val = measure;
		entry.dur = dif;
		queue.push(entry);
	}

	void AveragedMeasure::dropQueue(){

		while (!queue.empty())
			queue.pop();

		totalDur = 0;
		lastWriteTime = 0;
	
	}

// private:

	void AveragedMeasure::processQueue(uint8_t dif){
		while( !queue.empty() && (totalDur<windowDur || queue.size() > MAX_QUEUE_SIZE)){
			amEntry entry = queue.front();
			value = value + (entry.val-value)/SENSITIVITY;
			totalDur -= entry.dur;
			queue.pop();
		}
	}


