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
ControlledLED led;
ControlledTouchLCD disp;
ControlledCalc calc;

Controller controller;


void setup() {

  Serial.begin(115200);
  delay(1000);

Serial.println("Starting Controlled Modules");

  controller.loadControlled('Z',&os);
  controller.loadControlled('B',&led); 
  controller.loadControlled('C',&calc);
  controller.loadControlled('D',&disp); 

  Serial.println("Modules have started!!");

  controller.run(2,Controller::newString("FUN D0 ?{$BD:BTN==#B1}[?{$UB:RRR<#U4096}[w[$UB:RRR={#I128+$UB:RRR}],#B0],?{$UB:RRR>#U0}[w[$UB:RRR={#I-128+$UB:RRR}],#B0]]"),'C');
   controller.run(2,Controller::newString("FUN D1 ?{$BD:BTN==#B4}[?{$UB:GGG<#U4096}[w[$UB:GGG={#I128+$UB:GGG}],#B0],?{$UB:GGG>#U0}[w[$UB:GGG={#I-128+$UB:GGG}],#B0]]"),'C');
   controller.run(2,Controller::newString("FUN D2 ?{$BD:BTN==#B7}[?{$UB:BBB<#U4096}[w[$UB:BBB={#I128+$UB:BBB}],#B0],?{$UB:BBB>#U0}[w[$UB:BBB={#I-128+$UB:BBB}],#B0]]"),'C');
   controller.run(2,Controller::newString("FUN D3 ?{$BD:BTN==#B10}[?{$UB:WWW<#U4096}[w[$UB:WWW={#I128+$UB:WWW}],#B0],?{$UB:WWW>#U0}[w[$UB:WWW={#I-128+$UB:WWW}],#B0]]"),'C');
  controller.run(2,Controller::newString("FUN D4 ?{$BD:BTN==#B13}[?{$UB:UUU<#U4096}[w[$UB:UUU={#I128+$UB:UUU}],#B0],?{$UB:UUU>#U0}[w[$UB:UUU={#I-128+$UB:UUU}],#B0]]"),'C');
  
  controller.run(2,Controller::newString("RES 12"),'B');
  controller.run(2,Controller::newString("FRQ 3 1000"),'B');
  controller.run(2,Controller::newString("FRQ 5 3500"),'B');
  controller.run(2,Controller::newString("PWM A 3"),'B');
  controller.run(2,Controller::newString("PWM B 4"),'B');
  controller.run(2,Controller::newString("PWM U 5"),'B');
  controller.run(2,Controller::newString("PWM W 20"),'B');
  controller.run(2,Controller::newString("PWM R 23"),'B');
  controller.run(2,Controller::newString("PWM G 22"),'B');
  controller.run(2,Controller::newString("PWM B 21"),'B');
  
 controller.execute(&Serial1); 
 
  controller.run(2,Controller::newString("B BRT 255"),'D',2);
  controller.run(2,Controller::newString("B ROT 1"),'D',2);
   
    controller.run(2,Controller::newString("0 2 2 T B9 \" Red      Grn       Blu      Wht     Btm\" 255,255,255 0,0,0"),'D');
   controller.run(2,Controller::newString("1 2 20 B 35,20 10 \"   +\" 255,255,255 255,100,100"),'D');
 controller.run(2,Controller::newString("2 8 44 T 8 $UB:RRR 255,0,0 0,0,0"),'D');
   controller.run(2,Controller::newString("3 2 56 B 35,20 10 \"   -\" 255,255,255 95,20,20"),'D');
 controller.run(2,Controller::newString("FUN 1 UC:FND 0"),'D');
 controller.run(2,Controller::newString("FUN 3 UC:FND 0"),'D');

  controller.run(2,Controller::newString("4 50 20 B 35,20 10 \"   +\" 255,255,255 100,255,100"),'D');
 controller.run(2,Controller::newString("5 56 44 T 8 $UB:GGG 0,255,0 0,0,0"),'D');
   controller.run(2,Controller::newString("6 50 56 B 35,20 10 \"   -\" 255,255,255 20,95,20"),'D');
 controller.run(2,Controller::newString("FUN 4 UC:FND 1"),'D');
 controller.run(2,Controller::newString("FUN 6 UC:FND 1"),'D');

 controller.run(2,Controller::newString("7 98 20 B 35,20 10 \"   +\" 255,255,255 100,100,255"),'D');
 controller.run(2,Controller::newString("8 104 44 T 8 $UB:BBB 0,0,255 0,0,0"),'D');
   controller.run(2,Controller::newString("9 98 56 B 35,20 10 \"   -\" 255,255,255 20,20,95"),'D');
 controller.run(2,Controller::newString("FUN 7 UC:FND 2"),'D');
 controller.run(2,Controller::newString("FUN 9 UC:FND 2"),'D');

 controller.run(2,Controller::newString("10 146 20 B 35,20 10 \"   +\" 100,100,100 255,255,255"),'D');
 controller.run(2,Controller::newString("11 152 44 T 8 $UB:WWW 255,255,255 0,0,0"),'D');
   controller.run(2,Controller::newString("12 146 56 B 35,20 10 \"   -\" 255,255,255 65,65,65"),'D');
 controller.run(2,Controller::newString("FUN 10 UC:FND 3"),'D');
 controller.run(2,Controller::newString("FUN 12 UC:FND 3"),'D');

 controller.run(2,Controller::newString("13 194 20 B 35,20 10 \"   +\" 0,0,0 255,150,150"),'D');
 controller.run(2,Controller::newString("14 200 44 T 8 $UB:UUU 255,150,150 0,0,0"),'D');
   controller.run(2,Controller::newString("15 194 56 B 35,20 10 \"   -\" 255,155,155 65,0,0"),'D');
 controller.run(2,Controller::newString("FUN 13 UC:FND 4"),'D');
 controller.run(2,Controller::newString("FUN 15 UC:FND 4"),'D');
  
 controller.execute(&Serial1); 

 controller.run(2,Controller::newString("18 245 33 B 60,32 12 \"Manual\" 255,255,0 155,155,0"),'D');
 

 controller.schedule(1,3,3,false,0,Controller::newString("D"),'D');


}

void loop() {
        if (Serial1.available()){ 
          controller.processInput(&Serial1);
        }
        controller.execute(&Serial1);  
}
