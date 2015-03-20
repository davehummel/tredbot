/* 
http://pulsedlight3d.com
This sketch demonstrates getting distance with the LIDAR-Lite Sensor
It utilizes the 'Arduino Wire Library'

Modified to Teensy 3.1 by Kim Janson, www.levitezer.com
Sensor connected to pins 18 (SDA) and 19 (SCL), power to GND and Vin and there is a 4.7 uF capasitor, not sure if needed.
No other resistors etc. Powered from USB, based on half hour testing works ok.
Sorry for messy code.
*/
#include <i2c_t3.h>
//#include <Wire.h>
#define LIDARLite_ADDRESS 0x62 // Default I2C Address of LIDAR-Lite.
#define RegisterMeasure 0x00 // Register to write to initiate ranging.
#define MeasureValue 0x04 // Value to initiate ranging.
#define RegisterHighLowB 0x8f // Register to get both High and Low bytes in 1 call.

byte MeasureArray[2];
int reading = 0;
int reading2 = 0;
long time;
uint8_t nackack = 100;
void setup()
{
Wire.begin(I2C_MASTER,0x00 ,I2C_PINS_18_19, I2C_PULLUP_INT, I2C_RATE_100); 
Wire.setOpMode(I2C_OP_MODE_ISR);
Serial.begin(115200); // start serial communication at 9600bps
MeasureArray[0]=0x00; // Register to write to initiate ranging.
MeasureArray[1]=0x04; // Value to initiate ranging.
}

void loop()
{

nackack = 0; //initiate ranging.
while ( nackack == 0){ 
Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
nackack = Wire.write( MeasureArray,2); if (nackack == 0){delay(1);} } 

nackack = 100; //end transmission
while ( nackack != 0){ nackack = Wire.endTransmission(); if (nackack != 0){delay(1);} }

delay(15); 
/*http://kb.pulsedlight3d.com/support/...ck-start-guide
"Periodically poll the unit and wait until an ACK is received. The unit responds to read 
or write requests with a NACK when the sensor is busy processing a command or performing a 
measurement. (Optionally, wait approx. 20 milliseconds after acquisition and then proceed
to read high and low bytes)"
15 ms gives about 17 to 18 ms reads. Would like to use polling, but how?*/

nackack = 0;
while ( nackack == 0){ //if error write again
Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
nackack = Wire.write((int)RegisterHighLowB); // sets register pointer to (0x8f)
if (nackack == 0){delay(1);} }

nackack = 100;
while ( nackack != 0){ // if not sucses (0) do a gain
nackack = Wire.endTransmission(); // stop transmitting
if (nackack != 0){delay(1);}
}
// delay(20); // Wait 20ms for transmit
nackack = 0;
while ( nackack == 0){ nackack = Wire.requestFrom((int)LIDARLite_ADDRESS, 2); if (nackack == 0){delay(1);}} // request 2 bytes from LIDAR-Lite

delay(1);
if(2 == Wire.available()) // if two bytes were received
{
delay(1);

reading = -1;
while ( reading == -1){ reading = Wire.read(); if (reading == -1){delay(1);}}
reading = reading << 8; // shift high byte to be high 8 bits

reading2 = -1;
while ( reading2 == -1){ reading2 = Wire.read(); if (reading2 == -1){delay(1);}}
reading |= reading2; // receive low byte as lower 8 bits
// Serial.println(reading); // print the reading

// Print update rate and distance
Serial.print(millis()-time);
Serial.print(" ");
Serial.println(reading);
time=millis();
}
}
