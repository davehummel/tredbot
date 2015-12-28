#include <i2c_t3.h>
#include  <dh_mpr121.h>
#define _touch 5
TouchPad touchPad = TouchPad(_touch);
uint8_t activePoint;
bool checkTouchPad;
bool fingerDown;
uint8_t touchNum;
void touchPress(){
  checkTouchPad = true;
}

void onTouch(uint8_t num){
    fingerDown = true;
    touchNum = num;
}

void onRelease(uint8_t num){
  fingerDown = false;
  activePoint = num*2;
}


void setup() {
  pinMode(13,OUTPUT);
    pinMode(21,OUTPUT);
      pinMode(22,OUTPUT);
        pinMode(23,OUTPUT);
        
  analogWrite(21,0);
  analogWrite(22,0);
  analogWrite(23,0);
        
  Wire.begin(I2C_MASTER,0,I2C_PINS_18_19,I2C_PULLUP_EXT,I2C_RATE_400);
   touchPad.begin(touchPress);
}
int count = 100;
uint8_t r=0,g=0,b=0;
void loop() {
  delay(5);
  if (checkTouchPad){
    checkTouchPad = false;
    touchPad.process(onTouch,onRelease);
  }
  
  digitalWrite(13,fingerDown);
  if (fingerDown){
     if (touchNum == 0) {
       if (r >=5 ){
         r-=5;
       }else{
         r=0;
       }
     }
       if (touchNum == 1) {
       if (r >=1 ){
         r-=1;
       }else{
         r=0;
       }
     }
     if (touchNum == 2) {
       if (r <=254 ){
         r+=1;
       }else{
         r=255;
       }
     }
     if (touchNum == 3) {
       if (r <=250 ){
         r+=5;
       }else{
         r=255;
       }
     }
      if (touchNum == 4) {
       if (g >=5 ){
         g-=5;
       }else{
         g=0;
       }
     }
       if (touchNum == 5) {
       if (g >=1 ){
         g-=1;
       }else{
         g=0;
       }
     }
     if (touchNum == 6) {
       if (g <=254 ){
         g+=1;
       }else{
         g=255;
       }
     }
     if (touchNum == 7) {
       if (g <=250 ){
         g+=5;
       }else{
         g=255;
       }
     }
       if (touchNum == 8) {
       if (b >=5 ){
         b-=5;
       }else{
         b=0;
       }
     }
       if (touchNum == 9) {
       if (b >=1 ){
         b-=1;
       }else{
         b=0;
       }
     }
     if (touchNum == 10) {
       if (b <=254 ){
         b+=1;
       }else{
         b=255;
       }
     }
     if (touchNum == 11) {
       if (b <=250 ){
         b+=5;
       }else{
         b=255;
       }
     }
  }
  
  if (activePoint>0){
    count--;
    if (count == 0){
      count = 100;
      activePoint--;
    }
    digitalWrite(13,activePoint%2);
  }

  analogWrite(21,r);
   analogWrite(22,g);
      analogWrite(23,b);

}
