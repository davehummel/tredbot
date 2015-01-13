#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <Adafruit_NeoPixel.h>
#include <DaveH_HeadingUtils.h>
#include <Adafruit_LSM9DS0.h>


// Which pin on the Arduino is connected to the NeoPixels?
#define PIXEL_PIN            2

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      16
#define FRONTPIXEL      6

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
#define ZERO_COMMAND 'z'

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
//Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

//const float PI = 3.14159265;
const int MIN_STEP_INTERVAL = 10;
const int MAX_STEP_INTERVAL = 255;
const int MAX_TRUE_SIDE_DIF = 360;
const int SLEEP_INTERVAL = 5000;
const int MAX_SPEED_DELTA_INTERVAL = 20;

const String BLANKLINE = "                ";

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotorR = AFMS.getMotor(1);
Adafruit_DCMotor *myMotorL = AFMS.getMotor(2);

const long STARTTIME = 1000 * (millis() / 1000);

void setup() {
  Serial.begin(57600);
  Serial.write("TredBot Starting!\n");
  pinMode(13, OUTPUT);
  delay(1000);
  AFMS.begin();  // create with the default frequency 1.6KHz
  digitalWrite(13, HIGH);
  myMotorL->setSpeed(200);
  myMotorL->run(FORWARD);

  myMotorR->setSpeed(200);
  myMotorR->run(FORWARD);
  // turn on motor
  myMotorR->run(RELEASE);
  myMotorL->run(RELEASE);
  digitalWrite(13, LOW);

  pinMode(REDLITE, OUTPUT);
  pinMode(GREENLITE, OUTPUT);
  pinMode(BLUELITE, OUTPUT);

  lcd.begin(16, 2);
  //pixels.begin();
  Serial.write("TredBot Ready!\n");
  setTop("Ready!");
}

long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1125300L / result; // Back-calculate AVcc in mV
  return result;
}


uint8_t lSpeed = 0, rSpeed = 0;
bool execute = false, debug = true, justPinged = false;
bool lForward = true, rForward = true;
int expSideDif = MAX_TRUE_SIDE_DIF / 2;
int expAngleDelta = 270;
int headingDelta = 0;
int minMotorSpeed = 0;
int speedDeltaPerInterval = MAX_SPEED_DELTA_INTERVAL / 2;
int stepInterval = MIN_STEP_INTERVAL * 2;
int turnDeadZoneHalf = 100;
boolean isSetup = false;
Heading_Util *navUtil;

void loop() {
  uint8_t r = 90, g = 200, b = 255;
  long previousTime = 0;
  long timeDelta = 0;
  long prevCommandTime = 0;
  bool sleeping = false;
  unsigned int currentAngle = 0;
  unsigned int targetAngle = 0;
  int angleRemainder = 0;
  int targetSpeed = 0, currentSpeed = 0;
  int sideDif = 0;
  int prevSideDif;
  int prevHeadingDelta;
  long lastDisplayUpdate = 0;
  lcd.blink();
  Serial.println("Starting Sensor...");
  setTop("starting sensor");
  navUtil = new Heading_Util(Adafruit_LSM9DS0::LSM9DS0_ACCELRANGE_2G, Adafruit_LSM9DS0::LSM9DS0_MAGGAIN_2GAUSS, Adafruit_LSM9DS0::LSM9DS0_GYROSCALE_500DPS);
  if (!navUtil->begin()) {
    setBottom("Failed!");
    while (true);
  }
  runCalibrationCycle();
  Serial.println("Calibrated, now running...");
  while (true) {
    long now = millis();

    if (previousTime <= now - stepInterval) {
      if (lastDisplayUpdate < now - 1000) {
        lastDisplayUpdate = now;
        if (sleeping) {
          setTop("Sleeping...");
          setBottom(String((now - STARTTIME) / 1000) + "s " + String(readVcc() / 1000.0) + "v");
        }

      }
      // This manages the sleeping light effect
      if (sleeping) {
        sleepLighting(now);
      }
      timeDelta = now - previousTime;
      previousTime = now;
      if (Serial.available()) {
        if (sleeping) {
          setStatusOnDisplay(currentSpeed,  targetSpeed, currentAngle, targetAngle);
        }
        sleeping = false;
        prevCommandTime = now;

        setBacklight(0, 255, 0);
        readInput(targetAngle, targetSpeed);
      } else {
        if (!sleeping && prevCommandTime <= now - SLEEP_INTERVAL) {
          sleeping = true;
          currentAngle = targetAngle;
          targetSpeed = 0;
        }
      }
      bool updateMotor = false;
      if (currentSpeed != targetSpeed) {
        updateSpeed(currentSpeed, targetSpeed);
        updateMotor = true;
      }
      
      navUtil->readGyro(true);
      headingDelta = currentAngle;
      currentAngle = navUtil->heading;
      headingDelta = currentAngle-headingDelta;
        angleRemainder = targetAngle - currentAngle;
      if (angleRemainder>MAX_ANGLE_RANGE / 2)
        angleRemainder = MAX_ANGLE_RANGE - angleRemainder;  
      if (abs(angleRemainder)>turnDeadZoneHalf) {
        int temp = sideDif;
        updateDifferential( currentSpeed, angleRemainder,headingDelta, timeDelta, sideDif,prevSideDif,prevHeadingDelta );
        prevSideDif = sideDif;
        prevHeadingDelta = headingDelta;
        updateMotor = true;
      }else if (sideDif!=0){
        sideDif = 0;
        updateMotor = true;
      }
      
      if (updateMotor) {
        sendNavStats(currentSpeed, currentAngle);
        //drawTurn(currentAngle, targetAngle, currentSpeed);
        setStatusOnDisplay(currentSpeed,  targetSpeed, currentAngle, targetAngle);
        setMotorSpeeds(currentSpeed, sideDif);

      } else if (justPinged) {
        justPinged = false;
        sendNavStats(currentSpeed, currentAngle);
      }

    }
  }
}

void sendNavStats(int currentSpeed, unsigned int currentAngle) {
  Serial.write(">");
  Serial.print(currentSpeed);
  Serial.write(":");
  Serial.println(currentAngle);
}

void updateSpeed(int &currentSpeed, int targetSpeed) {
  
  int dif = targetSpeed - currentSpeed;
  if (dif > 0) {
    if (dif > speedDeltaPerInterval)
      dif = speedDeltaPerInterval;
  } else {
    if (dif * -1 > speedDeltaPerInterval)
      dif = -1 * speedDeltaPerInterval;
  }
  currentSpeed += dif;
}

void updateDifferential(int currentSpeed, int angleRemainder, int angleDelta, int timeDelta, int &sideDif, int prevSideDif, int prevAngleDelta) {

//  
//  float expAngleDelta = expAnglePerIntervalStopped*stopFrac + expAnglePerIntervalFast*fastFrac;
//  expAngleDelta*=timeDelta;
//  
  bool isLeft = angleRemainder<0;
//  
//  if (isLeft)
//     expAngleDelta*=-1;
//  
  if(debug){
  Serial.write("Time Delta:");
  Serial.println(timeDelta);
  Serial.print("Angle Remaining:");
  Serial.println(angleRemainder);
  Serial.print("Act Delta:");
  Serial.println(angleDelta);
  }
//
//  // First try or Someone is moving the bot the opposite direction we are trying to go .. revert to default turn speed
//  if ( sideDif == 0 || (expAngleDelta>0 && angleDelta<0) || (expAngleDelta<0 && angleDelta>0)){
//    sideDif=isLeft?expSideDif*-1:expSideDif;
//    angleDelta = expAngleDelta;
//  }
//  
//  if (isLeft){
//    if (expAngleDelta<angleRemainder){
//      expAngleDelta= angleRemainder;
//    }
//  }else{
//    if (expAngleDelta>angleRemainder){
//      expAngleDelta = angleRemainder;
//    }
//  }
//  
//    if (angleDelta == 0){
//      expAngleDelta = 2;
//    }else{
//      expAngleDelta = expAngleDelta/angleDelta;
//    }
//    if (expAngleDelta > 2)
//      expAngleDelta = 2;

    int effectiveDelta = 0;
    
    if (sideDif == 0){
      if (isLeft){
        sideDif=-expSideDif;
        effectiveDelta = -expAngleDelta;
      }else{
        sideDif=expSideDif;
        effectiveDelta = expAngleDelta;
      }
    } else{
      effectiveDelta = (angleDelta +prevAngleDelta )/2;
    }
    float reducer = 1;
    if (effectiveDelta != 0){  
      reducer = angleRemainder/(effectiveDelta*8.0);
      if (reducer>1.3)
        reducer = 1.3;
      if ( reducer > 0)
         sideDif *= reducer;
      else // Going the wrong direction
         sideDif = 0;
    }
  
    if (sideDif>MAX_TRUE_SIDE_DIF)
      sideDif = MAX_TRUE_SIDE_DIF;
    else if (sideDif<-MAX_TRUE_SIDE_DIF)
      sideDif = -MAX_TRUE_SIDE_DIF;
    else if (sideDif >1 && sideDif < minMotorSpeed)
      sideDif = minMotorSpeed *.8;
    else if (sideDif <-1 && sideDif > -minMotorSpeed)
      sideDif = minMotorSpeed *-.8;
      
    
    if (debug){
    Serial.print(angleRemainder);
    Serial.print("/");
    Serial.print(effectiveDelta);
    Serial.print("=");
    Serial.println(reducer);
    }
 
}

void setMotorSpeeds(int currentSpeed, int sideDif) {
//  if (sideDif<0){
//        myMotorL->setSpeed(0);
//        myMotorR->setSpeed(0);
//        for (int i = 0 ; i < 10; i++){
//          navUtil->readGyro(true);
//          Serial.print("}}");
//          Serial.println(navUtil->heading);
//          delay(40);
//        }
//        delay(1000000);
//        execute = false;
//  }
    
  if (debug) {
    Serial.write("T+");
    Serial.print((millis() - STARTTIME) / (float)1000);
    Serial.write("s>CS|SD|TL|TR>");
    Serial.print(currentSpeed);
    Serial.write("|");
    Serial.print(sideDif);
    Serial.write("|");
  }
  if (sideDif>MAX_TRUE_SIDE_DIF)
     sideDif = MAX_TRUE_SIDE_DIF;
  if (sideDif<-MAX_TRUE_SIDE_DIF)
     sideDif = -MAX_TRUE_SIDE_DIF;
  
  
  
  int left = currentSpeed;
  int right = currentSpeed;
  
  int offset = sideDif/2;
  
  if (sideDif < 0 && offset*-1<minMotorSpeed){
    right-=sideDif;
  }else if (sideDif>0 && offset<minMotorSpeed){
    left+=sideDif;
  } else {
    left+=offset;
    right-=offset;
  }

  // If only one component is too big, shift
  if (left > 255) {
    right -= left - 255;
    left = 255;
  } else if (left < -255) {
    right += left + 255;
    left = -255;
  } else if (right > 255) {
    left -= right - 255;
    right = 255;
  } else if (right < -255) {
    left += right + 255;
    right = -255;
  }
  
  //left = right = currentSpeed;

  if (left == 0) {
    lForward = true;
    myMotorL->run(BACKWARD);
    lSpeed = 0;
  } else if (left > 0) {
    lForward = true;
    myMotorL->run(BACKWARD);
    lSpeed = left;
  } else {
    lForward = false;
    myMotorL->run(FORWARD);
    lSpeed = -left;
  }

  if (right == 0) {
    rForward = true;
    myMotorR->run(BACKWARD);
    rSpeed = 0;
  } else if (right > 0) {
    rForward = true;
    myMotorR->run(BACKWARD);
    rSpeed = right;
  } else {
    rForward = false;
    myMotorR->run(FORWARD);
    rSpeed = -right;
  }
 // lSpeed = map(lSpeed,0,255,minLeftSpeed,255);
 // rSpeed = map(rSpeed,0,255,minRightSpeed,255);
  if (execute) {
    myMotorL->setSpeed(lSpeed);
    myMotorR->setSpeed(rSpeed);
  } else {
    myMotorL->setSpeed(0);
    myMotorR->setSpeed(0);
  }
  if (debug) {
    Serial.write("|");
    Serial.print(lSpeed);
    Serial.write(lForward ? "F|" : "B|");
    Serial.print(rSpeed);
    Serial.write(rForward ? "F<" : "B<");
    Serial.write("\n");
  }
}

void readInput(unsigned int &targetAngle, int &targetSpeed) {
  char command[1];
  Serial.readBytes(command, 1);

  switch (command[0]) {
    case UPDATE_COMMAND: // update -- command to execute a movement with rotation and velocity
      targetSpeed = Serial.parseInt();
      if (targetSpeed > 255)
        targetSpeed = 255;
      else if (targetSpeed < -255)
        targetSpeed = -255;
      targetAngle = Serial.parseInt();

      Serial.readBytes(command, 1);
      Serial.write("OK\n");
      return;
      break;
    case DEBUG_COMMAND:  // debug -- command to print a movement with rotation and velocity
      debug = !debug;
      Serial.write("OK:");
      Serial.write(debug ? "true\n" : "false\n");
      return;
    case EXECUTE_COMMAND: // execute -- command to determine if movements are executed by motors or just simulated
      execute = !execute;
      Serial.write("OK:");
      Serial.write(execute ? "true\n" : "false\n");
      return;
      break;
    case PING_COMMAND: // ping -- does nothing but keep from sleeping
      justPinged = true;
      return;
    case CONFIG_COMMAND: // configure

      stepInterval = Serial.parseInt();
      if (stepInterval < MIN_STEP_INTERVAL) {
        stepInterval = MIN_STEP_INTERVAL;
      } else if (stepInterval > MAX_STEP_INTERVAL) {
        stepInterval = MAX_STEP_INTERVAL;
      }

      speedDeltaPerInterval =  Serial.parseInt();
      if (speedDeltaPerInterval > MAX_SPEED_DELTA_INTERVAL) {
        speedDeltaPerInterval = MAX_SPEED_DELTA_INTERVAL;
      } else if (speedDeltaPerInterval < 1) {
        speedDeltaPerInterval = 1;
      }

 
      expSideDif =  Serial.parseInt();
      if (expSideDif < 1)
        expSideDif = 1;
      if (expSideDif>500)
        expSideDif = 500;
     
      isSetup = true;
      Serial.readBytes(command, 1);
      Serial.write("OK\n");
      return;
    case ZERO_COMMAND: // zero sensors
      Serial.write("OK:");

          runCalibrationCycle(); 
      
      return;
    case GET_CONFIG_COMMAND: // get configuration
      Serial.write("OK:");
      Serial.print(isSetup);
      Serial.print(' ');
      Serial.print(stepInterval);
      Serial.write(' ');
      Serial.print(speedDeltaPerInterval);
      Serial.write(' ');
      Serial.print(expSideDif);
      Serial.write('\n');
      return;
    default:
      Serial.write("Unknown Command\n");
  }
  return;
}

void setTop(String topLine) {
  lcd.setCursor(0, 0);
  lcd.print(BLANKLINE);
  lcd.setCursor(0, 0);
  lcd.print(topLine);
}
void setBottom(String bottomLine) {
  lcd.setCursor(0, 1);
  lcd.print(BLANKLINE);
  lcd.setCursor(0, 1);
  lcd.print(bottomLine);
}

void setStatusOnDisplay(int currentSpeed, int targetSpeed, unsigned int currentAngle, unsigned int targetAngle) {
  lcd.setCursor(0, 0);
  lcd.print(BLANKLINE);
  lcd.setCursor(0, 1);
  lcd.print(BLANKLINE);

  lcd.setCursor(0, 0);
  lcd.print(currentSpeed);
  lcd.setCursor(7, 0);
  lcd.print("->");
  lcd.print(targetSpeed);
  lcd.setCursor(0, 1);
  lcd.print(currentAngle);
  lcd.setCursor(7, 1);
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


void drawTurn(unsigned int currentAngle, unsigned int targetAngle, int currentSpeed) {
//  int r = currentSpeed / 4, g = 0, b = currentSpeed;
//  if (currentAngle == targetAngle) {
//    if (currentSpeed > 0) {
//      for (int turnTotal = 0; turnTotal < NUMPIXELS ; turnTotal++) {
//        if (turnTotal == FRONTPIXEL) {
//          pixels.setPixelColor(turnTotal, pixels.Color(r, g, b));
//        } else {
//          pixels.setPixelColor(turnTotal, pixels.Color(0, 10, 0));
//        }
//      }
//      pixels.show();
//      return;
//    } else if (currentSpeed < 0) {
//      int backPixel = FRONTPIXEL - NUMPIXELS / 2;
//      if (backPixel < 0) backPixel += NUMPIXELS;
//      for (int turnTotal = 0; turnTotal < NUMPIXELS ; turnTotal++) {
//        if (turnTotal == backPixel) {
//          pixels.setPixelColor(turnTotal, pixels.Color(255, 0, 0));
//        } else {
//          pixels.setPixelColor(turnTotal, pixels.Color(20, 0, 0));
//        }
//      }
//      pixels.show();
//      return;
//    } else {
//      drawCircle(20, 30, 0);
//      return;
//    }
//  }
//  int brightSpot, darkSpot;
//  int div = MAX_ANGLE_RANGE / NUMPIXELS;
//  brightSpot = targetAngle / div;
//  darkSpot = currentAngle / div;
//
//  brightSpot = darkSpot - brightSpot + FRONTPIXEL;
//  darkSpot = FRONTPIXEL;
//
//  while (brightSpot < 0 || brightSpot >= NUMPIXELS) {
//    if (brightSpot < 0)
//      brightSpot += NUMPIXELS;
//    else
//      brightSpot -= NUMPIXELS;
//  }
//
//
//  //  Serial.write("[");
//  //  Serial.print(brightSpot);
//  //  Serial.write(",");
//  //  Serial.print(darkSpot);
//  //  Serial.write("]\n");
//
//  for (int i = 0; i < NUMPIXELS; i ++) {
//    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
//  }
//
//  pixels.setPixelColor(brightSpot, pixels.Color(r, g, b));
//  pixels.setPixelColor(darkSpot, pixels.Color(r / 3, g / 3, b / 3));
//
//
//  pixels.show();
}

void drawCircle(uint8_t r, uint8_t g, uint8_t b) {
//  for (int i = 0; i < NUMPIXELS; i ++) {
//    pixels.setPixelColor(i, pixels.Color(r, g, b));
//  }
//  pixels.show();
}

void sleepLighting(long now) {
  float factor = ((now % 10000) - 5000) / 5000.0;
  if (factor < 0)
    factor = factor * -1;

  setBacklight(120 * factor, 0, 0);

  drawCircle(20.0 * factor, 0, 30 * factor);
}

void runCalibrationCycle(){
  // Clear any residue in the sensor by doing a read
  navUtil->readAll(false);
  setTop("Calibrating");
  setBottom("Gyro Sensor");
   Serial.println(navUtil->gyroStats.min.z); 
  navUtil->clearCalibration();
   Serial.println(navUtil->gyroStats.min.z); 
  navUtil->readAll(false);
  navUtil->setCalibrating(true);
  Serial.println(navUtil->gyroStats.min.z); 
  for (int i =0; i < 200;i++){
     delay(20)                                                                                                                                                                  ;
     navUtil->readAll(false);
     if (debug)
       Serial.println(navUtil->gyroData.z);
  }
  navUtil->setCalibrating(false);
   Serial.println(navUtil->gyroStats.min.z); 
  minMotorSpeed = tuneMotor(false); 
 
  delay(500);
  
  int temp = tuneMotor(true);
   Serial.println(navUtil->gyroStats.min.z); 
  if (temp>minMotorSpeed)
      minMotorSpeed = temp;
  
  Serial.write("Gyro Z: Offset=");Serial.print(navUtil->gyroStats.error.z);
  Serial.write(" max:");Serial.print(navUtil->gyroStats.max.z);
  Serial.write(" min:");Serial.println(navUtil->gyroStats.min.z);
  Serial.write(" MinMotorSpeed:");Serial.println(minMotorSpeed);
  
  navUtil->readAll(true);
  
  navUtil->heading = 0;
  
}

int tuneMotor(bool right){
 navUtil->readAll(false);
 Adafruit_DCMotor *motor = right?myMotorR:myMotorL;
  motor->setSpeed(0);
  motor->run(BACKWARD);
  
  for (int i =40;i <=255; i+=5){
    setTop("Speed:"+String(i));
    motor->setSpeed(i);
    delay(50);
    float sum = 0;
    for (int j = 0; j < 5 ; j++){
      navUtil->readGyro(false);
      sum+=navUtil->gyroData.z;
    }
    setBottom(String(sum));

    if (right? sum<-5 : sum>5){
      Serial.write("Min Motor Speed:");
      Serial.println(i);
      motor->setSpeed(0); 
      return i;
    }else if ((right?sum>5:sum<-5) || i == 255){
      Serial.println("Motor Not Wired Properly!!");
      setBacklight(255,0,0);
      setTop((right?"Left Motor":"Left Motor"));
      if (i==255)
        setBottom("Not Connected?");
      else
        setBottom("Inverted?");
      motor->setSpeed(0);
      delay(500);
      return -1;
    }
  }

}
