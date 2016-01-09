

#include "dh_logger.h"
#include "dh_controller.h"

#include "ControlledLED.h"
#include "ControlledCalc.h"


Logger logger;
ControlledLED led(21,22,23);
ControlledCalc calc;
Controller controller;
ADDR1* addrs[2];
void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  logger.setStream(&Serial1); 
delay(2000);

  controller.loadControlled('B',&led); 
  controller.loadControlled('C',&calc);

  controller.run(27,Controller::newString("INT 0 #D-1,#D0|#D0,#D0~#D0.5,#D255~#D1,#D0|#D2,#D0"),'C');
//  
  controller.run(28,Controller::newString("SET DC:VR0 i0[#D0.2]"),'C');

//  controller.run(29,Controller::newString("FUN C1 w[FC:VRA=#F1.12]"),'C');
//
// // controller.schedule(3,0,3000,false,0,Controller::newString("BLINK"),'B');
//
//  controller.schedule(3,0,2000,false,0,Controller::newString("EXE C0"),'C');
//  controller.schedule(4,1000,2000,false,0,Controller::newString("EXE C1"),'C');
//  controller.schedule(5,0,1,false,0,Controller::newString("EXE B0"),'C');
}

void loop() {
        if (Serial.available()){
          controller.processInput(&Serial);
        }
        controller.execute(&Serial1);  
}
