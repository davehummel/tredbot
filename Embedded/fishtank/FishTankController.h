#ifndef FISHTANKCONTROLLER_H__
#define FISHTANKCONTROLLER_H__
#include "dh_controller.h"

enum PumpState{
  Off,Full,Balance,Failed
};
class FishTankController:public Controller::Controlled{
public:
  FishTankController(uint8_t pump_Count,uint32_t pump_Max){
      pumpCount=pump_Count;
      pumpMax = pump_Max;
  }
  void begin(void){

  }
  void execute(uint32_t time,uint32_t id,char command[]){
    calculateLevel();
    calculateState();
    for (uint8_t i = 0 ; i < pumpCount;i++){
      switch (pumpState[i]) {
        case Off:
          if ((millis()/1000+i)/5%2){
            setPumpPower(i,1);
          }
      }
    }
  }
  void write(ADDR1 addr,float val){
    uint8_t letter = addr.addr%26;
    if (letter>)
      return;
    pumpLevel[pumpCount]= val;
  }
private:
  void setPumpPower(uint8_t pump,double power){

  }
  uint32_t pumpMax;
  uint8_t pumpCount;
  float totalWaterLevel=0;
  float pumpLevel[pumpCount]={0};
  double pumpPower[pumpCount]={0};
  PumpState pumpState[pumpCount] = {Off};
};

#endif
