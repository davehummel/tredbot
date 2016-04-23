#include "SPI.h"
#include <i2c_t3.h>
#include "ILI9341_t3.h"
#include <XPT2046_Touchscreen.h>
#include "CTFont.h"

#include "dh_logger.h"
#include "dh_controller.h"

#include "ControlledOSStatus.h"
#include "ControlledLED.h"
#include "ControlledCalc.h"
#include "ControlledTouchLCD.h"
#include "ControlledFusion.h"
#include "BNO055Fusion.h"
#include "ControlledVMeter.h"
#include "dh_ax-12a.h"
#include "ControlledPanTilt.h"
#include "ControlledLidar.h"
#include "ControlledI2CXL.h"
#include "dh_qik2s12v10.h"
#include "ControlledMotor.h"

Logger logger;
ControlledOSStatus os;
ControlledLED led;
ControlledTouchLCD disp;
ControlledCalc calc;
BNO055Fusion fusion;
ControlledVMeter vMeter(A10,A11);
ControlledPanTilt panTilt;
ControlledLidar lidar(27);
ControlledI2CXL echo;
ControlledMotor motor(false,2);

Controller controller;


void setup() {
  pinMode(27,OUTPUT);
  digitalWrite(27,LOW);

  Serial.begin(115200);
  Serial1.begin(115200);
  delay(1000);
     
  Wire1.begin(I2C_MASTER, 0x00, I2C_PINS_29_30, I2C_PULLUP_EXT, I2C_RATE_1200,I2C_OP_MODE_DMA);
 // Wire1.setDefaultTimeout(100); 
  fusion.setWire(&Wire1);
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_800,I2C_OP_MODE_DMA);
 // Wire.setDefaultTimeout(100); 
  lidar.setWire(&Wire);
  echo.setWire(&Wire);
  logger.setStream(&Serial1); 

  disp.tch_cs = 17;
  disp.tft_dc = 20;
  disp.tft_cs = 15;
  
delay(200);

Serial.println("Starting Controlled Modules");

  controller.loadControlled('Z',&os);
  controller.loadControlled('B',&led); 
  controller.loadControlled('C',&calc);
  controller.loadControlled('D',&disp); 
  controller.loadControlled('F',&fusion);
  controller.loadControlled('V',&vMeter);
  controller.loadControlled('P',&panTilt);
  controller.loadControlled('L',&lidar);
  controller.loadControlled('E',&echo);
  controller.loadControlled('M',&motor);
  
  Serial.println("Modules have started!!");
  controller.run(2,Controller::newString("PWM R 21"),'B');
    controller.run(2,Controller::newString("PWM G 22"),'B');
      controller.run(2,Controller::newString("PWM B 23"),'B');
          controller.run(2,Controller::newString("BIN Z 13"),'B');
  controller.run(2,Controller::newString("B BRT 255"),'D',2);
  controller.run(2,Controller::newString("B ROT 1"),'D',2);
   controller.run(28,Controller::newString("FUN C0 w[$DC:VRV={{$DV:VLT+$DC:VRV}/#D2}]"),'C'); 
    controller.run(28,Controller::newString("FUN C1 w[$DC:VRC={{$DV:CUR+$DC:VRC}/#D2}]"),'C'); 
   controller.run(27,Controller::newString("FUN D0 ?{$TD:BTN>{t-#T11}}[?{$BD:BTN==#B0}[w[$BB:RRR={$BB:RRR+#B1}],?{$BD:BTN==#B1}[w[$BB:GGG={$BB:GGG+#B1}],?{$BD:BTN==#B2}[w[$BB:BBB={$BB:BBB+#B1}],#B0]]],#B0]"),'C');


  controller.run(2,Controller::newString("0 40 40 B 45,28 B8 $BB:RRR 255,255,255 $BB:RRR,0,0"),'D');
  controller.run(2,Controller::newString("1 90 40 B 45,28 B8 $BB:GGG 255,255,255 0,$BB:GGG,0"),'D');
  controller.run(2,Controller::newString("2 140 40 B 45,28 B8 $BB:BBB 255,255,255 0,0,$BB:BBB"),'D');
  controller.run(2,Controller::newString("3 10 80 T B8 $DF:HED.0 255,255,255 0,0,0"),'D');
   controller.run(2,Controller::newString("4 70 80 T 8 $DC:VRV 255,255,255 0,0,0"),'D');
      controller.run(2,Controller::newString("40 70 100 T 8 $DC:VRC 255,255,255 0,0,0"),'D');
     controller.run(2,Controller::newString("5 140 80 T 8 $UL:AAA 255,255,255 0,0,0"),'D');
    controller.run(2,Controller::newString("6 180 80 T 8 $UE:AAA 255,255,255 0,0,0"),'D');
   controller.run(2,Controller::newString("7 220 80 T 8 $UE:BBB 255,255,255 0,0,0"),'D');
   controller.run(2,Controller::newString("8 0 218 T 8 $SZ:INP 0,255,0 10,10,10"),'D');
    controller.run(2,Controller::newString("9 0 230 T 8 $SZ:ETX 255,0,0 10,10,10"),'D');
    controller.run(2,Controller::newString("10 260 230 T B8 $TZ:ETM 200,0,0 10,10,10"),'D');
    controller.run(2,Controller::newString("80 2 2 T B8 \"Motor: ERR THR  DIF  M0B M1B M0Sp M1Sp M0Cur  M1Cur\" 255,255,255 0,0,0"),'D');
    controller.run(2,Controller::newString("81 42 16 T B8 $BM:ERR 255,12,12 0,0,0"),'D');
    controller.run(2,Controller::newString("82 116 16 T 8 $BM:BLL 255,255,255 0,0,0"),'D');
    controller.run(2,Controller::newString("83 140 16 T 8 $BM:BRR 255,255,255 0,0,0"),'D');
    controller.run(2,Controller::newString("84 67 16 T 8 $IM:THR 255,255,255 0,0,0"),'D');
    controller.run(2,Controller::newString("85 92 16 T 8 $IM:DIF 255,255,255 0,0,0"),'D');
    controller.run(2,Controller::newString("86 166 16 T 8 $IM:SLT 255,255,255 0,0,0"),'D');    
    controller.run(2,Controller::newString("87 199 16 T 8 $IM:SRT 255,255,255 0,0,0"),'D');
    controller.run(2,Controller::newString("88 232 16 T 8 $UM:CLR 255,255,255 0,0,0"),'D');
    controller.run(2,Controller::newString("89 274 16 T 8 $UM:CRT 255,255,255 0,0,0"),'D');
 

 controller.schedule(2,0,10,false,0,Controller::newString("EXE D0"),'C');
  controller.schedule(2,0,1,false,0,Controller::newString("EXE C0"),'C');

   controller.schedule(2,0,1,false,0,Controller::newString("EXE C1"),'C');

   controller.run(2,Controller::newString("SET BP:ENA #B1"),'C');
      controller.run(2,Controller::newString("SET BE:ENA #B0"),'C');
       controller.run(2,Controller::newString("SET BL:ENA #B1"),'C');
  controller.schedule(4,1000,10,false,1,Controller::newString("SET UP:PAN #U200"),'C');
  controller.schedule(4,1000,10,false,1,Controller::newString("SET UP:TLT #U512"),'C');
    controller.schedule(4,1000,1000,false,0,Controller::newString("SET BB:ZZZ ?{$BB:ZZZ==#B0}[#B1,#B0]"),'C');
controller.schedule(4214,1000,1000,false,0,Controller::newString("U AAA 1 0"),'L',1);

 controller.schedule(1,3,3,false,0,Controller::newString("D"),'D');
 
  controller.schedule(101,1000,1000,false,0,Controller::newString("D HED 0 0"),'F',1);

}

void loop() {
        if (Serial1.available()){ 
          controller.processInput(&Serial1);
        }
        controller.execute(&Serial1);  
}
