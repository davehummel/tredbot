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

//const float PI = 3.14159265;
const int MIN_STEP_INTERVAL = 20;
const int MAX_STEP_INTERVAL = 255;
const int SLEEP_INTERVAL = 20000;
const int MAX_SPEED_DELTA_INTERVAL = 20;
const unsigned int MAX_ANGLE_RANGE = 65535;
const String BLANKLINE = "                ";

Adafruit_MotorShield AFMS = Adafruit_MotorShield();  
Adafruit_DCMotor *myMotorR = AFMS.getMotor(1);
Adafruit_DCMotor *myMotorL = AFMS.getMotor(2);

const long STARTTIME = 1000*(millis()/1000);

void setup() {
  Serial.begin(57600);
  pinMode(13, OUTPUT);   
  
  AFMS.begin();  // create with the default frequency 1.6KHz
  digitalWrite(13,HIGH);
  myMotorL->setSpeed(200);
  myMotorL->run(FORWARD);
  // turn on motor
  myMotorL->run(RELEASE);
  myMotorR->setSpeed(200);
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


 float voltageIn(){
  unsigned int ADCValue;
  float Vcc = readVcc()/1000.0;
  ADCValue = analogRead(0);
  return (ADCValue / 1023.0) * 5;
}

uint8_t lSpeed = 0,rSpeed = 0;
bool execute=false,debug=false;
bool lForward = true,rForward = true;
float expRadsPerIntervalStopped = MAX_ANGLE_RANGE/2000; // turn 180 in 1 seconds
float expRadsPerIntervalFast = MAX_ANGLE_RANGE/4000; // turn 180 in 2 seconds
int defTredDeltaDifStopped = 180; // +90 Forward and -90 Backwards
int defTredDeltaDifFast = 80; // +- 40 from max speed , gets converted to 255, 175
int speedDeltaPerInterval = MAX_SPEED_DELTA_INTERVAL/2;
int stepInterval = MIN_STEP_INTERVAL*2;
boolean isSetup = false;

void loop() {
  uint8_t r=90,g=200,b=255;
  long previousTime=0;
  long timeDelta=0;
  long prevCommandTime=0;
  bool sleeping = false;
  unsigned int currentAngle=0;
  unsigned int targetAngle=0;
  bool justFinishedTurn= false;
  int targetSpeed=0,currentSpeed=0;
  int leftDif=0, rightDif=0;
  long lastDisplayUpdate = 0;
  lcd.blink();
  while(true){
    long now = millis();

    if (previousTime<=now-stepInterval){
      if (lastDisplayUpdate<now-1000){
        lastDisplayUpdate = now;
        if (sleeping){
           setTop("Sleeping...");
           setBottom(String((now-STARTTIME)/1000)+"s");
        }
        
      }
      // This manages the sleeping light effect
      if (sleeping){
         sleepLighting(now);
      }
      timeDelta = now - previousTime;
      previousTime = now;
      if (Serial.available()){
        sleeping = false;
        prevCommandTime = now;
     
        setBacklight(0,255,0);
        readInput(targetAngle,targetSpeed);
      } else {
        if (!sleeping && prevCommandTime<=now-SLEEP_INTERVAL){
          sleeping = true;
          currentAngle = targetAngle;
          targetSpeed = 0;
         
        }
      }
      bool updateMotor = false;
      if (currentSpeed != targetSpeed){
        updateSpeed(currentSpeed, targetSpeed); 
        updateMotor = true;
      }
      if (targetAngle != currentAngle){
      
        updateDifferential( currentSpeed, currentAngle, targetAngle,timeDelta, leftDif, rightDif,justFinishedTurn);
        updateMotor = true;
      } else if (justFinishedTurn){
       

        updateMotor = true;
        leftDif = 0;
        rightDif = 0;
        justFinishedTurn = false;
      }
      
      if (updateMotor) {
        drawTurn(currentAngle,targetAngle,currentSpeed);
        setStatusOnDisplay(currentSpeed,  targetSpeed, currentAngle, targetAngle);
        setMotorSpeeds(currentSpeed,leftDif,rightDif);
          Serial.write(">");
          Serial.print(currentSpeed);
          Serial.write(":");
          Serial.println(currentAngle);
      }
      
    }
  }
}

bool isLeft(unsigned int currentAngle,unsigned int targetAngle){
	if (targetAngle<currentAngle){
		if (currentAngle-targetAngle>MAX_ANGLE_RANGE/2){
			return false;
		}
		return true;
	}else{
		if (targetAngle-currentAngle>MAX_ANGLE_RANGE/2){
			return true;
		}
		return false;
	}
}

void updateSpeed(int &currentSpeed, int targetSpeed){
   int dif = targetSpeed - currentSpeed;
   if (dif > 0){
     if (dif > speedDeltaPerInterval)
       dif = speedDeltaPerInterval;
   } else {
     if (dif*-1> speedDeltaPerInterval)
        dif = -1*speedDeltaPerInterval;
   }
   currentSpeed+=dif;
}

void updateDifferential(int currentSpeed, unsigned int &currentAngle, unsigned int targetAngle, int timeDelta, int &leftDif, int &rightDif, bool &justFinishedTurn){
  float slowFrac = (1.0-currentSpeed/255.0);
  float fastFrac = (currentSpeed/255.0);
  unsigned int expAngle = (expRadsPerIntervalStopped*slowFrac+expRadsPerIntervalFast*fastFrac)* timeDelta;
 
  int speedDelta = defTredDeltaDifStopped*slowFrac+defTredDeltaDifFast*fastFrac;
  Serial.write("Ud:");
  Serial.print(timeDelta);
  Serial.write("ms expAngle=");
  Serial.print(expAngle);
  Serial.write(" speedDelta="); 
  Serial.print(speedDelta);
 
  bool isLeft = false;
  unsigned int angleRemainder = 0;
  	if (targetAngle<currentAngle){
		if ((angleRemainder = currentAngle-targetAngle)>MAX_ANGLE_RANGE/2){
			isLeft = false;
			angleRemainder = MAX_ANGLE_RANGE - angleRemainder; 
		}else{
		  isLeft = true;
                }
	}else{
		if ((angleRemainder = targetAngle-currentAngle)>MAX_ANGLE_RANGE/2){
			isLeft = true;
			angleRemainder = MAX_ANGLE_RANGE - angleRemainder; 
		}else{
		  isLeft = false;
                }
	}
  
  Serial.write(isLeft?">L>":">R>");
  Serial.println(angleRemainder);
  // if this is the last partial step of a turn
  
  if (expAngle >= angleRemainder){
    justFinishedTurn = true;
    speedDelta = speedDelta * ((float)angleRemainder/(float)expAngle);  // Reduce the speed of this final turn segment
    // Update currentAngle ... this is the last movement so they should equal
	currentAngle = targetAngle;
  }else{
    justFinishedTurn = false;
    // Update currentAngle ... calculate and watch for overflow
	unsigned int gap;
    if (isLeft){
		gap = currentAngle;
	}else{
		gap = MAX_ANGLE_RANGE - currentAngle; 
	}
	if (gap<expAngle){
		expAngle-=gap;
		if (isLeft){
			currentAngle = MAX_ANGLE_RANGE - expAngle;
		}else{
			currentAngle = expAngle;
		}
	}else{
		if (isLeft){
			currentAngle -= expAngle;
		}else{
			currentAngle += expAngle;
		}
	}
  }
  // use this to both add and subtract
  speedDelta=speedDelta/2;
  if (isLeft){
    leftDif=-speedDelta;
    rightDif=speedDelta;
  }else{
    leftDif=speedDelta;
    rightDif=-speedDelta;
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
  int left = leftDif+currentSpeed;
  int right = rightDif+currentSpeed;
  
  // If the difference is too big to fit, rescale
  if (leftDif - rightDif > 255){
	int max = (leftDif<0?leftDif*-1:leftDif);
	if (rightDif<0){
		if (rightDif*-1>max)
		  max = rightDif*-1;
	}else{
		if (rightDif>max)
		  max = rightDif;
	}
	leftDif *= 255.0/max;
	rightDif *= 255.0/max;
  }
  // If only one component is too big, shift
  if (left>255) {
	right -= left - 255;
	left = 255;
  } else if (left < -255){
	right += left + 255;
	left = -255;
  } else if (right>255) {
	left -= right - 255;
	right = 255;
  } else if (right < -255){
	left += right + 255;
	right = -255;
  } 
 
  
  if (left==0){
   lForward = true;
   myMotorL->run(FORWARD);
   lSpeed = 0; 
  }else if (left>0){
   lForward = true;
   myMotorL->run(FORWARD);
   lSpeed = left;
  } else {
   lForward = false;
   myMotorL->run(BACKWARD);
   lSpeed = -left;
  }

  if (right==0){
   rForward = true;
   myMotorR->run(FORWARD);
   rSpeed = 0; 
  }else if (right>0){
   rForward = true;
   myMotorR->run(FORWARD);
   rSpeed = right;
  } else {
   rForward = false;
   myMotorR->run(BACKWARD);
   rSpeed = -right;
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

void readInput(unsigned int &targetAngle, int &targetSpeed){
  char command[1];
  Serial.readBytes(command,1);

  switch (command[0]){
   case UPDATE_COMMAND: // update -- command to execute a movement with rotation and velocity
     targetSpeed = Serial.parseInt();
     if (targetSpeed>255)
       targetSpeed = 255;
     else if (targetSpeed<0)
       targetSpeed = 0;
     targetAngle = Serial.parseInt();
  
     Serial.readBytes(command,1);
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
   case CONFIG_COMMAND: // configure
   
     stepInterval = Serial.parseInt();
     if (stepInterval<MIN_STEP_INTERVAL){
       stepInterval = MIN_STEP_INTERVAL;
     }else if(stepInterval>MAX_STEP_INTERVAL){
       stepInterval=MAX_STEP_INTERVAL; 
     }

     speedDeltaPerInterval =  Serial.parseInt();
     if (speedDeltaPerInterval>MAX_SPEED_DELTA_INTERVAL){
       speedDeltaPerInterval=MAX_SPEED_DELTA_INTERVAL;
     }

     expRadsPerIntervalStopped = Serial.parseFloat();
    
     expRadsPerIntervalFast = Serial.parseFloat();
    
     defTredDeltaDifStopped =  Serial.parseInt();
     
     defTredDeltaDifFast =  Serial.parseInt();
    
      isSetup = true;
      Serial.readBytes(command,1);
      Serial.write("OK\n");
     return;
   case GET_CONFIG_COMMAND: // get configuration
     Serial.write("OK:");
     Serial.print(isSetup);
     Serial.print(' ');
     Serial.print(stepInterval);
     Serial.write(' ');
     Serial.print(speedDeltaPerInterval);
     Serial.write(' ');
     Serial.print(expRadsPerIntervalStopped);
     Serial.write(' ');
     Serial.print(expRadsPerIntervalFast);
     Serial.write(' ');
     Serial.print(defTredDeltaDifStopped);
     Serial.write(' ');
     Serial.print(defTredDeltaDifFast);
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

void setStatusOnDisplay(int currentSpeed, int targetSpeed,unsigned int currentAngle, unsigned int targetAngle){
  lcd.setCursor(0,0);
  lcd.print(BLANKLINE);
  lcd.setCursor(0,1);
  lcd.print(BLANKLINE);
  
  lcd.setCursor(0,0);
  lcd.print(currentSpeed);
  lcd.setCursor(7,0);
  lcd.print("->");
  lcd.print(targetSpeed);
  lcd.setCursor(0,1);
  lcd.print(currentAngle);
  lcd.setCursor(7,1);
  lcd.print("->");
  lcd.print(targetAngle);
}

void setBacklight(uint8_t r, uint8_t g, uint8_t b) {
  // common anode so invert!
  r = map(r, 0, 255, 255, 0);
  g = map(g, 0, 255, 255, 0);
  b = map(b, 0, 255, 255, 0);

  analogWrite(REDLITE, r);
  analogWrite(GREENLITE, g);
  analogWrite(BLUELITE, b);
}


void drawTurn(unsigned int currentAngle, unsigned int targetAngle, int currentSpeed){
  int r=currentSpeed/4,g=0,b=currentSpeed;
     if (currentAngle == targetAngle){
       if (currentSpeed>0){
        for (int turnTotal = 0; turnTotal < NUMPIXELS ; turnTotal++){
          if (turnTotal == FRONTLEFT || turnTotal == FRONTLEFT+1){
            pixels.setPixelColor(turnTotal,pixels.Color(r,g,b));
          }else{
            pixels.setPixelColor(turnTotal,pixels.Color(0,0,0));
          }
        }
        pixels.show();
        return;
       }else{
         drawCircle(0,30,0);
       }
     }   
  int brightSpot,darkSpot; 
  brightSpot = ((float)(targetAngle)/MAX_ANGLE_RANGE)*(NUMPIXELS/2);
  darkSpot = ((float)(currentAngle)/MAX_ANGLE_RANGE)*(NUMPIXELS/2);

    brightSpot=FRONTLEFT-brightSpot;
    if (brightSpot<0)
       brightSpot+=NUMPIXELS;
       
    darkSpot=FRONTLEFT-darkSpot;
    if (darkSpot<0)
       darkSpot+=NUMPIXELS;
  
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
  // Christmas variation
  for (int i =0; i< NUMPIXELS; i ++){
    pixels.setPixelColor(i,pixels.Color(r*(i%2),g*((i+1)%2),b*0));
  }
//  for (int i =0; i< NUMPIXELS; i ++){
//    pixels.setPixelColor(i,pixels.Color(r,g,b));
//  }
  pixels.show();
}

void sleepLighting(long now){
 float factor = ((now%10000)-5000)/5000.0;
 if (factor<0)
     factor = factor * -1;
 factor = factor;
 setBacklight(255.0*(factor>.5),255*(factor<.5),0);
 //drawCircle(25.0*factor,0.0*factor,5.0*factor); 
 // Christmas variation
 drawCircle(255.0*factor,255*(1.0-factor),0); 
}
