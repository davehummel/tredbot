
#ifndef DH_TRIM_H__
#define DH_TRIM_H__

#include <queue>
#include <dh_logger.h>
#include "dh_average_measure.h"


class TrimFilter{
public:

	TrimFilter(int16_t _gTrimX=0,int16_t _gTrimY=0,int16_t _gTrimZ=0){
		gTrimX = new AveragedMeasure(_gTrimX);
		gTrimY = new AveragedMeasure(_gTrimY);
		gTrimZ = new AveragedMeasure(_gTrimZ);
	}

	void filterGyro(int16_t &gx,int16_t &gy,int16_t &gz){
		gx-=gTrimX->getValue();
		gy-=gTrimY->getValue();
		gz-=gTrimZ->getValue();
	}

	void movementDetected(){
		xDrop = gTrimX->dropQueue();
		yDrop = gTrimY->dropQueue();
		zDrop = gTrimZ->dropQueue();
	}

	void addStoppedGyroData(int16_t gx, int16_t gy, int16_t gz){
		gTrimX->addMeasure(gx);
		gTrimY->addMeasure(gy);
		gTrimZ->addMeasure(gz);
	}

	void setInitialGyroTrim(int16_t trimX, int16_t trimY, int16_t trimZ ){
		gTrimX->setValue(trimX);
		gTrimY->setValue(trimY);
		gTrimZ->setValue(trimZ);
	}


	std::queue<int16_t> *xDrop;

	std::queue<int16_t> *yDrop;

	std::queue<int16_t> *zDrop;


private:

	AveragedMeasure *gTrimX;
	AveragedMeasure *gTrimY; 
	AveragedMeasure *gTrimZ;


};



#endif