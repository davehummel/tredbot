#include <dh_movement.h>
#include <dh_logger.h>
#include <dh_arduino_logger.h>
#include <dh_position.h>
#include <quaternion.h>
#include <dh_averaged_measure.h> 
#include <dh_sensor_processor.h>
 
uint16_t flash=40;

void setup(){ 
	pinMode(13, OUTPUT);
	Serial1.begin(115200);  
	Serial1.print("Test Start\n");
	Logger* logger = new ArduinoLogger(1); 
	logger->print("Logger!\n");
 
	SensorProcessor sp = SensorProcessor(100); 
	sp.logger = logger;
	sp.movement = new MovementEval(5,1,1,100,20);
	sp.movement->logger = logger;

	sp.processGyro((int16_t) 105,(int16_t)100,(int16_t)100,(uint32_t)1);

	sp.printState();

	if (sp.getReady()){
		Serial1.println("Failed: should not be ready yet");
		return;
	} 

	for (int16_t i = 0; i < 20;i++){
		sp.processGyro(50,-i,i,i*10+11);

		if (sp.movement->getMoving()){
			Serial1.print ("Movement detected to early on iteration ");
			Serial1.print( i);
			Serial1.println();
			return;
		}

	}  

	sp.printState();

	flash = 1000; 

	Serial1.println("Done");

}

void loop() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(flash);               // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(flash);               // wait for a second
}
