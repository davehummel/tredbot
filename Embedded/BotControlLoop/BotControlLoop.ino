 #include <Arduino.h>
 #include <dh_controller.h>
 #include <ControlledLED.h>
#include <stdint.h>
#include <list>
Controller controller;

Controlled* library[25];

void setup(){
	Serial1.begin(115200);
	library[0] = new ControlledLED();
	controller.schedule(1,100,1000,0,"blink",library[0],false);
}


elapsedMicros time;
void loop(){
	vector<Controller::ControlledResponse>* responses = controller.execute((uint32_t)time);
	if (responses){
		for (vector<Controller::ControlledResponse>::iterator iter=responses->begin();iter!=responses->end();iter++){
			sendSerial(iter->id,iter->content);
		}
		delete responses;
	}
}

void sendSerial(uint32_t id,char content[]){
	Serial1.print(id);
	Serial1.write(' ');
	Serial1.println(content);

}