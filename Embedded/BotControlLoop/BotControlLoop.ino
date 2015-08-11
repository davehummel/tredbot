

 #include <i2c_t3.h>
 #include <Arduino.h>
 #include "quadfit.h"

 #include <dh_ax-12a.h>
 #include <dh_qik2s12v10.h>
 #include <dh_controller.h>
 #include <ControlledLED.h>
 #include <ControlledVMeter.h>
 #include <ControlledPanTilt.h>
 #include <ControlledLidar.h>
 #include <ControlledMotor.h>
 #include <ControlledI2CXL.h>
 #include <ControlledBN055.h>
 #include <ControlledNavigator.h>



Controller controller;

void setup(){
	Serial.begin(115200);
	Serial1.begin(460800);
	delay(500);
	
	controller.loadControlled('B', new ControlledLED());
	controller.loadControlled('G', new ControlledBN055());
	controller.loadControlled('V', new ControlledVMeter());
	controller.loadControlled('L', new ControlledLidar());
	controller.loadControlled('S', new ControlledI2CXL());
	controller.loadControlled('P', new ControlledPanTilt());
	controller.loadControlled('M', new ControlledMotor());
	controller.loadControlled('N', new ControlledNavigator());
	controller.schedule(1,0,1000,false,0,Controller::newString("blink"),'B',true);
	// Schedule a task to keep reading the error value from the motor
	controller.schedule(3, 500, 200, false, 0, Controller::newString("ERROR"), 'M', true);
	// Zero the gyro
    controller.schedule(2,200,0,false,1,Controller::newString("ZERO 20 20"),'N',true);
	// Schedule a heading calculation to get things setup
	

	
	Serial1.println("Starting!");
	Serial.println("Starting!");
	
	
		//controller.schedule(250,100/4,100/4,false,100,Controller::newString("SCAN"),'L',true);
}


void loop(){
        if (Serial1.available()){
          controller.processInput(&Serial1);
        }
		
        controller.execute(&Serial1);


}


 
