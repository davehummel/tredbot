#include <i2c_t3.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

elapsedMillis sincePrint;

void setup()
{
  //Wire.begin(); // join i2c bus
  Wire.begin(I2C_MASTER, 0, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_100, I2C_OP_MODE_DMA );
  Serial.begin(115200); 
  pinMode(13,OUTPUT);
  digitalWrite(13, HIGH);
  delay(100);  // give the lidar time to startup
}


void loop(){
  int out ;
  Serial.print(sincePrint);
  sincePrint = 0;

  Serial.print(">");
  Serial.println(out = readDistance());

  if (out == 0){
    Serial.println("Getting sick");
  }

}

int readDistance(){
  uint8_t nackack = 100; // Setup variable to hold ACK/NACK resopnses     
  while (nackack != 0){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
   // Wait 1 ms to prevent overpolling
    Wire.beginTransmission(LIDARLite_ADDRESS);
    Wire.write(RegisterMeasure);
    Wire.write(MeasureValue);
    nackack = Wire.endTransmission(I2C_STOP,1000);
    if (nackack!=0){
        Serial.print("A");
        Serial.print(nackack);
        }
  }
  delay(18);
  nackack = 100;
  uint8_t trys = 0;
  while (nackack != 0){ 

    Wire.beginTransmission(LIDARLite_ADDRESS);
    Wire.write(RegisterHighLowB);
    nackack = Wire.endTransmission(I2C_STOP,1000);
    delay(1);
    if (nackack!=0){
      Serial.print("C");
      Serial.print(nackack);
          delay(1);
      continue;
    }
    Wire.requestFrom(LIDARLite_ADDRESS,2,I2C_STOP,1000);
  }

  int reading = Wire.readByte() ;
  reading = reading << 8;
  reading |= Wire.readByte();
  return reading;
 
}
