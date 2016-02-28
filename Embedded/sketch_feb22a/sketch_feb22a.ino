#include "SPI.h"
#include "ILI9341_t3.h"
#include <XPT2046_Touchscreen.h>
#include "CTFont.h"

#include "dh_logger.h"
#include "dh_controller.h"

#include "ControlledOSStatus.h"
#include "ControlledLED.h"
#include "ControlledCalc.h"
#include "ControlledTouchLCD.h"


Logger logger;
ControlledOSStatus os;
ControlledLED led(6,5,4,13,3);
ControlledTouchLCD disp;
ControlledCalc calc;

Controller controller;


void setup() {


  Serial.begin(115200);

  delay(1000);
     
  logger.setStream(&Serial); 
delay(2000);

Serial.println("Starting Controlled Modules");

  controller.loadControlled('Z',&os);
  controller.loadControlled('B',&led); 
  controller.loadControlled('C',&calc);
  controller.loadControlled('D',&disp); 

  Serial.println("Modules have started!!");

  controller.run(2,Controller::newString("B BRT 255"),'D',2);
  controller.run(2,Controller::newString("B ROT 1"),'D',2);
  controller.run(27,Controller::newString("FUN D0 ?{$TD:BTN>{t-#T11}}[?{$BD:BTN==#B0}[w[$BB:RRR={$BB:RRR+#B1}],?{$BD:BTN==#B1}[w[$BB:GGG={$BB:GGG+#B1}],?{$BD:BTN==#B2}[w[$BB:BBB={$BB:BBB+#B1}],?{$BD:BTN==#B3}[w[$BB:WWW={$BB:WWW+#B1}],#B0]]]],#B0]"),'C');
   controller.run(2,Controller::newString("0 40 40 B 45,28 B8 $BB:RRR 255,255,255 $BB:RRR,0,0"),'D');
  controller.run(2,Controller::newString("1 90 40 B 45,28 B8 $BB:GGG 255,255,255 0,$BB:GGG,0"),'D');
  controller.run(2,Controller::newString("2 140 40 B 45,28 B8 $BB:BBB 255,255,255 0,0,$BB:BBB"),'D');
    controller.run(2,Controller::newString("3 190 40 B 45,28 B8 $BB:WWW 255,255,255 $BB:WWW,$BB:WWW,$BB:WWW"),'D');
  controller.schedule(2,0,10,false,0,Controller::newString("EXE D0"),'C');
 controller.schedule(1,3,3,false,0,Controller::newString("D"),'D');

}

void loop() {
        if (Serial.available()){ 
          controller.processInput(&Serial);
        }
        controller.execute(&Serial);  
}
