
#ifndef DH_CONTROLLEDNAV_H__
#define DH_CONTROLLEDNAV_H__
#include "dh_controller.h"
 #include <ControlledPanTilt.h>
 #include <ControlledMotor.h>
 #include <ControlledI2CXL.h>
 #include <ControlledBN055.h>
 #include <ControlledLidar.h>
#include "quadfit.h"

#define ABS_CONF  1.2
#define HIGH_CONF 3
#define LOW_CONF 10

#define NAV_ID 200
#define ECHO_ID 210
#define LIDAR_ID 216
#define LIA_ID 220
#define GYRO_ID 222
#define MOTOR_ID 230
#define SERVO_ID 240

#define MIN_DIST 25
#define STOP_DIST 30

#define BASE_DELAY 100
#define BASE_INTERVAL 10

#define ACCEL_MULT 1
#define ACCEL_DELAY 5

#define RANGE_MULT 10
#define RANGE_EVAL_MULT 5

#define ECHO_MULT 10
#define ECHO_DELAY 1
#define ECHO_MIN 24
#define ECHO_MAX 400
#define ECHO_AB_DELAY 40

#define LIDAR_MULT 2
#define LIDAR_DELAY 0
#define LIDAR_MIN 40
#define LIDAR_MAX 2000

#define GYRO_MULT 2
#define GYRO_DELAY 2


class ControlledNavigator: public Controller::Controlled{

public:

	void setup(void){

		rangeFreq = 1000/(baseInterval*rangeMult);
		accelFreq = 1000/(baseInterval*accelMult);

		qfitA.setSampleCount(rangeEvalMult);
		qfitB.setSampleCount(rangeEvalMult);
		qfitL.setSampleCount(rangeEvalMult);
		if (qfitA.xData)
			delete[] qfitA.xData;
		qfitA.xData = new double[rangeEvalMult];
		if (qfitB.xData)
			delete[] qfitB.xData;
		qfitB.xData = new double[rangeEvalMult];
		if (qfitL.xData)
			delete[] qfitL.xData;
		qfitL.xData = new double[rangeEvalMult];
		if (qfitA.yData)
			delete[] qfitA.yData;
		qfitA.yData = new double[rangeEvalMult];
		if (qfitB.yData)
			delete[] qfitB.yData;
		qfitB.yData = new double[rangeEvalMult];
		if (qfitL.yData)
			delete[] qfitL.yData;
		qfitL.yData = new double[rangeEvalMult];

		if (qfitA.approxYVals)
			delete[] qfitA.approxYVals;
		qfitA.approxYVals = new double[rangeEvalMult];
		if (qfitB.approxYVals)
			delete[] qfitB.approxYVals;
		qfitB.approxYVals = new double[rangeEvalMult];
		if (qfitL.approxYVals)
			delete[] qfitL.approxYVals;
		qfitL.approxYVals = new double[rangeEvalMult];
		for (uint16_t i = 0; i < rangeEvalMult; i++){
			qfitA.xData[i] = i;
			qfitB.xData[i] = i;
			qfitL.xData[i] = i;
		}
		qfitA.processXData();
		qfitB.processXData();
		qfitL.processXData();

		if (prevEstError)
			delete[] prevEstError;

		prevEstError = new double[rangeEvalMult];
		prevEsts = new double[rangeEvalMult];
	}

	void begin(void){

		setup();

		echo = (ControlledI2CXL*) controller->getControlled('S');
		motor = (ControlledMotor*) controller->getControlled('M');
		panTilt = (ControlledPanTilt*) controller->getControlled('P');
		gyro = (ControlledBN055*) controller->getControlled('G');
		lidar = (ControlledLidar*) controller->getControlled('L');
		

	}

	void execute(uint32_t time,uint32_t id,char command[], bool serializeOnComplete){
		uint16_t pointer = 0;
		switch (command[0]){
			case 'Z':{ // ZERO
				uint8_t sampleRate;
				if (command[4] == '\0'){
					lia_bias[0]+= gyro->accel[0];
					lia_bias[1]+= gyro->accel[1];
					lia_bias[2]+= gyro->accel[2];
					break;
				}

				pointer = 5;
				if (!Controller::parse_uint16(zeroSampleCount,pointer,command)){
					return;
				}

				pointer++;
				if (!Controller::parse_uint8(sampleRate,pointer,command)){
					return;
				}

					lia_bias[0]=0;
					lia_bias[1]=0;
					lia_bias[2]=0;

				controller->schedule(NAV_ID,sampleRate,sampleRate,false,zeroSampleCount,Controller::newString("ZERO"),'N',false);
				controller->schedule(LIA_ID,1,sampleRate,false,zeroSampleCount,Controller::newString("LIA"),'G',true);
				break;
			}
			case 'T':{ // TURN
				pointer = 5;
				if (!Controller::parse_int16(targetDir,pointer,command)){
					return;
				}

				pointer++;

				if (!Controller::parse_float(turnFactor,pointer,command)){
					return;
				}

				pointer++;

				if (!Controller::parse_int16(minTurnThrottle,pointer,command)){
					return;
				}

				if (!isMoving){
					targetDist = 0;
					initThrottle = 0;
					maxAllowedRangeSamples = 2000/baseInterval;
					start();
				}else{
					if (estDir!=-9999){
						targetDir = estDir+targetDir;
					}
					if (maxAllowedRangeSamples - cycleCount< 2000/baseInterval)
						maxAllowedRangeSamples = cyleCount + 2000/baseInterval;
				}
				break;
			}
			case 'M':{ // MOVE
				if (command[4] == '\0'){
					moveEval(time);
					break;
				}
				pointer = 5;
				if (!Controller::parse_uint16(targetSpeed,pointer,command)){
					return;
				}

				pointer++;

				if (!Controller::parse_uint16(targetDist,pointer,command)){
					return;
				}

				pointer++;

				if (!Controller::parse_uint8(initThrottle,pointer,command)){
					return;
				}
				pointer++;

				uint32_t maxMoveTime;

				if (!Controller::parse_uint32(maxMoveTime,pointer,command)){
					return;
				}

				maxAllowedRangeSamples = (maxMoveTime/baseInterval);

				if (isMoving){
					stop();
				}


				if (panTilt->getHeight() != 512)
					controller->run(SERVO_ID,Controller::newString("MOVE 2 512"),'P',false);
				if (panTilt->getGoal() != 512)
					controller->run(SERVO_ID,Controller::newString("MOVE 1 512"),'P',false);

				start();
				break;
			}
		}
	}
	void serialize(Stream* output, uint32_t id, char command[]){
		if (printMovingUpdate == true){
			output->print('<');
			output->print(id);
			output->print('@');
			output->print(printTime);
			output->print('M');
			output->print(estDist);
			output->print('S');
			output->println(estSpeed);

			printMovingUpdate = false;

		}
		if (printZeroUpdate == true){
			output->print('<');
			output->print(id);
			output->print('@');
			output->print(printTime);
			output->print("ZERO");

			output->print(lia_bias[0]);
			output->print(',');
			output->print(lia_bias[1]);
			output->print(',');
			output->println(lia_bias[2]);

			printZeroUpdate = false;

		}	
		if (printBlockUpdate == true){
			output->print('<');
			output->print(id);
			output->print('@');
			output->print(printTime);
			output->print("BLOCKED");

			output->println(nearestObj);

			printBlockUpdate = false;

		}	
		if (printStarted == true){
			output->print('<');
			output->print(id);
			output->print('@');
			output->print(printTime);
			output->println("Started");

			printStarted = false;

		}	
	}
	void startSchedule(char command[], uint32_t id){
	
	}
	void endSchedule(char command[], uint32_t id){
		switch (command[0]){
			case 'Z':{ // ZERO
				if (command[4] == '\0'){
					lia_bias[0]/=(double)zeroSampleCount;
					lia_bias[1]/=(double)zeroSampleCount;
					lia_bias[2]/=(double)zeroSampleCount;
					printZeroUpdate = true;
					controller->run(id,Controller::newString("DONE"),'N',true);
					break;
				}
			}
		}
	}


	uint16_t targetSpeed;
	uint16_t targetDist;
	int16_t targetDir;

	uint8_t initThrottle; 
	uint8_t currentThrottle;
	uint16_t nearestObj;
	double estSpeed;
	double estDist;
	int16_t estDir;
	double lia_bias[3] = {0.,0.,0.};
	bool isMoving = false;

	double ALTestSpeed;
	double ALTestDist;
	
private:
	// DIRECT CONFIGURATIONS to SENSOR LOGIC
	uint16_t minDist = MIN_DIST;
	uint16_t stopDist = STOP_DIST;
	uint16_t baseInterval = BASE_INTERVAL;
	uint16_t accelMult = ACCEL_MULT;
	uint16_t accelDelay = ACCEL_DELAY;
	uint16_t rangeMult = RANGE_MULT;
	uint16_t rangeEvalMult = RANGE_EVAL_MULT;
	uint16_t echoMult = ECHO_MULT;
	uint16_t echoDelay = ECHO_DELAY;
	uint16_t echoMin = ECHO_MIN;
	uint16_t echoMax = ECHO_MAX;
	uint16_t echoABDelay = ECHO_AB_DELAY;
	uint16_t lidarMult = LIDAR_MULT;
	uint16_t lidarDelay = LIDAR_DELAY;
	uint16_t lidarMin = LIDAR_MIN;
	uint16_t lidarMax = LIDAR_MAX;
	uint16_t gyroMult = GYRO_MULT;
	uint16_t gyroDelay = GYRO_DELAY;
	uint16_t zeroSampleCount;
	// DERIVED CONFIGURATIONS to SENSOR LOGIC
	uint16_t cyclesPerRangeEval = 0;
	QuadFit<double> qfitA,qfitB,qfitL;
	uint8_t echoSubSamples;
	uint8_t lidarSubSamples;
	double accelFreq;
	double rangeFreq;
	// // OPERATIONAL 
	uint16_t rangeSampleCount;
	uint16_t maxAllowedRangeSamples ;
	uint32_t cycleCount;
	double prevEstSpeed;
	double* prevEsts;

	double* prevEstError;

	double prevAccelYData;



	bool printMovingUpdate = false;
	bool printZeroUpdate = false;
	bool printBlockUpdate = false;
	bool printStarted = false;

	uint32_t printTime = 0;

	ControlledMotor* motor;
	ControlledI2CXL* echo;
	ControlledPanTilt* panTilt;
	ControlledBN055* gyro;
	ControlledLidar* lidar;


	void moveEval(uint32_t time){
		cycleCount++;

		printTime = time;

		nearestObj = echo->readingA;
		if (echo->readingB < nearestObj)
			nearestObj = echo->readingB;

		if (nearestObj<=stopDist && nearestObj > 0){
			printBlockUpdate = true;
			stop();
			return;
		}

	

		calcSpeed();
		calcDirection();
		motor->updateMotor();




		if (cycleCount == maxAllowedRangeSamples){
			Serial.println("TIME ABORT!");
			stop();
			return;
		}

		if (estSpeed*.1 + estDist >= targetDist){
			Serial.println("FINISHED!");
			stop();
			return;
		}

// TODO write slow down function here
		if (estSpeed*.3 + estDist >= targetDist){
					Serial.print("Imd Speed ");
					Serial.print(estSpeed);
					Serial.print(" Dist ");
					Serial.println(estDist);
					motor->m0Speed = 40;
					motor->m1Speed = 40;
		}

	}

	void calcDirection(){
		if (cycleCount%gyroMult==0) {
			processGyro();
		}
	}


	void calcSpeed(){
		// TODO try to collect regular data
		if (cycleCount%accelMult==0){
			processAccel();
		}

		if (cycleCount%echoMult==0){
			qfitA.yData[rangeEvalMult-1] += echo->readingA;
			qfitB.yData[rangeEvalMult-1] += echo->readingB;
			echoSubSamples++;
		}
		if (cycleCount%lidarMult==0){
			qfitL.yData[rangeEvalMult-1] += lidar->lastRead;
			lidarSubSamples++;
		}

	
		if (cycleCount%rangeMult == 0){	
			qfitA.yData[rangeEvalMult-1] /= (double) echoSubSamples;
			qfitB.yData[rangeEvalMult-1] /= (double) echoSubSamples;
			qfitL.yData[rangeEvalMult-1] /= (double) lidarSubSamples;
			echoSubSamples = 0;
			lidarSubSamples = 0;

			if (rangeMult == cycleCount){
				presample();
			}

			double confA=999,confB=999,confL=999;
		//	Serial.print("EchoA:");
			calcConf(confA,echoMin,echoMax,qfitA);
		//	Serial.print("EchoB:");
			calcConf(confB,echoMin,echoMax,qfitB);
		//	Serial.print("Lidar:");
			calcConf(confL,echoMin,echoMax,qfitL);


			mergeRangeAndAccel(confA,confB,confL);

	
			// if (cycleCount% ((rangeEvalMult-1)*rangeMult) == 0){	
			// 	ALTmergeRangeAndAccel(confA,confB,confL);
			// 	ALTprevEstDist = ALTestDist;
			// 	ALTprevEstSpeed = ALTestSpeed;
			// 		Serial.print("ALTDist:");
			// 		Serial.print(ALTestDist);
			// 		Serial.print("ALTSpeed:");
			// 		Serial.println(ALTestSpeed);
			// }
					Serial.print("estDist:");
					Serial.print(estDist);
					Serial.print("estSpeed:");
					Serial.println(estSpeed);

			shift(rangeEvalMult, qfitA);
			shift(rangeEvalMult, qfitB);
			shift(rangeEvalMult, qfitL);
			prevEstDist = estDist;
			prevEstSpeed = estSpeed;

			printMovingUpdate = true;
		}

	}

	void processAccel(){
		double prevSpeed = estSpeed;
		double curAccel =  lia_bias[1] -gyro->accel[1];
		estSpeed += ((prevAccelYData + curAccel)/ 2.0) / (accelFreq); 
		estDist += ((estSpeed + prevSpeed) / 2.0) / (accelFreq); 
		prevAccelYData = curAccel;

		// ALTprevSpeed = ALTestSpeed;
		// ALTestSpeed += ((prevAccelYData + curAccel)/2.0) / (accelFreq); 
		// ALTestDist += ((ALTestSpeed + prevSpeed) / 2.0) / (accelFreq); 
	}

	void processGyro(){
		bool firstUpdate = estDir = -9999;
		estDir = gyro->heading[0];
		if (firstUpdate){
			targetDir = estDir + targetDir;
		}

		int16_t delta = solveDirDelta(estDir,targetDir);
		solveMotorDifferential(delta,motorA,motorB,minTurnThrottle,turnFactor);

	}

	int16_t solveDirDelta(int16_t source, int16_t target){
		uint16_t delta = target%360 - source%360;
		if (delta > 180){
			delta =  delta - 360;
		}else if (delta < -180){
			delta = 360 + delta;
		}
		return delta;
	}

	void calcConf(double &rangeConf,double min,double max, QuadFit<double> &qfit){

		Serial.print(qfit.yData[0]);
		Serial.print(',');
		Serial.print(qfit.yData[1]);
		Serial.print(',');
		Serial.print(qfit.yData[2]);
		Serial.print(',');
		Serial.print(qfit.yData[3]);
		Serial.print(',');
		Serial.print(qfit.yData[4]);


		for (uint8_t i = 0 ; i < rangeEvalMult ; i++){
			if (qfit.yData[i]>=max && i > 0){
				Serial.println(" No Conf!!");
				rangeConf = 1000;
				return;
			}else if (qfit.yData[i]< min && i < rangeEvalMult){
				Serial.println(" No Conf!!");
				rangeConf = 1000;
				return;
			}
		}

		qfit.solve();

		qfit.calculate();

		rangeConf = 0;

		for (uint8_t i = 0 ; i <= rangeEvalMult ; i++){
			float temp = (qfit.approxYVals[i] - qfit.yData[i]);
			temp *= .2 + (float)i/3.0;
			rangeConf +=  temp>0? temp : -temp;
		}

		Serial.print(" -> ");
		Serial.println(rangeConf);
	
		return;

	}

	void mergeRangeAndAccel(double echoConfA,double echoConfB, double lidarConf ){
		if (echoConfA >= HIGH_CONF && echoConfB >= HIGH_CONF && lidarConf >= HIGH_CONF){
			Serial.println("Use Accel");
			return;// Just use accel data
		}

		float estSpeedEchoA,estSpeedEchoB,estSpeedLidar,estDistEchoA,estDistEchoB,estDistLidar;

		if (lidarConf < .01 ) {// zero check
			solveRangeMove(estSpeedLidar,estDistLidar,qfitL);
			estSpeed = estSpeedLidar;
			estDist = prevEstDist + estDistLidar;
			Serial.println("Chose Lidar with perfect match");
			return;
		}

		if (echoConfA < .01 ) {// zero check
			solveRangeMove(estSpeedEchoA,estDistEchoA,qfitA);
			estSpeed = estSpeedEchoA;
			estDist = prevEstDist + estDistEchoA;
			Serial.println("Chose Echo A with perfect match");
			return;
		}

		if (echoConfB < .01 ) {// zero check
			solveRangeMove(estSpeedEchoB,estDistEchoB,qfitB);
			estSpeed = estSpeedEchoB;
			estDist = prevEstDist + estDistEchoB;
			Serial.println("Chose Echo B with perfect match");
			return;
		}

		uint8_t trustLevelA, trustLevelB, trustLevelL;
		if (echoConfA < ABS_CONF)
			trustLevelA = 3;
		else if (echoConfA < HIGH_CONF)
			trustLevelA = 2;
		else if (echoConfA < LOW_CONF)
			trustLevelA = 1;
		else 
			trustLevelA = 0;

		if (echoConfB < ABS_CONF)
			trustLevelB = 3;
		else if (echoConfB < HIGH_CONF)
			trustLevelB = 2;
		else if (echoConfB < LOW_CONF)
			trustLevelB = 1;
		else 
			trustLevelB = 0;

		if (lidarConf < ABS_CONF)
			trustLevelL = 3;
		else if (lidarConf < HIGH_CONF)
			trustLevelL = 2;
		else if (lidarConf < LOW_CONF)
			trustLevelL = 1;
		else 
			trustLevelL = 0;

		// Serial.print("Trust Level A:");
		// Serial.print(trustLevelA);
		// Serial.print("Trust Level B:");
		// Serial.print(trustLevelB);
		// Serial.print("Trust Level L:");
		// Serial.println(trustLevelL);

		uint8_t maxTrustLevel = trustLevelA;
		if (trustLevelB>maxTrustLevel)
			maxTrustLevel = trustLevelB;
		if (trustLevelL>maxTrustLevel)
			maxTrustLevel = trustLevelL;


		if (maxTrustLevel == 3){
			uint8_t count = 0;
			estSpeed = 0;
			estDist = 0;

			if (trustLevelA == 3){
				solveRangeMove(estSpeedEchoA,estDistEchoA,qfitA);
				estSpeed += estSpeedEchoA;
				estDist += prevEstDist + estDistEchoA;
				count++;
			} 
			if (trustLevelB == 3){
				solveRangeMove(estSpeedEchoB,estDistEchoB,qfitB);
				estSpeed += estSpeedEchoB;
				estDist += prevEstDist + estDistEchoB;
				count++;
			} 
			if (trustLevelL == 3){
				solveRangeMove(estSpeedLidar,estDistLidar,qfitL);
				estSpeed += estSpeedLidar;
				estDist += prevEstDist + estDistLidar;
				count++;
			} 
			if (count>1){
				estSpeed/=(double)count;
				estDist/=(double)count;
			}
			return;
		} else if (maxTrustLevel == 2){
			uint8_t count = 1;
			double distAddition=estDist-prevEstDist;

			if (trustLevelA == 2){
				solveRangeMove(estSpeedEchoA,estDistEchoA,qfitA);
		 		estSpeed +=estSpeedEchoA;
				distAddition+= estDistEchoA;
				count++;
			} 

			if (trustLevelB == 2){
				solveRangeMove(estSpeedEchoB,estDistEchoB,qfitB);
				estSpeed  +=estSpeedEchoB;
				distAddition+= estDistEchoB;
				count++;
			} 	
			if (trustLevelL == 2){
				solveRangeMove(estSpeedLidar,estDistLidar,qfitL);
		 		estSpeed  +=estSpeedLidar;
				distAddition+= estDistLidar;
				count++;
			}
			estSpeed/=(double)count;
			estDist = prevEstDist + (distAddition)/(double)count;
			return;
		}



	}

	void solveRangeMove(float &echoSpeed,float &echoDist, QuadFit<double> &qfit ){
		echoDist =  qfit.approxYVals[rangeEvalMult -2 ] - qfit.approxYVals[rangeEvalMult -1 ] ;
		echoSpeed = -qfit.calculateD(4) * rangeFreq;

	}

	void ALTmergeRangeAndAccel(double echoConfA,double echoConfB, double lidarConf ){
		if (echoConfA >= HIGH_CONF && echoConfB >= HIGH_CONF && lidarConf >= HIGH_CONF){
			Serial.println("ALTUse Accel");
			return;// Just use accel data
		}

		float estSpeedEchoA,estSpeedEchoB,estSpeedLidar,estDistEchoA,estDistEchoB,estDistLidar;

		if (lidarConf < .01 ) {// zero check
			ALTsolveRangeMove(estSpeedLidar,estDistLidar,qfitL);
			ALTestSpeed = estSpeedLidar;
			ALTestDist = ALTprevEstDist + estDistLidar;
			Serial.println("ALTChose Lidar with perfect match");
			return;
		}

		if (echoConfA < .01 ) {// zero check
			ALTsolveRangeMove(estSpeedEchoA,estDistEchoA,qfitA);
			ALTestSpeed = estSpeedEchoA;
			ALTestDist = ALTprevEstDist + estDistEchoA;
			Serial.println("ALTChose Echo A with perfect match");
			return;
		}

		if (echoConfB < .01 ) {// zero check
			ALTsolveRangeMove(estSpeedEchoB,estDistEchoB,qfitB);
			ALTestSpeed = estSpeedEchoB;
			ALTestDist = ALTprevEstDist + estDistEchoB;
			Serial.println("ALTChose Echo B with perfect match");
			return;
		}

		uint8_t trustLevelA, trustLevelB, trustLevelL;
		if (echoConfA < ABS_CONF)
			trustLevelA = 3;
		else if (echoConfA < HIGH_CONF)
			trustLevelA = 2;
		else if (echoConfA < LOW_CONF)
			trustLevelA = 1;
		else 
			trustLevelA = 0;

		if (echoConfB < ABS_CONF)
			trustLevelB = 3;
		else if (echoConfB < HIGH_CONF)
			trustLevelB = 2;
		else if (echoConfB < LOW_CONF)
			trustLevelB = 1;
		else 
			trustLevelB = 0;

		if (lidarConf < ABS_CONF)
			trustLevelL = 3;
		else if (lidarConf < HIGH_CONF)
			trustLevelL = 2;
		else if (lidarConf < LOW_CONF)
			trustLevelL = 1;
		else 
			trustLevelL = 0;

		Serial.print("ALTTrust Level A:");
		Serial.print(trustLevelA);
		Serial.print("ALTTrust Level B:");
		Serial.print(trustLevelB);
		Serial.print("ALTTrust Level L:");
		Serial.println(trustLevelL);

		uint8_t maxTrustLevel = trustLevelA;
		if (trustLevelB>maxTrustLevel)
			maxTrustLevel = trustLevelB;
		if (trustLevelL>maxTrustLevel)
			maxTrustLevel = trustLevelL;


		if (maxTrustLevel == 3){
			uint8_t count = 0;
			ALTestSpeed = 0;
			ALTestDist = 0;

			if (trustLevelA == 3){
				ALTsolveRangeMove(estSpeedEchoA,estDistEchoA,qfitA);
				ALTestSpeed += estSpeedEchoA;
				ALTestDist += ALTprevEstDist + estDistEchoA;
				count++;
			} 
			if (trustLevelB == 3){
				ALTsolveRangeMove(estSpeedEchoB,estDistEchoB,qfitB);
				ALTestSpeed += estSpeedEchoB;
				ALTestDist += ALTprevEstDist + estDistEchoB;
				count++;
			} 
			if (trustLevelL == 3){
				ALTsolveRangeMove(estSpeedLidar,estDistLidar,qfitL);
				ALTestSpeed += estSpeedLidar;
				ALTestDist += ALTprevEstDist + estDistLidar;
				count++;
			} 
			if (count>1){
				ALTestSpeed/=(double)count;
				ALTestDist/=(double)count;
			}
			return;
		} else if (maxTrustLevel == 2){
			uint8_t count = 1;
			double distAddition=ALTestDist-ALTprevEstDist;

			if (trustLevelA == 2){
				ALTsolveRangeMove(estSpeedEchoA,estDistEchoA,qfitA);
		 		ALTestSpeed +=estSpeedEchoA;
				distAddition+= estDistEchoA;
				count++;
			} 

			if (trustLevelB == 2){
				ALTsolveRangeMove(estSpeedEchoB,estDistEchoB,qfitB);
				ALTestSpeed  +=estSpeedEchoB;
				distAddition+= estDistEchoB;
				count++;
			} 	
			if (trustLevelL == 2){
				ALTsolveRangeMove(estSpeedLidar,estDistLidar,qfitL);
		 		ALTestSpeed  +=estSpeedLidar;
				distAddition+= estDistLidar;
				count++;
			}
			ALTestSpeed/=(double)count;
			ALTestDist = ALTprevEstDist + (distAddition)/(double)count;
			return;
		}


	}

	void ALTsolveRangeMove(float &echoSpeed,float &echoDist, QuadFit<double> &qfit ){
	
			echoDist = qfit.yData[0] - qfit.yData[ rangeEvalMult - 1];

			echoSpeed=qfit.yData[rangeEvalMult-4]-qfit.yData[rangeEvalMult-3];
			for (int i = rangeEvalMult-2; i < rangeEvalMult; i++){
				echoSpeed=(echoSpeed+qfit.yData[i-1]-qfit.yData[i])/2.0;
			}
	
				echoSpeed = echoSpeed * (double)(rangeFreq);
		
	}

	void shift(uint8_t size, QuadFit<double> &qfit){
		for (uint8_t i =0 ; i <size-1 ; i++){
			qfit.yData[i] = qfit.yData[i+1];
		}
		qfit.yData[size-1] = 0;
	}

	void presample(){
		prevEstSpeed = 0;
		prevEstDist = 0;
		estSpeed = 0;
		estDist = 0;
		for (uint8_t i = 0; i < rangeEvalMult-1 ; i++){
			qfitA.yData[i] = qfitA.yData[rangeEvalMult-1];
			qfitB.yData[i] = qfitB.yData[rangeEvalMult-1];
			qfitL.yData[i] = qfitL.yData[rangeEvalMult-1];
		}
		printStarted = true;
	}

	void start(){
		estDir = -9999;
		cycleCount = 0;
		rangeSampleCount = 0;
		isMoving = true;
		currentThrottle = initThrottle;
		prevEstSpeed = 0;
		prevEstDist = 0;
		estSpeed = 0;
		estDist = 0;
		ALTprevEstSpeed = 0;
		ALTprevEstDist = 0;
		ALTestSpeed = 0;
		ALTestDist = 0;
		prevAccelYData=0;
		qfitA.yData[rangeEvalMult-1]  = 0;
		qfitB.yData[rangeEvalMult-1]  = 0;
		qfitL.yData[rangeEvalMult-1]  = 0;
		runSensorTasks();
		controller->schedule(NAV_ID,BASE_DELAY+baseInterval,baseInterval,false,0,Controller::newString("MOVE"),'N',true);
					motor->m0F = true;
					motor->m1F = true;
					motor->m0Speed = initThrottle;
					motor->m1Speed = initThrottle;

	}

	void runSensorTasks(){
		echo->startScan(ECHO_ID,echoDelay,echoMult*baseInterval,echoABDelay,0,false);
		lidar->startScan(LIDAR_ID,BASE_DELAY + lidarDelay,lidarMult*baseInterval,0,true);
		controller->schedule(LIA_ID,BASE_DELAY + accelDelay,accelMult*baseInterval,false,0,Controller::newString("LIA"),'G',false);
		controller->schedule(GYRO_ID,BASE_DELAY + gyroDelay,gyroMult*baseInterval,false,0,Controller::newString("H"),'G',false);
	}



	void stop(){
		isMoving = false;

		controller->kill(ECHO_ID);
		controller->kill(ECHO_ID+1);
		controller->kill(LIA_ID);
		controller->kill(GYRO_ID);
		controller->kill(LIDAR_ID);
		controller->kill(NAV_ID);
		motor->stopMotor();
	}
		


};

	

#endif