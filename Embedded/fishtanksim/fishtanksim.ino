#include "dh_logger.h"
#include "dh_controller.h"

#include "ControlledOSStatus.h"

#include "ControlledCalc.h"


Logger logger;
ControlledOSStatus os;

ControlledCalc calc;

Controller controller;


void setup() {

  Serial.begin(115200);
  Serial1.begin(115200);

    delay(100);

  controller.setOutputStream(&Serial);
  logger.setStream(&Serial1);


Serial.println("Starting Controlled Modules");

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
