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
  Wire.setDefaultTimeout(10000);
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

  controller.loadControlled('C',&calc);
  controller.loadControlled('D',&disp);
  controller.loadControlled('T',&thermo);

  Serial.println("Modules have started!!");

//    controller.run(2,Controller::newString("INT 0 #U5000,#F10/#U7500,#F6~#U8500,#F5~#U9500,#F4~#U11000,#F3~#U12500,#F2~#U14000,#F1~#U17200,#F0/#U19000,#F-1"),'C');
//    controller.run(2,Controller::newString("INT 1 #U2600,#F10/#U3200,#F6~#U4325,#F5~#U4700,#F4~#U5150,#F3~#U5950,#F2~#U7000,#F1~#U8100,#F0/#U9000,#F-1"),'C');


  controller.run(2,Controller::newString("FUN P0 w[$UP:AAA={#F3600+{#F500*{{$FC:VRB-$FC:VRA}^#B3}}}]"),'C');
  controller.run(2,Controller::newString("FUN P1 w[$UP:BBB={#F3600+{#F500*{{$FC:VRA-$FC:VRB}^#B3}}}]"),'C');
  // controller.run(2,Controller::newString("SET $FC:VRM 3800"),'C');
//   controller.run(2,Controller::newString("FUN D5 w[$UP:AAA={$UP:AAA+#U100}]"),'C');
//   controller.run(2,Controller::newString("FUN D6 w[$UP:AAA={$UP:AAA-#U10}]"),'C');

      controller.schedule(610,1000,1000,false,0,Controller::newString("SET $FC:VRA i0[$UA:AAA]"),'C');
      controller.schedule(611,1000,1000,false,0,Controller::newString("SET $FC:VRB i1[$UA:BBB]"),'C');
//       controller.schedule(610,100,100,false,0,Controller::newString("SET $FC:VRA $UA:AAA"),'C');
//      controller.schedule(611,100,100,false,0,Controller::newString("SET $FC:VRB $UA:BBB"),'C');

      controller.schedule(614,1000,1000,false,0,Controller::newString("EXE P0"),'C');
   controller.schedule(615,1000,1000,false,0,Controller::newString("EXE P1"),'C');

      controller.schedule(616,1000,10,false,0,Controller::newString("SET $UP:EEE #L0" ),'C');
      controller.schedule(616,1000,10,false,0,Controller::newString("SET $UP:FFF #L0"),'C');
     // controller.schedule(616,1000,10,false,0,Controller::newString("SET $UP:GGG #L0"),'C');
      controller.schedule(616,1000,10,false,0,Controller::newString("SET $UP:HHH #L0"),'C');
      controller.schedule(616,1000,10,false,0,Controller::newString("SET $UP:III #L0"),'C');
      controller.schedule(616,1000,10,false,0,Controller::newString("SET $UP:JJJ #L0"),'C');
      controller.schedule(616,1000,10,false,0,Controller::newString("SET $UP:KKK #L0"),'C');
      controller.schedule(616,1000,10,false,0,Controller::newString("SET $UP:GGG {t%#L4096}"),'C');


  controller.run(2,Controller::newString("FRQ 20"),'P');


 controller.execute();

  controller.run(2,Controller::newString("B BRT 255"),'D',2);
  controller.run(2,Controller::newString("B ROT 1"),'D',2);

    controller.run(2,Controller::newString("0 2 2 T B9 \" Red      Grn       Blu      DBl    UV     Wht     Btm\" 255,255,255 0,0,0"),'D');
 controller.run(2,Controller::newString("2 8 44 T 8 $UP:EEE 255,0,0 0,0,0"),'D');

 controller.run(2,Controller::newString("5 56 44 T 8 $UP:FFF 100,205,0 0,0,0"),'D');

 controller.run(2,Controller::newString("8 104 44 T 8 $UP:GGG 0,150,250 0,0,0"),'D');

 controller.run(2,Controller::newString("11 152 44 T 8 $UP:HHH 0,0,200 0,0,0"),'D');

 controller.run(2,Controller::newString("14 200 44 T 8 $UP:III 200,0,250 0,0,0"),'D');

  controller.run(2,Controller::newString("17 248 44 T 8 $UP:JJJ 250,250,250 0,0,0"),'D');

  controller.run(2,Controller::newString("20 306 44 T 8 $UP:KKK 200,150,40 0,0,0"),'D');


 controller.execute();

 controller.run(2,Controller::newString("30 2 82 T B9 \" Pumps:  Left      Right\" 255,255,255 0,0,0"),'D');

 controller.run(2,Controller::newString("32 62 124 T 8 $UP:AAA 255,210,180 0,0,0"),'D');

 controller.run(2,Controller::newString("FUN 31 UC:FND 5"),'D');
 controller.run(2,Controller::newString("FUN 33 UC:FND 6"),'D');
//
// controller.run(2,Controller::newString("34 96 100 B 35,20 10 \"   +\" 100,60,25 255,210,180"),'D');
 controller.run(2,Controller::newString("35 102 124 T 8 $UP:BBB 255,210,180 0,0,0"),'D');
//   controller.run(2,Controller::newString("36 96 136 B 35,20 10 \"   -\" 100,60,25 255,210,180"),'D');
// controller.run(2,Controller::newString("FUN 34 UC:FND 6"),'D');
// controller.run(2,Controller::newString("FUN 36 UC:FND 6"),'D');
 controller.run(2,Controller::newString("80 0 218 T 8 $SZ:INP 0,255,0 0,10,160"),'D');
   controller.run(2,Controller::newString("81 0 230 T 8 $SZ:ETX 255,0,0 10,10,10"),'D');
    controller.run(2,Controller::newString("82 260 230 T B8 $TZ:ETM 200,0,0 10,10,10"),'D');

   controller.run(2,Controller::newString("38 180 124 T 8 $FC:VRA 255,210,180 0,0,0"),'D');
   controller.run(2,Controller::newString("39 240 124 T 8 $FC:VRB 255,210,180 0,0,0"),'D');
   

 controller.run(2,Controller::newString("40 2 162 T B9 \" Top Tmp    Btm Tmp\" 255,255,255 0,0,0"),'D');
 controller.run(2,Controller::newString("41 20 178 T 8 $FT:AAA 180,210,240 0,0,0"),'D');
 controller.run(2,Controller::newString("42 90 178 T 8 $FT:BBB 180,210,240 0,0,0"),'D');
 controller.execute();
 controller.schedule(1,3,3,false,0,Controller::newString("D"),'D');
 controller.run(2,Controller::newString("Enable"),'T');

  controller.run(2,Controller::newString("B ZZZ 1 0"),'Z',1);

}

void loop() {
        if (Serial1.available()){
          controller.processInput(&Serial1);
        }
        controller.execute();
}
