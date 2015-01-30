
#ifndef DH_TRIM_H__
#define DH_TRIM_H__

#include <queue>
#include <dh_logger.h>
#include "dh_average_measure.h"


class TrimFilter{
public:

	TrimFilter(int16_t _gTrimX=0,int16_t _gTrimY=0,int16_t _gTrimZ=0){
		gTrimX.setValue (_gTrimX);
		gTrimY.setValue (_gTrimY);
		gTrimZ.setValue (_gTrimZ);
	}

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

	void addStoppedGyroData(int16_t gx, int16_t gy, int16_t gz, uint32_t time){
		gTrimX.addMeasure(gx,time);
		gTrimY.addMeasure(gy,time);
		gTrimZ.addMeasure(gz,time);
	}

	void overrideGyroTrim(int16_t trimX, int16_t trimY, int16_t trimZ ){
		gTrimX.setValue(trimX);
		gTrimY.setValue(trimY);
		gTrimZ.setValue(trimZ);
	}

	void setRewindWindow(uint16_t windowDuration){
		gTrimX.setRewindWindow(windowDuration);
		gTrimY.setRewindWindow(windowDuration);
		gTrimZ.setRewindWindow(windowDuration);
	}

	uint16_t getRewindWindow(){
		return rewindWindow;
	}



private:

	uint16_t rewindWindow = 100;

	AveragedMeasure gTrimX;
	AveragedMeasure gTrimY; 
	AveragedMeasure gTrimZ;


};



#endif