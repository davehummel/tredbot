
#ifndef DH_POSITION_H__
#define DH_POSITION_H__

#include <dh_logger.h>



class PositionState{
public:

	typedef struct {
		float xRot = 0;
		float yRot = 0;
		float zRot = 0;

		int32_t x = 0;
		int32_t y = 0;
		int32_t z = 0;
	} PSposition;

// TODO Primitive first version
	void inputGyro (float gx, float gy,float gz, uint16_t duration,bool firstMove){
		if (firstMove){ // A first move is a aggregate of multiple steps, including a zero step, all added together for the last X ms
			// it does not need to be averaged with the prev rot velocity
			zRot+=gz*duration;
		}else{
			zRot+=((prevZRotVel+gz)*duration)/2.0;
		}
		prevZRotVel = gz;
	}

	void inputAcl (float ax,float ay, float gz, uint16_t duration, bool firstMove);

	PSposition getPosition(){
		PSposition position = PSposition();
		position.zRot = zRot;
		return position;
	}


	Logger *logger;

private:
     //TODO primitive first version
	 float prevZRotVel = 0;
     float zRot = 0; 

};

#endif