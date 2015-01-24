
#ifndef DH_TRIMFILTER_H__
#define DH_TRIMFILTER_H__

#include <dh_logger.h>




class TrimFilter{
public:
	

	void filterGyro(uint16_t *gx,uint16_t *gy,uint16_t *gz);

	void movementDetected();

	void addStoppedGyroData(uint16_t gx, uint16_t gy, uint16_t gz);


private:



};

#endif