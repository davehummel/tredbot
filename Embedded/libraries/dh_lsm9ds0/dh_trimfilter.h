
#ifndef DH_MOVEMENT_H__
#define DH_MOVEMENT_H__

#include <dh_logger.h>


class MovementEval{
public:
	
	public bool evaluateGyro (AveragedMeasure &gx, AveragedMeasure& gy,AveragedMEasure& gz); 

private:

}

class TrimFilter{
public:
	void filterGyro(int16_t &gx,int16_t &gy,int16_t &gz){
		gx-=gTrimX.getValue();
		gy-=gTrimY.getValue();
		gz-=gTrimZ.getValue();
	}

	void movementDetected(){
		gTrimX.dropQueue();
		gTrimY.dropQueue();
		gTrimZ.dropQueue();
	}

	void addStoppedGyroData(int16_t gx, int16_t gy, int16_t gz){
		gTrimX.addMeasure(gx);
		gTrimY.addMeasure(gy);
		gTrimZ.addMeasure(gz);
	}

	void setInitialGyroTrim(int16_t trimX, int16_t trimY, int16_t trimZ ){
		gTrimX.setValue(trimX);
		gTrimY.setValue(trimY);
		gTrimZ.setValue(trimZ);
	}

private:

	AverageMeasure gTrimX, gTrimY, gTrimZ;

};

class AveragedMeasure{

	ADD MINMAX range data for movement filter
public:
	AveragedMeasure(int16_t initVal=0, uint8_t mergeQueueLength=5){
		value = initVal;
		if (mergeQueueLength<1)
			mergeQueueLength = 1;
		else if (mergeQueueLength>100)
			mergeQueueLength = 100;

		queue = new int16_t[mergeQueueLength];
	}

	void setValue(int16_t val){
		dropQueue();
		value = val;
	}

	void addMeasure(int16_t measure){
		if (queueSize == queue.length)
			processQueue();
		
		queue[queueSize] = measure;

		queueSize ++;

	}

	int16_t[] dropQueue(){
		int16_t out [queueSize];
		for (int i = 0; i < queueSize ; i++){
			out[i] = queue[i];
		} 
		queueSize = 0;
		return out;
	}

	int16_t getValue(){
		return value;
	}

private:

	void processQueue(){
		int16_t delta = 0;
		for (int8_t i; i < queueSize ; i++){
			delta+=queue[i]-value;
		}
		delta/=queueSize;
		value = (2*value + delta)/2;
	}

	int16_t value;

	int16_t queue[];

	uint8_t queueSize = 0;

}

#endif