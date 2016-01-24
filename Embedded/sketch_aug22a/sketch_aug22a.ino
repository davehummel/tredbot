#include <i2c_t3.h>

#define SensorAddress1 byte(0x70)
#define SensorAddress2 byte(0x71)
//The Sensor ranging command has a value of 0x51
#define RangeCommand byte(0x51)

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100 );

  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

    delay(2000);
      Serial.println("Starting");
      

}

  uint8_t addr = SensorAddress1;//0x71

 
void loop() {
 			 Wire.beginTransmission(addr);             //Start addressing 
   Wire.write(RangeCommand);                             //send range command 
  Wire.endTransmission();  
  delay(40);
  Wire.requestFrom(addr, byte(2));
  byte HighByte = Wire.read();                        //Read the high byte back 
  byte LowByte = Wire.read();                        //Read the low byte back 
			            uint16_t reading = word(HighByte, LowByte);  
  Serial.println(reading);
}

/*
void loop() {
 long time;
    Serial1.print("X101|");
    Serial1.print(time = millis() );
    Serial1.println("|START!!");
    boolean again = true;
    while(again){
      while (Serial1.available()){
     
        char in = Serial1.read();
        if (in == 'T'){
          doTime = true;
        }
        if (in == '\n'){
            Serial.println(millis()-time);
            Serial1.print("X102|");
            Serial1.print( millis() );
            Serial1.println("|END!!");
            again = false;
            break;
        }
      }
  }
  
  if (doTime){
    doTime = false;
    Serial1.print("T1|");
    Serial1.print(millis());
    Serial1.println("|");
  }
  
  flip = !flip;
  
}*/
