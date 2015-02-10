
#ifndef DH_SENSOR_PROC_H__
#define DH_SENSOR_PROC_H__


#include <dh_logger.h>
#include <dh_movement.h>
#include <dh_position.h>
#include <dh_averaged_measure.h>

class SensorProcessor{
public:

	SensorProcessor(uint32_t _window){
		gTrim[0]=new AveragedMeasure<int32_t>(_window);
		gTrim[1]=new AveragedMeasure<int32_t>(_window);
		gTrim[2]=new AveragedMeasure<int32_t>(_window);

		gTrimValue[0] = 0;
		gTrimValue[1] = 0;
		gTrimValue[2] = 0;
	}

	bool getReady(){
		return gTrim[0]->getReady();
	}

	void processGyro(int16_t gx,int16_t gy,int16_t gz, uint32_t time){
		if (time<lastTime)
			return;

		if (movement->getMoving()){
			gTrim[0]->addMeasure(gx,time);
			gTrim[1]->addMeasure(gy,time);
			gTrim[2]->addMeasure(gz,time);

			dpsX = (gx-gTrimValue[0])*DPSXFACTOR;
			dpsY = (gy-gTrimValue[1])*DPSYFACTOR;
			dpsZ = (gz-gTrimValue[2])*DPSZFACTOR;
			
			bool stateChange = movement->evaluateGyro(dpsX,dpsY,dpsZ,time);

		}else{
			gTrim[0]->addMeasure(gx,time);
			gTrim[1]->addMeasure(gy,time);
			gTrim[2]->addMeasure(gz,time);

			gTrimValue[0] = gTrim[0]->getValue();
			gTrimValue[1] = gTrim[1]->getValue();
			gTrimValue[2] = gTrim[2]->getValue();

			if (gTrim[0]->getReady()){

			dpsX = (gx-gTrimValue[0])*DPSXFACTOR;
			dpsY = (gy-gTrimValue[1])*DPSYFACTOR;
			dpsZ = (gz-gTrimValue[2])*DPSZFACTOR;

				bool stateChange = movement->evaluateGyro(dpsX,dpsY,dpsZ,time);

				if (stateChange){
				//	replayWindow();
				}

			}
		}
		lastTime = time;
	}

	

	void setExternalMoveIndivator(bool isMoving,bool overrideActive){
		if (movement->overrideEvaluation(isMoving,overrideActive)){
			if (isMoving = movement->getMoving()){
				if (logger){
					logger->print("Started moving\n");
				}
			//	replayWindow();
			}else{
				if (logger){
					logger->print("Stopped moving\n");
				}
				//position->revertState();
			}
		}
	}

	void printState(){
		if (logger){
			logger->print("Sensor Processor:");
			logger->print(isMoving?"moving @":"still @");
			logger->print(lastTime);
			logger->println();
			if (gTrim[0]->getReady()){
				logger->print("GTrim Values : ");
				logger->print(gTrimValue[0]);
				logger->print(",");
				logger->print(gTrimValue[1]);
				logger->print(",");
				logger->print(gTrimValue[2]);
				logger->println();
			} else {
				logger->print("Not yet Ready - adjusting trim\n");
			}
			logger->print(" XTRIM: ");
			gTrim[0]->printDebugStatus();
			logger->print(" YTRIM: ");
			gTrim[1]->printDebugStatus();
			logger->print(" ZTRIM: ");
			gTrim[2]->printDebugStatus();

			if (movement)
				movement->printState();
		}
	}

	Logger *logger;
	MovementEval *movement;
	PositionState* position;

	bool isMoving=false;

	double DPSXFACTOR = .015;
	double DPSYFACTOR = .015;
	double DPSZFACTOR = .015;

	int32_t gTrimValue[3];

	double dpsX,dpsY,dpsZ;

private:
	uint32_t lastTime = 0;

	AveragedMeasure<int32_t> *gTrim[3];

};




#endif