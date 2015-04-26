
 #include <i2c_t3.h>
 #include <Arduino.h>
 #include <ControlledLED.h>
 #include <ControlledVMeter.h>
 #include <dh_ax-12a.h>
 #include <dh_qik2s12v10.h>
 #include <dh_controller.h>
 #include <ControlledPanTilt.h>
 #include <ControlledLidar.h>
 #include <ControlledMotor.h>
 #include <ControlledI2CXL.h>
 #include <ControlledNavigator.h>


 #include <i2c_t3.h>

Controller controller;

void setup(){

	Serial1.begin(460800);

	controller.loadControlled('B', new ControlledLED());
	controller.loadControlled('V',new ControlledVMeter());
	controller.loadControlled('L',new ControlledLidar());
	controller.loadControlled('S',new ControlledI2CXL());
	controller.loadControlled('P', new ControlledPanTilt());
	controller.loadControlled('M', new ControlledMotor());
	controller.loadControlled('N', new ControlledNavigator());
	controller.schedule(1,0,1000,false,0,Controller::newString("blink"),'B',false);

	Serial1.println("Starting!");
}


void loop(){
        if (Serial1.available()){
          controller.processInput(&Serial1);
        }


        controller.execute(&Serial1);
        
}

