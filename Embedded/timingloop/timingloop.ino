#include <Wire.h>
#include <Adafruit_MotorShield.h>

const long MIN_STEP_INTERVAL = 20;
const long MAX_STEP_INTERVAL = 255;
const long SLEEP_INTERVAL = 5000;
const int MIN_STOPPED_TURN_INTERVAL = 500;
const int MIN_FAST_TURN_INTERVAL = 80;
const int MAX_STOPPED_TURN_TOTAL = 320;
const int MAX_FAST_TURN_TOTAL = 80;
const int MAX_SPEED_DELTA_INTERVAL = 20;
const unsigned int MAX_UNSIGNED_INT = 65535;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();  
Adafruit_DCMotor *myMotorR = AFMS.getMotor(1);
Adafruit_DCMotor *myMotorL = AFMS.getMotor(2);

const uint8_t MAXSPEED = 255;

const long STARTTIME = millis();

void setup() {
  Serial.begin(57600);
  AFMS.begin();  // create with the default frequency 1.6KHz
  digitalWrite(13,HIGH);
  myMotorL->setSpeed(MAXSPEED);
  myMotorL->run(FORWARD);
  // turn on motor
  myMotorL->run(RELEASE);
  myMotorR->setSpeed(MAXSPEED);
  myMotorR->run(FORWARD);
  // turn on motor
  myMotorR->run(RELEASE);
  digitalWrite(13,LOW);
  Serial.write("TredBot Ready!");
}




uint8_t lSpeed = 0,rSpeed = 0;
bool execute=false,debug=false;
bool lForward = true,rForward = true;
int stopped_turn_interval = MIN_STOPPED_TURN_INTERVAL*2;
int fast_turn_interval = MIN_FAST_TURN_INTERVAL*2;
int stopped_turn_total = MAX_STOPPED_TURN_TOTAL/2;
int fast_turn_total = MAX_FAST_TURN_TOTAL /2;
int speed_delta_interval = MAX_SPEED_DELTA_INTERVAL/2;
int step_interval = MIN_STEP_INTERVAL*2;


void loop() {

  long previousTime=0;
  long timeDelta=0;
  long prevCommandTime=0;
  bool sleeping = true;
  unsigned int turnRemaining = 0;
  int targetSpeed=0,currentSpeed=0;
  int leftDif=0, rightDif=0;
  bool turnLeft=false,turnRight=false;
  
  while(true){
    long now = millis();
  
    if (previousTime<=now-step_interval){
      timeDelta = now - previousTime;
      previousTime = now;
      if (Serial.available()){
        if (sleeping){
         Serial.write("\nWaking up!!"); 
        }
        sleeping = false;
        prevCommandTime = now;
        readInput(turnRemaining,targetSpeed,turnLeft,turnRight,currentSpeed);
      } else {
        if (!sleeping && prevCommandTime<=now-SLEEP_INTERVAL){
          sleeping = true;
          turnRemaining=0;
          targetSpeed = 0;
          Serial.write("\nI'm sleepy!!");
        }
      }
      bool updateMotor = false;
      if (currentSpeed != targetSpeed){
        updateSpeed(currentSpeed, targetSpeed); 
        updateMotor = true;
      }
      if (turnRemaining > 0 ){
        updateDifferential( currentSpeed, turnLeft, turnRight,timeDelta, turnRemaining,leftDif, rightDif);
        updateMotor = true;
      } else if (turnLeft|turnRight){
        updateMotor = true;
        leftDif = 0;
        rightDif = 0;
        turnLeft = false;
        turnRight = false;
      }
      
      if (updateMotor) {
        setMotorSpeeds(currentSpeed,leftDif,rightDif);
      }
    }
  }
}

void updateSpeed(int &currentSpeed, int targetSpeed){
   int dif = targetSpeed - currentSpeed;
   if (dif > 0){
     if (dif > speed_delta_interval)
       dif = speed_delta_interval;
   } else {
     if (dif*-1> speed_delta_interval)
        dif = -1*speed_delta_interval;
   }
   currentSpeed+=dif;
}

void updateDifferential(int currentSpeed, bool turnLeft, bool turnRight, long timeDelta, unsigned int &turnRemaining, int &leftDif, int &rightDif){
  float slowFrac = (1.0-currentSpeed/255.0);
  float fastFrac = (currentSpeed/255.0);
  float difAmount = slowFrac*stopped_turn_total+fastFrac*fast_turn_total;
  float turnTime = slowFrac*stopped_turn_interval+fastFrac*fast_turn_interval;
  float frac = timeDelta/turnTime;
  int turnStep = frac*MAX_UNSIGNED_INT;
  // if this is the last partial step of a turn
  if (turnStep>turnRemaining){
    difAmount = difAmount*turnRemaining/(float)turnStep;
    
    turnRemaining = 0;
  }else{
    turnRemaining -= turnStep;
  }
  // use this to both add and subtract
  difAmount=difAmount/2;
  if (turnLeft){
    leftDif=-difAmount;
    rightDif=difAmount;
  }else if (turnRight){
    leftDif=difAmount;
    rightDif=-difAmount;
  }
  
}

void setMotorSpeeds(int currentSpeed, int leftDif, int rightDif){
 if (debug){
   Serial.write("\nT+");
   Serial.print((millis() - STARTTIME)/(float)1000);
   Serial.write("s>CS|LD|RD|SC|TL|TR>"); 
   Serial.print(currentSpeed);
   Serial.write("|");
   Serial.print(leftDif);
   Serial.write("|");
   Serial.print(rightDif);
   Serial.write("|");
 }
  float left = leftDif+currentSpeed;
  float right = rightDif+currentSpeed;
  
  if (left>255 || right > 255 || left<-255 || right<-255){
    float scaleFactor=0;
    
    if (left>0){
      scaleFactor = left;
    }else{
      scaleFactor = -left;
    }
    if (right>0){
      if (right>scaleFactor)
        scaleFactor = right;
    } else {
      if (right*-1 > scaleFactor)
        scaleFactor = right*-1;
    }
   
    scaleFactor = 255.0/scaleFactor;
    
    left*=scaleFactor;
    right*=scaleFactor; 
    if (debug){
      Serial.print(scaleFactor);
    }
  }
  
  if (left==0){
   lForward = true;
   myMotorL->run(FORWARD);
   lSpeed = 0; 
  }else if (left>0){
   lForward = true;
   myMotorL->run(FORWARD);
   lSpeed = (left/255.0)*MAXSPEED;
  } else {
   lForward = false;
   myMotorL->run(BACKWARD);
   lSpeed = (left/-255.0)*MAXSPEED;
  }

  if (right==0){
   rForward = true;
     myMotorR->run(FORWARD);
   rSpeed = 0; 
  }else if (right>0){
   rForward = true;
     myMotorR->run(FORWARD);
   rSpeed = (right/255.0)*MAXSPEED;
  } else {
   rForward = false;
     myMotorR->run(BACKWARD);
   rSpeed = (right/-255.0)*MAXSPEED;
  }
  if (execute){
    myMotorL->setSpeed(lSpeed);
    myMotorR->setSpeed(rSpeed);
  } else {
    myMotorL->setSpeed(0);
    myMotorR->setSpeed(0);  
  }
  if (debug){
    Serial.write("|");
    Serial.print(lSpeed);
    Serial.write(lForward?"F|":"B|");
    Serial.print(rSpeed);
    Serial.write(rForward?"F<":"B<");
  }
}

void sendStatus(int targetSpeed, int currentSpeed,unsigned int turnRemaining){
 Serial.write("\nLeft Speed=");
 Serial.print(lSpeed);
 Serial.write(lForward?" forward ":" backward");
 Serial.write(" Right Speed=");
 Serial.print(rSpeed);
 Serial.write(rForward?" forward ":" backward");
 Serial.write("\nTarget Speed=");
 Serial.print(targetSpeed);
 Serial.write(" Current Speed=");
 Serial.print(currentSpeed);
 Serial.write("\nTurn Remaining=");
 Serial.print(turnRemaining);
 Serial.write("\nUp Time=");
 Serial.print((millis() - STARTTIME)/(float)1000);
 Serial.write(" seconds"); 
}

void readInput(unsigned int &turnRemaining,int &targetSpeed,bool &turnLeft,bool &turnRight,int currentSpeed){
  char command[1];
  Serial.readBytes(command,1);

  switch (command[0]){
   case 'u': // updaet -- command to execute a movement with rotation and velocity
     targetSpeed = Serial.parseInt();
     turnRemaining = Serial.parseInt();
     // Read Direction
     turnRight = false;
     turnLeft = false;
     Serial.readBytes(command,1);
     if ('r'==command[0]){
       turnRight = true;
     }else if ('l'==command[0]){
       turnLeft = true;
     }
     Serial.write("\nUpdate: speed=");
     Serial.println(targetSpeed);
     if (turnLeft)
     Serial.write(" turn L:");
     else if (turnRight)
     Serial.write(" turn R:");
     Serial.print(turnRemaining);
     return;
   break;
   case 'd':  // debug -- execute and test
     debug = !debug;
     Serial.write("\nDebug:");
     Serial.write(debug?"True":"False");
     return;
   case 'e': // test -- command to print a movement with rotation and velocity
     execute = !execute;
     Serial.write("\nExecute:");
     Serial.write(execute?"True":"False");
     return;
   break;
   case 'p': // ping -- does nothing but keep from sleeping
     return;
   case 's': // status -- return status
     sendStatus(targetSpeed,currentSpeed,turnRemaining);
     return;
   case 'c': // configure
     step_interval = Serial.parseInt();
     if (step_interval<MIN_STEP_INTERVAL){
       step_interval = MIN_STEP_INTERVAL;
     }else if(step_interval>MAX_STEP_INTERVAL){
       step_interval=MAX_STEP_INTERVAL; 
     }
     stopped_turn_interval = Serial.parseInt();
     if (stopped_turn_interval<MIN_STOPPED_TURN_INTERVAL){
       stopped_turn_interval = MIN_STOPPED_TURN_INTERVAL;
     }
     fast_turn_interval =  Serial.parseInt();
     if (fast_turn_interval<MIN_FAST_TURN_INTERVAL){
       fast_turn_interval = MIN_FAST_TURN_INTERVAL; 
     }
     stopped_turn_total =  Serial.parseInt();
     if (stopped_turn_total>MAX_STOPPED_TURN_TOTAL){
       stopped_turn_total = MAX_STOPPED_TURN_TOTAL; 
     }
     fast_turn_total =  Serial.parseInt();
     if (fast_turn_total>MAX_FAST_TURN_TOTAL){
       fast_turn_total = MAX_FAST_TURN_TOTAL;
     }
     speed_delta_interval =  Serial.parseInt();
     Serial.readBytes(command,1);
     if (speed_delta_interval>MAX_SPEED_DELTA_INTERVAL){
       speed_delta_interval=MAX_SPEED_DELTA_INTERVAL;
     }
     return;
   default:
     Serial.write("\nUnknown Command:");
     Serial.write(command[0]); 
  }
  return;
}



