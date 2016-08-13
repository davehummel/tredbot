#include "SPI.h"
#include <i2c_t3.h>
#include "ILI9341_t3.h"
#include <XPT2046_Touchscreen.h>
#include "CTFont.h"
#include <OneWire.h>

#include "dh_logger.h"
#include "dh_controller.h"

#include "ControlledOSStatus.h"
#include "ControlledLED.h"
#include "ControlledI2CPWM.h"
#include "ControlledI2CADC.h"
#include "ControlledCalc.h"
#include "ControlledTouchLCD.h"
#include "ControlledThermometer.h"

Logger logger;
ControlledOSStatus os;
ControlledLED led;
ControlledI2CPWM pwm;
ControlledI2CADC adc;

ControlledTouchLCD disp;
ControlledCalc calc;
byte thermoIDs[2] = { 0xAD , 0xC5 };
ControlledThermometer thermo(17,thermoIDs, 2);

Controller controller;


void setup() {

  Serial.begin(115200);
  Serial1.begin(115200);
  Wire.begin();
  
    delay(100);

  controller.setOutputStream(&Serial1);
  logger.setStream(&Serial1);

  
  disp.tch_cs = 8;
  disp.tft_dc = 9;
  disp.tft_cs = 10;

  pwm.setWire(&Wire);
  adc.setWire(&Wire);

Serial.println("Starting Controlled Modules");

  controller.loadControlled('Z',&os);
  controller.loadControlled('A',&adc);
  controller.loadControlled('P',&pwm);
  controller.loadControlled('B',&led);
  controller.loadControlled('C',&calc);
  controller.loadControlled('D',&disp);
  controller.loadControlled('T',&thermo);

  Serial.println("Modules have started!!");

  controller.run(2,Controller::newString("FUN D0 ?{$BD:BTN==#B1}[?{$UB:RRR<#U4096}[w[$UB:RRR={#I128+$UB:RRR}],#B0],?{$UB:RRR>#U0}[w[$UB:RRR={#I-128+$UB:RRR}],#B0]]"),'C');
   controller.run(2,Controller::newString("FUN D1 ?{$BD:BTN==#B4}[?{$UB:GGG<#U4096}[w[$UB:GGG={#I128+$UB:GGG}],#B0],?{$UB:GGG>#U0}[w[$UB:GGG={#I-128+$UB:GGG}],#B0]]"),'C');
   controller.run(2,Controller::newString("FUN D2 ?{$BD:BTN==#B7}[?{$UB:BBB<#U4096}[w[$UB:BBB={#I128+$UB:BBB}],#B0],?{$UB:BBB>#U0}[w[$UB:BBB={#I-128+$UB:BBB}],#B0]]"),'C');
   controller.run(2,Controller::newString("FUN D3 ?{$BD:BTN==#B10}[?{$UB:WWW<#U4096}[w[$UB:WWW={#I128+$UB:WWW}],#B0],?{$UB:WWW>#U0}[w[$UB:WWW={#I-128+$UB:WWW}],#B0]]"),'C');
  controller.run(2,Controller::newString("FUN D4 ?{$BD:BTN==#B13}[?{$UB:UUU<#U4096}[w[$UB:UUU={#I128+$UB:UUU}],#B0],?{$UB:UUU>#U0}[w[$UB:UUU={#I-128+$UB:UUU}],#B0]]"),'C');
   controller.run(2,Controller::newString("FUN D5 ?{$BD:BTN==#B31}[?{$UP:AAA<#U4096}[w[$UP:AAA={#I2+$UP:AAA}],#B0],?{$UP:AAA>#U0}[w[$UP:AAA={#I-2+$UP:AAA}],#B0]]"),'C');
    controller.run(2,Controller::newString("FUN D6 ?{$BD:BTN==#B34}[?{$UB:YYY<#U4096}[w[$UB:YYY={#I2+$UB:YYY}],#B0],?{$UB:YYY>#U0}[w[$UB:YYY={#I-2+$UB:YYY}],#B0]]"),'C');

      controller.schedule(4,1000,10,false,1,Controller::newString("SET UP:AAA #U3890"),'C');

      controller.schedule(4,1000,10,false,1,Controller::newString("SET UP:BBB #U3890"),'C');
  
  controller.run(2,Controller::newString("FRQ 1600"),'P');
  controller.run(2,Controller::newString("RES 12"),'B');
  controller.run(2,Controller::newString("FRQ 3 1000"),'B');
  controller.run(2,Controller::newString("FRQ 5 3500"),'B');
  controller.run(2,Controller::newString("PWM U 5"),'B');
  controller.run(2,Controller::newString("PWM W 20"),'B');
  controller.run(2,Controller::newString("PWM R 23"),'B');
  controller.run(2,Controller::newString("PWM G 22"),'B');
  controller.run(2,Controller::newString("PWM B 21"),'B');

 controller.execute();

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

 controller.execute();

 controller.run(2,Controller::newString("18 245 33 B 60,32 12 \"Manual\" 255,255,0 155,155,0"),'D');
 controller.run(2,Controller::newString("30 2 82 T B9 \" Pumps:  Left      Right\" 255,255,255 0,0,0"),'D');

 controller.run(2,Controller::newString("31 56 100 B 35,20 10 \"   +\" 100,60,25 255,210,180"),'D');
 controller.run(2,Controller::newString("32 62 124 T 8 $UP:AAA 255,210,180 0,0,0"),'D');
   controller.run(2,Controller::newString("33 56 136 B 35,20 10 \"   -\" 100,60,25 255,210,180"),'D');
 controller.run(2,Controller::newString("FUN 31 UC:FND 5"),'D');
 controller.run(2,Controller::newString("FUN 33 UC:FND 5"),'D');

 controller.run(2,Controller::newString("34 96 100 B 35,20 10 \"   +\" 100,60,25 255,210,180"),'D');
 controller.run(2,Controller::newString("35 102 124 T 8 $UP:BBB 255,210,180 0,0,0"),'D');
   controller.run(2,Controller::newString("36 96 136 B 35,20 10 \"   -\" 100,60,25 255,210,180"),'D');
 controller.run(2,Controller::newString("FUN 34 UC:FND 6"),'D');
 controller.run(2,Controller::newString("FUN 36 UC:FND 6"),'D');

   controller.run(2,Controller::newString("38 180 124 T 8 $UA:AAA 255,210,180 0,0,0"),'D');
   controller.run(2,Controller::newString("39 240 124 T 8 $UA:BBB 255,210,180 0,0,0"),'D');

 controller.run(2,Controller::newString("40 2 162 T B9 \" Top Tmp    Btm Tmp\" 255,255,255 0,0,0"),'D');
 controller.run(2,Controller::newString("41 20 178 T 8 $FT:AAA 180,210,240 0,0,0"),'D');
 controller.run(2,Controller::newString("42 90 178 T 8 $FT:BBB 180,210,240 0,0,0"),'D');
 controller.execute();
 controller.schedule(1,3,3,false,0,Controller::newString("D"),'D');
 controller.run(2,Controller::newString("Enable"),'T');

}

void loop() {
        if (Serial1.available()){
          controller.processInput(&Serial1);
        }
        controller.execute();
}
