
#ifndef DH_MOVEMENTEVAL_H__
#define DH_MOVEMENTEVAL_H__

#include <dh_logger.h>
#include <queue>


class MovementEval{
public:

	typedef struct {
		float gXResidue = 0;
		float gYResidue = 0;
		float gZResidue = 0;

		float aXResidue = 0;
		float aYResidue = 0;
		float aZResidue = 0;

		float duration =0;
	} movementResidue;

	MovementEval(float _gNetLimit=.4,float _gSingleLimit=.25, float _window=100, float _resolution=10){
		gNetLimit = _gNetLimit;
		gSingleLimit = _gSingleLimit;
		window = _window;
		resolution = _resolution;
		maxQueueSize = window/resolution;
	}

	// return true if this evauation triggered a change in movement state
	bool evaluateGyro (float gx, float gy,float gz, uint32_t time); 

	bool evaluateAcl (float ax,float ay, float gz, uint32_t time);

	// Forces Evaluator into set state until overrideEndTime
	bool overrideEvaluation(bool moving, uint32_t _overrideEndTime){
		bool prevState = getMoving();
		isOverrideMoving = moving;
		overrideEndTime = _overrideEndTime;
		bool curState = getMoving();
		return curState!=prevState;
	}

	bool getMoving(){

		if (lastUpdateTime<overrideEndTime)
			return isOverrideMoving;

		return isGyroMoving||isAccelMoving;
	}

	movementResidue prevMovement;

	float gNetLimit;
	float gSingleLimit;
	float aNetLimit;
	float aSingleLimit;

	Logger *logger;

private:

	typedef struct {
		float gX=0;
		float gY=0;
		float gZ=0;

		float aX=0;
		float aY=0;
		float aZ=0;
	} mEntry;

	void balanceQueue(uint32_t time);

	void buildResidueOnMove();


	uint16_t window;
	uint16_t resolution;

	uint16_t maxQueueSize;

	bool isGyroMoving=false;
	bool isAccelMoving=false;
	bool isOverrideMoving=false;
	bool queueFilling = true;

	uint32_t overrideEndTime=0;
	uint32_t lastUpdateTime=0;

	float gNetX=0;
	float gNetY=0;
	float gNetZ=0;

	float aNetX =0;
	float aNetY =0;
	float aNetZ =0;

	std::queue<mEntry> queue;

};

#endif