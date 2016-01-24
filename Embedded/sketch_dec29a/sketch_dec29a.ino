#include "SPI.h"
#include <i2c_t3.h>
#include "ILI9341_t3.h"
#include <XPT2046_Touchscreen.h>
#include "CTFont.h"

#include "dh_logger.h"
#include "dh_controller.h"

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


Logger logger;
ControlledLED led(21,22,23);
ControlledTouchLCD disp;
ControlledCalc calc;
BNO055Fusion fusion;
ControlledVMeter vMeter(A10,A11);
ControlledPanTilt panTilt;
ControlledLidar lidar(27);
ControlledI2CXL echo;
Controller controller;


void setup() {
  pinMode(27,OUTPUT);
  digitalWrite(27,LOW);

  Serial.begin(115200);
  Serial1.begin(115200);
  Wire1.begin(I2C_MASTER, 0x00, I2C_PINS_29_30, I2C_PULLUP_EXT, I2C_RATE_1200,I2C_OP_MODE_DMA);
 // Wire1.setDefaultTimeout(100); 
  fusion.setWire(&Wire1);
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_800,I2C_OP_MODE_DMA);
 // Wire.setDefaultTimeout(100); 
  lidar.setWire(&Wire);
  echo.setWire(&Wire);
  logger.setStream(&Serial1); 
delay(2000);
Serial.println("Starting Controlled Modules");

  controller.loadControlled('B',&led); 
  controller.loadControlled('C',&calc);
  controller.loadControlled('D',&disp); 
  controller.loadControlled('F',&fusion);
  controller.loadControlled('V',&vMeter);
  controller.loadControlled('P',&panTilt);
  controller.loadControlled('L',&lidar);
  controller.loadControlled('E',&echo);
  
  Serial.println("Modules have started!");
  
  controller.run(2,Controller::newString("B BRT 255"),'D',2);
  controller.run(2,Controller::newString("B ROT 1"),'D',2);
  
   controller.run(27,Controller::newString("FUN D0 ?{$TD:BTN>{t-#T11}}[?{$BD:BTN==#B0}[w[$BB:RRR={$BB:RRR+#B1}],?{$BD:BTN==#B1}[w[$BB:GGG={$BB:GGG+#B1}],?{$BD:BTN==#B2}[w[$BB:BBB={$BB:BBB+#B1}],#B0]]],#B0]"),'C');


  controller.run(2,Controller::newString("0 40 40 B 45,28 B16 $BB:RRR 255,255,255 $BB:RRR,0,0"),'D');
  controller.run(2,Controller::newString("1 90 40 B 45,28 B16 $BB:GGG 255,255,255 0,$BB:GGG,0"),'D');
  controller.run(2,Controller::newString("2 140 40 B 45,28 B16 $BB:BBB 255,255,255 0,0,$BB:BBB"),'D');
  controller.run(2,Controller::newString("3 10 80 T B12 $FF:HED.0 255,255,255 0,0,0"),'D');
   controller.run(2,Controller::newString("4 70 80 T 12 $FV:VLT 255,255,255 0,0,0"),'D');
      controller.run(2,Controller::newString("40 70 100 T 12 $FV:CUR 255,255,255 0,0,0"),'D');
     controller.run(2,Controller::newString("5 120 80 T 12 $UL:AAA 255,255,255 0,0,0"),'D');
    controller.run(2,Controller::newString("6 160 80 T 12 $UE:AAA 255,255,255 0,0,0"),'D');
        controller.run(2,Controller::newString("7 190 80 T 12 $UE:BBB 255,255,255 0,0,0"),'D');
controller.run(2,Controller::newString("INT 0 #D0,#D512/#D179.999,#D0|#D180,#D1024/#D360,#D512"),'C');
 controller.schedule(2,0,10,false,0,Controller::newString("EXE D0"),'C');

   controller.run(2,Controller::newString("SET BP:ENA #B1"),'C');
      controller.run(2,Controller::newString("SET BE:ENA #B1"),'C');
       controller.run(2,Controller::newString("SET BL:ENA #B1"),'C');
  controller.schedule(4,1000,10,false,0,Controller::newString("SET UP:PAN {{{#U200+$BB:RRR}+$BB:GGG}+$BB:BBB}"),'C');


 controller.schedule(4,0,10,false,0,Controller::newString("D"),'D');
 
}

void loop() {
        if (Serial.available()){
          controller.processInput(&Serial);
        }
        controller.execute(&Serial);  
}
