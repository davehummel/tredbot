
#include <i2c_t3.h>

#define SensorAddress1 byte(0x70)
#define SensorAddress2 byte(0x71)
//The Sensor ranging command has a value of 0x51
#define RangeCommand byte(0x51)
//These are the two commands that need to be sent in sequence to change the sensor address
#define ChangeAddressCommand1 byte(0xAA)
#define ChangeAddressCommand2 byte(0xA5)


#define A_OFFSET 3
#define B_OFFSET 3

void setup() {

  Serial.begin(115200);
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_800,I2C_OP_MODE_DMA);
  delay(1000);
  
}

uint16_t reading;
void loop() {

   Wire.beginTransmission(SensorAddress2);             //Start addressing 
   Wire.write(RangeCommand);                             //send range command 
   Wire.endTransmission();   
   delay (50);
   Wire.beginTransmission(SensorAddress1);             //Start addressing 
   Wire.write(RangeCommand);                             //send range command 
   Wire.endTransmission();   
   Wire.requestFrom(SensorAddress2, byte(2));
   if(Wire.available() >= 2){                            //Sensor responded with the two bytes 
      byte HighByte = Wire.read();                        //Read the high byte back 
      byte LowByte = Wire.read();                        //Read the low byte back 
      reading  = word(HighByte, LowByte)+A_OFFSET;         //Make a 16-bit word out of the two bytes for the range  
   }else{
    reading = 0; 
   }
   Serial.print(reading);
   Serial.print(" ");
   delay(50);
     Wire.requestFrom(SensorAddress1, byte(2));
   if(Wire.available() >= 2){                            //Sensor responded with the two bytes 
      byte HighByte = Wire.read();                        //Read the high byte back 
      byte LowByte = Wire.read();                        //Read the low byte back 
      reading  = word(HighByte, LowByte)+A_OFFSET;         //Make a 16-bit word out of the two bytes for the range  
   }else{
    reading = 0; 
   }
   Serial.println(reading);
}


