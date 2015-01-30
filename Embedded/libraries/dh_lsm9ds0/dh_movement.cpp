#include "dh_movement.h"
#include <dh_logger.h>
#include <queue>

bool MovementEval::evaluateGyro(float gx, float gy, float gz, uint32_t time){
	if (isGyroMoving){
		bool output = getMoving();
			float temp = gx>0?gx:-gx;
		if (temp>gNetLimit){
			buildResidueOnMove();
			return false;
		}
		temp = gy>0?gy:-gy;
		if (temp>gNetLimit){
			buildResidueOnMove();
			return false;
		}
		temp = gz>0?gz:-gz;
		if (temp>gNetLimit){
			buildResidueOnMove();
			return false;
		}

		balanceQueue(time);
		mEntry entry = queue.back();

		entry.gX+=gx;
		entry.gY+=gy;
		entry.gZ+=gz;

		gNetX = gNetX+gx;
		gNetY = gNetY+gy;
		gNetZ = gNetZ+gz;
		
		return getMoving()!=output;
	}else {
		bool output = !getMoving();
		float temp = gx>0?gx:-gx;
		if (temp>gSingleLimit){
			if (logger){
				logger->print("Movement detected on X in sudden move");
				logger->print(gx);
				logger->println();
			}
			isGyroMoving = true;
			buildResidueOnMove();
			return output;
		}
		temp = gy>0?gy:-gy;
		if (temp>gSingleLimit){
			if (logger){
				logger->print("Movement detected on Y in sudden move");
				logger->print(gy);
				logger->println();
			}
		
			isGyroMoving = true;
			buildResidueOnMove();
			return output;
		}
		temp = gz>0?gz:-gz;
		if (temp>gSingleLimit){
			if (logger){
				logger->print("Movement detected on Z in sudden move");
				logger->print(gz);
				logger->println();
			}
			isGyroMoving = true;
			buildResidueOnMove();
			return output;
		}

		balanceQueue(time);

		temp = gx+gNetX;
		if (temp>0?temp>gNetLimit:-temp>gNetLimit){
			if (logger){
				logger->print("Movement detected on X in net movement at ");
				logger->print(temp);
				logger->println();
			}
			isGyroMoving = true;
			buildResidueOnMove();
			return output;
		}
		temp = gy+gNetY;
		if (temp>0?temp>gNetLimit:-temp>gNetLimit){
			if (logger){
				logger->print("Movement detected on Y in net movement at ");
				logger->print(temp);
				logger->println();
			}
			isGyroMoving = true;
			buildResidueOnMove();
			return output;
		}
		temp = gz+gNetZ;
		if (temp>0?temp>gNetLimit:-temp>gNetLimit){
			if (logger){
				logger->print("Movement detected on Z in net movement at ");
				logger->print(temp);
				logger->println();
			}
			isGyroMoving = true;
			buildResidueOnMove();
			return output;
		}
		
		mEntry entry = queue.back();

		entry.gX+=gx;
		entry.gY+=gy;
		entry.gZ+=gz;

		gNetX = gNetX+gx;
		gNetY = gNetY+gy;
		gNetZ = gNetZ+gz;
		
		// if (logger){
		// logger->print("Queue size:");
		// logger->print((uint16_t)queue.size());
		// logger->print(" gNetX=");
		// logger->print(gNetX);
		// logger->println();
		// }

		return false;
	}
}

void MovementEval::balanceQueue(uint32_t time){
	int16_t dif = time/resolution - lastUpdateTime/resolution;
	lastUpdateTime = time;

	if (dif == 0){
		return;
	}

	if (dif*resolution>window){

		while (!queue.empty())
			queue.pop();
		gNetX=gNetY=gNetZ=aNetX=aNetY=aNetZ = 0;

	}else{

		dif = maxQueueSize - (dif + queue.size());

		if (dif<0){
			for (int i = 0 ; i < -dif ; i++){
				if (queue.empty())
					break;
				mEntry entry = queue.front();

				gNetX-=entry.gX;
				gNetY-=entry.gY;
				gNetZ-=entry.gZ;

				aNetX-=entry.aX;
				aNetY-=entry.aY;
				aNetZ-=entry.aZ;
				
				queue.pop();
			}
		}
	}

	queue.push(mEntry());

}

void MovementEval::buildResidueOnMove(){
	prevMovement.gXResidue = gNetX;
	prevMovement.gYResidue = gNetY;
	prevMovement.gZResidue = gNetZ;

	prevMovement.aXResidue = aNetX;
	prevMovement.aYResidue = aNetY;
	prevMovement.aZResidue = aNetZ;

	prevMovement.duration = queue.size()*resolution;

	while (!queue.empty()){
		queue.pop();
	}

	mEntry entry = mEntry();

	entry.gX = gNetX = gNetLimit;
	entry.gY = gNetY = gNetLimit;
	entry.gZ = gNetZ = gNetLimit;


}