
 #include <i2c_t3.h>
 #include <Arduino.h>

 #include <dh_controller.h>
 #include <ControlledLED.h>
 #include <ControlledLidar.h>



Controller controller;

void setup(){
	Serial1.begin(115200);
	controller.library['B'-'A'] = new ControlledLED();
	controller.library['B'-'A']->begin();
	controller.library['L'-'A'] = new ControlledLidar();
	controller.library['L'-'A']->begin();
	controller.schedule(1,1000,10000,0,"blink",'B',false);
	Serial1.println("Starting!");
}


elapsedMillis time;

void loop(){
        if (Serial1.available()){
          controller.processInput(&Serial1);
        }
        controller.execute((uint32_t)time, &Serial1);
}

