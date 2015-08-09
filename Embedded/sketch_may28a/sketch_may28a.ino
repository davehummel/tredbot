#include <Wire.h>
#include "utility/Adafruit_PWMServoDriver.h"
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor2 = AFMS.getMotor(2);

static int forwardCenter = 525;
static int sideCenter = 505;
float acumf[5] = {0,0,0,0,0};

void setup() {
  // put your setup code here, to run once:
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  Serial.begin(115200);
  AFMS.begin();
  motor1->setSpeed(0);
  motor2->setSpeed(0);
}

int skipper = 0;
int forward = 0;
int side = 0;
void loop() {
  skipper++;
  side = analogRead(0);
   
  Serial.print("forward:");
  Serial.println(forward);
  Serial.print("side:");
  Serial.println(side);
  if (skipper==10){
    forward = analogRead(1);
    float forf = forward-forwardCenter;
    forf = forf/400.;
      acumf[4] = forf;
      forf+=acumf[3];
      forf+=acumf[2];
      forf+=acumf[1];
      forf+=acumf[0];
      
      forf/=5.;
      
      acumf[0] = acumf[1];
       acumf[1] = acumf[2];
        acumf[2] = acumf[3];
         acumf[3] = acumf[4];
  
    if (forf<.1 && forf>-.1 )
      forf = 0;
    if (forf>1)
      forf = 1;
    if (forf<-1)
      forf = -1;
    forward = 255.*forf;
    skipper = 0;
  }
  
  float sidf = side-sideCenter;
  sidf = sidf/450.;
  if (sidf<0 && sidf>-.05)
    sidf = 0;
  if (sidf>1)
    sidf = 1;
  if (sidf<-1)
    sidf = -1;
  if (forward<100 && forward >-100)
    side = 250.*sidf;
  else
    side = 120.*sidf-10;
  Serial.print("forward:");
  Serial.println(forward);
  Serial.print("side:");
  Serial.println(side);
  //invert side if reverse
//  if (forward<0)
//    side = -side;
  setMotor(forward,side);
  delay(10);
}

void setMotor(int speed, int turn){  

  int motor1speed=speed + turn;
  int motor2speed=speed - turn;
  
  if (motor1speed>255) 
    motor1speed = 255;
  else if (motor1speed<-255)
    motor1speed = -255;
    
  if (motor2speed>255) 
    motor2speed = 255;
  else if (motor2speed<-255)
    motor2speed = -255;
  
  if (motor1speed<0){
    motor1speed = -motor1speed;
    motor1->run(FORWARD);
  }else{
    motor1->run(BACKWARD);
  }
  
  if (motor2speed<0){
    motor2speed = -motor2speed;
    motor2->run(FORWARD);
  }else{
    motor2->run(BACKWARD);
  }
  
 Serial.println(motor1speed);
  Serial.println(motor2speed);
  Serial.println(" ");
  
  motor1->setSpeed(motor1speed);
  motor2->setSpeed(motor2speed);
  
}
