#include <Entropy.h>


#include <SmartMatrix_32x32.h>

SmartMatrix matrix;

void setup() {
  // put your setup code here, to run once:
    matrix.begin();
    matrix.setBrightness(255);
    matrix.setColorCorrection(ccNone);
}
uint8_t x=20,y=20;
uint8_t swip = 0;
 rgb24 color = {(uint8_t)random(255), (uint8_t)random(255), (uint8_t)random(255)};
void loop() {
  matrix.drawPixel(x,y,color);

  uint8_t in = random(8);
  
  switch (in){
    case 0 : x--;y--;break;
    case 1 : y--;break;
    case 2 : x++;y--; break;
    case 3 : x--;break;
    case 4 : x++; break;
    case 5 : x--;y++; break;
    case 6 : y++; break;
    case 7 : x++;y++; break;
  }
  
  
  

  if (x >= 32)
    x = 0 ; 
    if (y >= 32) 
    y = 0;
  
  
  in = random(8);
  
  if(in&1==1){
    if (color.red<255)
       color.red+=4; 
  }else {
    if (color.red>0)
       color.red-=4;
  }
 
  
  if(in&2==2){
    if (color.blue<255)
       color.blue+=4; 
  }else {
    if (color.blue>0)
       color.blue-=4;
  }
  if(in&4==4){
    if (color.green<255)
       color.green+=4; 
  }else {
    if (color.green>0)
       color.green-=4;
  }
  
  
  /*
  color.green = random();
   color.red = random();
 color.blue = random();
 */
    
  matrix.swapBuffers(true);
}

