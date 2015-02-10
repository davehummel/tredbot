#include <dh_movement.h>
#include <dh_logger.h>
#include <dh_arduino_logger.h>

uint16_t flash=40;

void setup(){ 
	pinMode(13, OUTPUT);
	Serial1.begin(115200); 
	Serial1.print("Test Start\n");
	Logger* logger = new ArduinoLogger(1);
	logger->print("Logger!\n");
	MovementEval movement = MovementEval(5,1,1,200,10);
	movement.logger = logger;
		Serial1.println("Getting Movement");
	bool temp = movement.getMoving();

	if (temp){
		Serial1.println("Error:Detected movement at before start");
		return;
	}
	Serial1.println("Eval Gyro"); 
	 temp = movement.evaluateGyro(0,0,0,1);
	if (temp){
		Serial1.println("Error:Detected movement at first");
		return;
	}
	Serial1.println("Getting Movement");
	temp = movement.getMoving();

	if (temp){
		Serial1.println("Error:Detected movement at first"); 
		return;
	}

	 temp = movement.evaluateGyro(0,0,.9,15);
 
	if (temp){
		Serial1.println("Error:Detected movement at 1");
		return;
	}


	 temp = movement.evaluateGyro(0,0,.9,30);

	if (temp){
		Serial1.println("Error:Detected movement at 2");
		return;
	}

	 temp = movement.evaluateGyro(0,0,.9,41);

	if (temp){ 
		Serial1.println("Error:Detected movement at 3");
		return;
	}


	 temp = movement.evaluateGyro(0,0,.9,71);

	if (temp){
		Serial1.println("Error:Detected movement at 4");
		return;
	}

	 temp = movement.evaluateGyro(0,0,.9,98);

	if (temp){ 
		Serial1.println("Error:Detected movement at 5");
		return;
	}

		 temp = movement.evaluateGyro(0,0,.9,110);

	if (!temp){
		Serial1.println("Error:Should detect movement at 5.4");
		return;
	}
	 if (movement.getMoving() == false ){
	 			Serial1.println("Get moving should be true");
		return;
	 }	 if (movement.getMoving() == false ){
	 		
	 			Serial1.println("Get moving should be true");
		return; 
	 }
	 
	 movement.printState(); 
 
	  temp = movement.evaluateGyro(0,0,1.8,111);

	  	 movement.printState();

	 if (movement.getMoving() == false ){
	 		
	 			Serial1.println("Get moving should be true");
		return;
	 }

	 temp = movement.evaluateGyro(0,0,1.8,270);

	 movement.printState();

	if (movement.getMoving() == false ){
	 	Serial1.println("Get moving should be true");
		return;
	 }



	 temp = movement.evaluateGyro(0,0,1.6,312);

	if (!temp){
		Serial1.println("Error:Should stop stopping movement"); 
		return;
	}

	if (movement.getMoving() == true ){
	 	Serial1.println("Get moving should be false");
		return;
	}


	temp = movement.evaluateGyro(0,0,-.4,350);

	if (temp || movement.getMoving() == true ){
	 	Serial1.println("Get moving should be false");
		return;
	}

	movement.printState();

	temp = movement.evaluateGyro(0,0,-.8,560);

	if (temp || movement.getMoving() == true ){
	 	Serial1.println("Get moving should be false");
		return;
	}


	movement.printState();

	flash = 1000;

	Serial1.println("Done");

}

void loop() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(flash);               // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(flash);               // wait for a second
}
