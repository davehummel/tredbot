#include "dh_logger.h"
#include "dh_controller.h"

#include "ControlledOSStatus.h"

#include "ControlledCalc.h"
#include "ControlledLED.h"


Logger logger;
ControlledOSStatus os;

ControlledCalc calc;

ControlledLED led;

Controller controller;


void setup() {

  Serial.begin(115200);
  Serial1.begin(115200);

    delay(1000);
  
  controller.setOutputStream(&Serial);
  controller.setErrorStream(&Serial1);


Serial.println("Starting Controlled Modules");

  controller.loadControlled('A',&led);

  controller.loadControlled('P',&led);

  controller.loadControlled('Z',&os);

  controller.loadControlled('C',&calc);

  Serial.println("Modules have started!!");

}

void loop() {
        if (Serial.available()){
          controller.processInput(&Serial);
        }
        controller.execute();
}
