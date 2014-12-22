#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIXEL_PIN            2

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      16
#define FRONTLEFT      5

#define REDLITE 3
#define GREENLITE 5
#define BLUELITE 6

#define STATUS_COMMAND 's'
#define UPDATE_COMMAND 'u'
#define DEBUG_COMMAND 'd'
#define EXECUTE_COMMAND 'e'
#define PING_COMMAND 'p'
#define CONFIG_COMMAND 'c'
#define GET_CONFIG_COMMAND 'g'

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

const long MIN_STEP_INTERVAL = 20;
const long MAX_STEP_INTERVAL = 255;
const long SLEEP_INTERVAL = 5000;
const int MIN_STOPPED_TURN_INTERVAL = 1000;
const int MIN_FAST_TURN_INTERVAL = 400;
const int MAX_STOPPED_TURN_SPEED = 320;
const int MAX_FAST_TURN_SPEED = 80;
const int MAX_SPEED_DELTA_INTERVAL = 20;
const unsigned int MAX_UNSIGNED_INT = 65535;
const String BLANKLINE = "                ";

Adafruit_MotorShield AFMS = Adafruit_MotorShield();  
Adafruit_DCMotor *myMotorR = AFMS.getMotor(1);
Adafruit_DCMotor *myMotorL = AFMS.getMotor(2);

const uint8_t MAXSPEED = 255;

const long STARTTIME = 1000*(millis()/1000);

void setup() {
  Serial.begin(57600);
  pinMode(13, OUTPUT);   
  
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
  
  pinMode(REDLITE, OUTPUT);
  pinMode(GREENLITE, OUTPUT);
  pinMode(BLUELITE, OUTPUT);
  
  lcd.begin(16, 2);
  pixels.begin();
  Serial.write("TredBot Ready!\n");
  setTop("Ready!");
  setBottom("0 s");
}

 long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;
}


double voltageIn(){
  unsigned int ADCValue;
  double Vcc = readVcc()/1000.0;
  ADCValue = analogRead(0);
  return (ADCValue / 1023.0) * 5;
}

uint8_t lSpeed = 0,rSpeed = 0;
bool execute=false,debug=false;
bool lForward = true,rForward = true;
int stopped_turn_interval = MIN_STOPPED_TURN_INTERVAL*2;
int fast_turn_interval = MIN_FAST_TURN_INTERVAL*2;
int stopped_turn_speed = MAX_STOPPED_TURN_SPEED/2;
int fast_turn_speed = MAX_FAST_TURN_SPEED /2;
int speed_delta_interval = MAX_SPEED_DELTA_INTERVAL/2;
int step_interval = MIN_STEP_INTERVAL*2;


void loop() {
  uint8_t r=90,g=200,b=255;
  long previousTime=0;
  long timeDelta=0;
  long prevCommandTime=0;
  bool sleeping = false;
  unsigned int turnTotal = 0;
  unsigned int turnRemaining = 0;
  int targetSpeed=0,currentSpeed=0;
  int leftDif=0, rightDif=0;
  bool turnLeft=false,turnRight=false;
  long lastDisplayUpdate = 0;
  lcd.blink();
  while(true){
    long now = millis();

    if (previousTime<=now-step_interval){
      if (lastDisplayUpdate<now-1000){
        lastDisplayUpdate = now;
        if (sleeping){
           setBottom(String(voltageIn())+"v");
        }
        setBacklight(r,g,b);
      }
      // This manages the sleeping light effect
      if (sleeping){
         float factor = (now%10000-5000)/20000.0;
           if (factor<0)
              factor = factor * -1;
           factor = factor+.01;
           drawCircle(20*factor,255*fact,10*factor);
      }
      timeDelta = now - previousTime;
      previousTime = now;
      if (Serial.available()){
        sleeping = false;
        prevCommandTime = now;
        setTop("Awake!");
        readInput(turnTotal,turnRemaining,targetSpeed,turnLeft,turnRight,currentSpeed);
      } else {
        if (!sleeping && prevCommandTime<=now-SLEEP_INTERVAL){
          sleeping = true;
          turnRemaining=0;
          targetSpeed = 0;
          setTop("Sleeping...");
        }
      }
      bool updateMotor = false;
      if (currentSpeed != targetSpeed){
        if (currentSpeed>targetSpeed)
            setBottom("Speed up to "+String(targetSpeed));
        else
            setBottom("Slow to "+String(targetSpeed));
        updateSpeed(currentSpeed, targetSpeed); 
        updateMotor = true;
      }else if (!sleeping){
        setBottom("Speed = "+String(currentSpeed));
      }
      if (turnRemaining > 0 ){
        if (turnLeft)
           setTop("Turning Left");
        else
           setTop("Turning Right");
        drawTurn(turnTotal,turnRemaining, turnLeft,r,g,b);
      
       
        updateDifferential( currentSpeed, turnLeft, turnRight,timeDelta, turnRemaining,leftDif, rightDif);
        updateMotor = true;
      } else if (turnLeft|turnRight){
        setTop("Straight Ahead!!");
        drawTurn(0,0,false,r,g,b);
        updateMotor = true;
        leftDif = 0;
        rightDif = 0;
        turnLeft = false;
        turnRight = false;
        turnTotal = 0;
      }
      
      if (updateMotor) {
        setMotorSpeeds(currentSpeed,leftDif,rightDif);
          Serial.write(turnLeft?">L:":">R:");
          Serial.print(turnRemaining);
          Serial.write(":");
          Serial.println(currentSpeed);
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
  float difAmount = slowFrac*stopped_turn_speed+fastFrac*fast_turn_speed;
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
   Serial.write("T+");
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
    Serial.write("\n");
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

void readInput(unsigned int &turnStart, unsigned int &turnRemaining,int &targetSpeed,bool &turnLeft,bool &turnRight,int currentSpeed){
  char command[1];
  Serial.readBytes(command,1);

  switch (command[0]){
   case UPDATE_COMMAND: // update -- command to execute a movement with rotation and velocity
     targetSpeed = Serial.parseInt();
     turnStart=turnRemaining = Serial.parseInt();
     // Read Direction
     turnRight = false;
     turnLeft = false;
     Serial.readBytes(command,1);
     if ('r'==command[0]){
       turnRight = true;
     }else if ('l'==command[0]){
       turnLeft = true;
     }
     Serial.write("OK\n");
     return;
   break;
   case DEBUG_COMMAND:  // debug -- command to print a movement with rotation and velocity
     debug = !debug;
     Serial.write("OK:");
     Serial.write(debug?"true\n":"false\n");
     return;
   case EXECUTE_COMMAND: // execute -- command to determine if movements are executed by motors or just simulated
     execute = !execute;
     Serial.write("OK:");
     Serial.write(execute?"true\n":"false\n");
     return;
   break;
   case PING_COMMAND: // ping -- does nothing but keep from sleeping
     return;
   case STATUS_COMMAND: // status -- return status
     sendStatus(targetSpeed,currentSpeed,turnRemaining);
     return;
   case CONFIG_COMMAND: // configure
   
     step_interval = Serial.parseInt();
     if (step_interval<MIN_STEP_INTERVAL){
       step_interval = MIN_STEP_INTERVAL;
     }else if(step_interval>MAX_STEP_INTERVAL){
       step_interval=MAX_STEP_INTERVAL; 
     }

     speed_delta_interval =  Serial.parseInt();
     if (speed_delta_interval>MAX_SPEED_DELTA_INTERVAL){
       speed_delta_interval=MAX_SPEED_DELTA_INTERVAL;
     }

     stopped_turn_interval = Serial.parseInt();
     if (stopped_turn_interval<MIN_STOPPED_TURN_INTERVAL){
       stopped_turn_interval = MIN_STOPPED_TURN_INTERVAL;
     }
     fast_turn_interval =  Serial.parseInt();
     if (fast_turn_interval<MIN_FAST_TURN_INTERVAL){
       fast_turn_interval = MIN_FAST_TURN_INTERVAL; 
     }
     stopped_turn_speed =  Serial.parseInt();
     if (stopped_turn_speed>MAX_STOPPED_TURN_SPEED){
       stopped_turn_speed = MAX_STOPPED_TURN_SPEED; 
     }
     fast_turn_speed =  Serial.parseInt();
     if (fast_turn_speed>MAX_FAST_TURN_SPEED){
       fast_turn_speed = MAX_FAST_TURN_SPEED;
     }

      Serial.readBytes(command,1);
      Serial.write("OK\n");
     return;
   case GET_CONFIG_COMMAND: // get configuration
     Serial.print(step_interval);
     Serial.write(' ');
     Serial.print(speed_delta_interval);
     Serial.write(' ');
     Serial.print(stopped_turn_interval);
     Serial.write(' ');
     Serial.print(fast_turn_interval);
     Serial.write(' ');
     Serial.print(stopped_turn_speed);
     Serial.write(' ');
     Serial.print(fast_turn_speed);
        Serial.write('\n');
     return;
   default:
     Serial.write("Unknown Command\n");
  }
  return;
}

void setTop(String topLine){
  lcd.setCursor(0,0);
  lcd.print(BLANKLINE);
  lcd.setCursor(0,0);
  lcd.print(topLine);
}
void setBottom(String bottomLine){
  lcd.setCursor(0,1);
  lcd.print(BLANKLINE);
  lcd.setCursor(0,1);
  lcd.print(bottomLine);
}

void setBacklight(uint8_t r, uint8_t g, uint8_t b) {
  // normalize the red LED - its brighter than the rest!
//  r = map(r, 0, 255, 0, 100);
//  g = map(g, 0, 255, 0, 150);
// 
//  r = map(r, 0, 255, 0, brightness);
//  g = map(g, 0, 255, 0, brightness);
//  b = map(b, 0, 255, 0, brightness);
 
  // common anode so invert!
  r = map(r, 0, 255, 255, 0);
  g = map(g, 0, 255, 255, 0);
  b = map(b, 0, 255, 255, 0);

  analogWrite(REDLITE, r);
  analogWrite(GREENLITE, g);
  analogWrite(BLUELITE, b);
}


void drawTurn(unsigned int turnTotal, unsigned int currentTurn, boolean turnLeft, uint8_t r, uint8_t g, uint8_t b){
     if (turnTotal == 0){
        for (turnTotal = 0; turnTotal < NUMPIXELS ; turnTotal++){
          if (turnTotal == FRONTLEFT || turnTotal == FRONTLEFT+1){
            pixels.setPixelColor(turnTotal,pixels.Color(r,g,b));
          }else{
            pixels.setPixelColor(turnTotal,pixels.Color(0,0,0));
          }
        }
        pixels.show();
        turnTotal = 0;
        return;
     }   
  int brightSpot,darkSpot; 
  brightSpot = ((float)(currentTurn-1)/MAX_UNSIGNED_INT)*(NUMPIXELS/2);
  darkSpot = ((float)(turnTotal-1)/MAX_UNSIGNED_INT)*(NUMPIXELS/2);

  
  if (turnLeft){
    brightSpot+=FRONTLEFT+1;
    if (brightSpot>=NUMPIXELS)
       brightSpot-=NUMPIXELS;
    darkSpot+=FRONTLEFT+1;
    if (darkSpot>=NUMPIXELS)
       darkSpot-=NUMPIXELS;
  }else{
    brightSpot=FRONTLEFT-brightSpot;
    if (brightSpot<0)
       brightSpot+=NUMPIXELS;
    darkSpot=FRONTLEFT-darkSpot;
    if (darkSpot<0)
       darkSpot+=NUMPIXELS;
  }
  
//  Serial.write("[");
//  Serial.print(brightSpot);
//  Serial.write(",");
//  Serial.print(darkSpot);
//  Serial.write("]");
  
  for (int i =0; i< NUMPIXELS; i ++){
    pixels.setPixelColor(i,pixels.Color(0,0,0));
  }
  
   pixels.setPixelColor(brightSpot,pixels.Color(r,g,b));
   pixels.setPixelColor(darkSpot,pixels.Color(r/5,g/5,b/5));
  
  
    pixels.show();
}

void drawCircle(uint8_t r, uint8_t g, uint8_t b){
  for (int i =0; i< NUMPIXELS; i ++){
    pixels.setPixelColor(i,pixels.Color(r,g,b));
  }
  pixels.show();
}
