 #include <i2c_t3.h>
 #include <Adafruit_INA219.h>



//The Arduino Wire library uses the 7-bit version of the address, so the code example uses 0x70 instead of the 8‑bit 0xE0
#define SensorAddress1 byte(0x70)
#define SensorAddress2 byte(0x71)
//The Sensor ranging command has a value of 0x51
#define RangeCommand byte(0x51)
//These are the two commands that need to be sent in sequence to change the sensor address
#define ChangeAddressCommand1 byte(0xAA)
#define ChangeAddressCommand2 byte(0xA5)


 Adafruit_INA219 *powerMonitor;

 float shuntvoltage ;
 float busvoltage; 
 float current_mA;
 float loadvoltage;

bool powerMonitorDataReady = false;

void setupPowerMonitor(){
  powerMonitor = new Adafruit_INA219();
  powerMonitor->begin();  
}

void executePowerMonitor(){
      shuntvoltage = powerMonitor->getShuntVoltage_mV();
      busvoltage = powerMonitor->getBusVoltage_V();
      current_mA =powerMonitor->getCurrent_mA();
      loadvoltage = busvoltage + (shuntvoltage / 1000);
      powerMonitorDataReady = true;
      Serial1.print("Voltage:");
      Serial1.println(loadvoltage);
      Serial1.print("Current:");
      Serial1.println(current_mA);
}

  uint16_t range1,range2;
void setupSonarReading(){
	Wire.begin();
}

void executeSonarReading(){
	Wire.beginTransmission(SensorAddress1);             //Start addressing 
    Wire.write(RangeCommand);                             //send range command 
    Wire.endTransmission();     
     delay(50);      
    Wire.requestFrom(SensorAddress1, byte(2));
        if(Wire.available() >= 2){                            //Sensor responded with the two bytes 
            byte HighByte = Wire.read();                        //Read the high byte back 
            byte LowByte = Wire.read();                        //Read the low byte back 
            range1 = word(HighByte, LowByte);         //Make a 16-bit word out of the two bytes for the range  
        }else {
        	range1 = 0;
        }    
     Serial1.print("Range1:");
     Serial1.println(range1);   
          Serial1.println("a"); 
    Wire.beginTransmission(SensorAddress2);             //Start addressing 
    Wire.write(RangeCommand);                             //send range command 
    Wire.endTransmission();   
              Serial1.println("b");   
     delay(50);      
    Wire.requestFrom(SensorAddress2, byte(2));
        if(Wire.available() >= 2){                            //Sensor responded with the two bytes 
            byte HighByte = Wire.read();                        //Read the high byte back 
            byte LowByte = Wire.read();                        //Read the low byte back 
            range2 = word(HighByte, LowByte);         //Make a 16-bit word out of the two bytes for the range  
        }else {
        	range2 = 0;
        }    
     Serial1.print("Range2:");
     Serial1.println(range2);                       
}

void setup() {
	Serial1.begin(115200);
	Serial.begin(115200);
	pinMode(13,OUTPUT);
	setupPowerMonitor();
	setupSonarReading();
}

bool flip = false;
void loop() {
    if (Serial1.available()){
     	digitalWrite(13,flip);
     	flip = ! flip;
     	switch(Serial1.read()){
     		case 'p':
     			executePowerMonitor();
     			break;
     		case 's':
     			executeSonarReading();
     			break;
 	}

  }

}

