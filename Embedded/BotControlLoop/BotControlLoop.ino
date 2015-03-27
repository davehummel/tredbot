 #include <i2c_t3.h>
 #include <Arduino.h>

 #include <dh_controller.h>
 #include <ControlledLED.h>
 #include <ControlledLidar.h>



Controller controller;

void setup(){
	Serial1.begin(115200);
	controller.library[0] = new ControlledLED();
	controller.library[0]->begin();
	// controller.library[1] = new ControlledLidar();
	// controller.library[1]->begin();
	controller.schedule(1,1000,10000,0,"blink",controller.library[0],false);
	//controller.schedule(100,1000,0,0,"START",library[1],false);
	Serial1.println("Starting!");
}


elapsedMillis time;
void loop(){
        if (Serial1.available()){
          controller.process(&Serial1);
        }
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
