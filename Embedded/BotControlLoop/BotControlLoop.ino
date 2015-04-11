
 #include <i2c_t3.h>
 #include <Arduino.h>
 #include <ControlledLED.h>
 #include <ControlledVMeter.h>
 #include <dh_ax-12a.h>
 #include <dh_controller.h>
 #include <ControlledPanTilt.h>
 #include <ControlledLidar.h>
 #include <ControlledI2CXL.h>


 #include <i2c_t3.h>

Controller controller;

void setup(){
	Serial1.begin(460800);
	elapsedMillis m ;
	controller.library['B'-'A'] = new ControlledLED();
	controller.library['B'-'A']->begin();
	controller.library['V'-'A'] = new ControlledVMeter();
	controller.library['V'-'A']->begin();
	controller.library['L'-'A'] = new ControlledLidar();
	controller.library['L'-'A']->begin();
	controller.library['E'-'A'] = new ControlledI2CXL();
	controller.library['E'-'A']->begin();
	controller.library['S'-'A'] = new ControlledPanTilt();
	((ControlledPanTilt*)controller.library['S'-'A'])->lidar = (ControlledLidar*)controller.library['L'-'A'] ;
	controller.library['S'-'A']->begin();
	controller.schedule(1,0,1000,false,0,Controller::newString("blink"),'B',false);
	Serial1.println("Starting!");
}


void loop(){
        if (Serial1.available()){
          controller.processInput(&Serial1);
        }

        controller.execute(&Serial1);
        
}

