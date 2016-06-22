/* VL6180X_t3 Basic Example Code
 by: Kris Winer
 date: September 1, 2014
 license: Beerware - Use this code however you'd like. If you 
 find it useful you can buy me a beer some time.
 
 Demonstrates basic VL6180X proximity sensor functionality including parameterizing 
 the register addresses, initializing the sensor, getting range and ambient light sening data out.
 Eventually will use this sensor to recognize hand gestures and control 
 functions througha microcontroller like Teensy 3.1.
 
 Added display functions to allow display to on breadboard monitor. 
 
 Sketch runs on the Teensy 3.1.
 
 This sketch is intended specifically for the VL6180X breakout board and Add-On Shield for the Teensy 3.1.
 It uses SDA/SCL on pins 17/16, respectively, and it uses the Teensy 3.1-specific Wire library i2c_t3.h.
 
 The Add-on shield can also be used as a stand-alone breakout board for any Arduino, Teensy, or 
 other microcontroller.
 
 The sensor communicates via I2C at 400 Hz or higher.
 SDA and SCL have 4K7 pull-up resistors (to 2.8 V) on the breakout board.
 
 Hardware setup:
 Breakout Board --------- Arduino/Teensy
 3V3 ---------------------- VIN or any digital GPIO pin with digitalWrite(HIGH)!
 SDA -----------------------A4/17
 SCL -----------------------A5/16
 GND ---------------------- GND or any digital GPIO pin with digitalWrite(LOW)!
 
 Note: The VL6180X breakout board is an I2C sensor and uses the Arduino Wire or Teensy i2c_t3.h library. 
 Even though the voltages up to 5 V, the sensor is not 5V tolerant and we are using a 3.3 V 8 MHz Pro Mini or a 3.3 V Teensy 3.1.
 We are also using the 400 kHz fast I2C mode by setting the TWI_FREQ  to 400000L /twi.h utility filefor the Pro Mini useage.
 The Teensy has no internal pullups and we are using the Wire.begin function of the i2c_t3.h library
 to select 400 Hz i2c speed.
 */
//#include <Wire.h>   
#include <i2c_t3.h>

// VL6180X Data Sheet and Register Map Register Map
// http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00112632.pdf
// The VL6180X uses 16-bit register addresses rather than the usual 8-bit (2-byte) address found in most
// other sensors. This requires a slight modification to the read and write functions but is straightforward.
//
// VL6180X registers
#define VL6180X_WHO_AM_I                             0x0000   // should be 0xB4              
#define VL6180X_IDENTIFICATION_MODEL_ID              0x0000
#define VL6180X_IDENTIFICATION_MODEL_REV_MAJOR       0x0001
#define VL6180X_IDENTIFICATION_MODEL_REV_MINOR       0x0002
#define VL6180X_IDENTIFICATION_MODULE_REV_MAJOR      0x0003
#define VL6180X_IDENTIFICATION_MODULE_REV_MINOR      0x0004
#define VL6180X_IDENTIFICATION_DATE_HI               0x0006
#define VL6180X_IDENTIFICATION_DATE_LO               0x0007
#define VL6180X_IDENTIFICATION_TIME_HI               0x0008
#define VL6180X_IDENTIFICATION_TIME_LO               0x0009
#define VL6180X_SYSTEM_MODE_GPIO0                    0x0010
#define VL6180X_SYSTEM_MODE_GPIO1                    0x0011
#define VL6180X_SYSTEM_HISTORY_CTRL                  0x0012
#define VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO         0x0014
#define VL6180X_SYSTEM_INTERRUPT_CLEAR               0x0015
#define VL6180X_SYSTEM_FRESH_OUT_OF_RESET            0x0016
#define VL6180X_SYSTEM_GROUPED_PARAMETER_HOLD        0x0017
#define VL6180X_SYSRANGE_START                       0x0018
#define VL6180X_SYSRANGE_THRESH_HIGH                 0x0019
#define VL6180X_SYSRANGE_THRESH_LOW                  0x001A
#define VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD     0x001B
#define VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME        0x001C
#define VL6180X_SYSRANGE_CROSSTALK_COMPENSATION_RATE 0x001E
#define VL6180X_SYSRANGE_CROSSTALK_VALID_HEIGHT      0x0021
#define VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE  0x0022
#define VL6180X_SYSRANGE_PART_TO_PART_RANGE_OFFSET   0x0024
#define VL6180X_SYSRANGE_RANGE_IGNORE_VALID_HEIGHT   0x0025
#define VL6180X_SYSRANGE_RANGE_IGNORE_THRESHOLD      0x0026
#define VL6180X_SYSRANGE_MAX_AMBIENT_LEVEL_MULT      0x002C
#define VL6180X_SYSRANGE_RANGE_CHECK_ENABLES         0x002D
#define VL6180X_SYSRANGE_VHV_RECALIBRATE             0x002E
#define VL6180X_SYSRANGE_VHV_REPEAT_RATE             0x0031
#define VL6180X_SYSALS_START                         0x0038
#define VL6180X_SYSALS_THRESH_HIGH                   0x003A
#define VL6180X_SYSALS_THRESH_LOW                    0x003C
#define VL6180X_SYSALS_INTERMEASUREMENT_PERIOD       0x003E
#define VL6180X_SYSALS_ANALOGUE_GAIN                 0x003F
#define VL6180X_SYSALS_INTEGRATION_PERIOD            0x0040
#define VL6180X_RESULT_RANGE_STATUS                  0x004D
#define VL6180X_RESULT_ALS_STATUS                    0x004E
#define VL6180X_RESULT_INTERRUPT_STATUS_GPIO         0x004F
#define VL6180X_RESULT_ALS_VAL                       0x0050
#define VL6180X_RESULT_HISTORY_BUFFER0               0x0052 // This is a FIFO buffer that can store 8 range values or 16 ALS values
#define VL6180X_RESULT_HISTORY_BUFFER1               0x0053 // It would be read in burst mode so all that is
#define VL6180X_RESULT_HISTORY_BUFFER2               0x0054 // needed would be to reference the first address
#define VL6180X_RESULT_HISTORY_BUFFER3               0x0055
#define VL6180X_RESULT_HISTORY_BUFFER4               0x0056
#define VL6180X_RESULT_HISTORY_BUFFER5               0x0057
#define VL6180X_RESULT_HISTORY_BUFFER6               0x0058
#define VL6180X_RESULT_HISTORY_BUFFER7               0x0059
#define VL6180X_RESULT_HISTORY_BUFFER8               0x0060  // end of FIFO
#define VL6180X_RESULT_RANGE_VAL                     0x0062
#define VL6180X_RESULT_RANGE_RAW                     0x0064
#define VL6180X_RESULT_RANGE_RETURN_RATE             0x0066
#define VL6180X_RESULT_RANGE_REFERENCE_RATE          0x0068
#define VL6180X_RESULT_RANGE_RETURN_SIGNAL_COUNT     0x006C
#define VL6180X_RESULT_RANGE_REFERENCE_SIGNAL_COUNT  0x0070
#define VL6180X_RESULT_RANGE_RETURN_AMB_COUNT        0x0074
#define VL6180X_RESULT_RANGE_REFERENCE_AMB_COUNT     0x0078
#define VL6180X_RESULT_RANGE_RETURN_CONV_TIME        0x007C
#define VL6180X_RESULT_RANGE_REFERENCE_CONV_TIME     0x0080
#define VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD      0x010A
#define VL6180X_FIRMWARE_BOOTUP                      0x0119
#define VL6180X_FIRMWARE_RESULT_SCALER               0x0120
#define VL6180X_I2C_SLAVE_DEVICE_ADDRESS             0x0212
#define VL6180X_INTERLEAVED_MODE_ENABLE              0x02A3

// Using the VL6180X breakout board/Teensy 3.1 Add-On Shield, device address is 0x29
#define VL6180X_ADDRESS  0x29   //  Device address of VL6180X  
  
// Pin definitions
int myLed = 13;
int powerPin = 9;
int gndPin   = 8;
float als;

enum VL6180XMode {
  contRangeMode = 0,
  contALSMode,
  interLeaveMode
};

enum ALSGain {  // define lower nibble of ALS gain register
  alsGain20 = 0,  // ALS gain of 20
  alsGain10,      // ALS gain of 10.32
  alsGain5,       // ALS gain of 5.21
  alsGain2_5,     // ALS gain of 2.60
  alsGain1_67,    // ALS gain of 1.72
  alsGain1_25,    // ALS gain of 1.28
  alsGain1_0,     // ALS gain of 1.01
  alsGain40       // ALS gain of 40
};
  
  
uint8_t VL6180XMode = interLeaveMode;
uint8_t ALSGain = alsGain20;
float realalsGain;
#define verboseMode  false

void setup()
{
//  Wire.begin();
//  TWBR = 12;  // 400 kbit/sec I2C speed for Pro Mini
  // Setup for Master mode, pins 16/17, external pullups, 400kHz for Teensy 3.1
  Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, I2C_RATE_400);

  Serial.begin(38400);
  
  // Set up the interrupt pin, its set as active high, push-pull
  pinMode(myLed, OUTPUT);
  digitalWrite(myLed, LOW);
  
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
  pinMode(gndPin, OUTPUT);
  digitalWrite(gndPin, LOW);
  

  // Read the WHO_AM_I register, this is a good test of communication
  Serial.println("VL6180X proximity sensor...");
  byte c = readByte(VL6180X_ADDRESS, VL6180X_WHO_AM_I);  // Read WHO_AM_I register for VL6180X
  Serial.print("VL6180X "); Serial.print("I AM "); Serial.print(c, HEX); Serial.print(" I should be "); Serial.println(0xB4, HEX);

  // Get info about the specific device
  byte majmodrev = readByte(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_MODEL_REV_MAJOR);  // Read MAJOR_MODEL_REVISION register for VL6180X
  Serial.print("VL6180X major model revision "); Serial.println((majmodrev & 0x07));  
  byte minmodrev = readByte(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_MODEL_REV_MINOR);  // Read MINOR_MODEL_REVISION register for VL6180X
  Serial.print("VL6180X minor model revision "); Serial.println((minmodrev & 0x07));  
  byte majmodurev = readByte(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_MODULE_REV_MAJOR);  // Read MAJOR_MODULE_REVISION register for VL6180X
  Serial.print("VL6180X major module revision "); Serial.println((majmodurev & 0x07));  
  byte minmodurev = readByte(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_MODULE_REV_MINOR);  // Read MINOR_MODULE_REVISION register for VL6180X
  Serial.print("VL6180X minor module revision "); Serial.println((minmodurev & 0x07));  
  byte dateh = readByte(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_DATE_HI);  // Read IDENTIFICATION_DATE_HI register for VL6180X
  Serial.print("VL6180X manufactured in 201"); Serial.print((dateh & 0xF0) >> 4 ); Serial.print(", month ");  Serial.println(dateh & 0x0F); 
  byte datel = readByte(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_DATE_LO);  // Read IDENTIFICATION_DATE_LO register for VL6180X
  Serial.print("VL6180X manufactured on day "); Serial.print((datel & 0xF8) >> 3 ); Serial.print(", manufacturing phase ");  Serial.println(datel & 0x07); 
  uint8_t timeh = readByte(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_TIME_HI);  // Read IDENTIFICATION_TIME register for VL6180X
  uint8_t timel = readByte(VL6180X_ADDRESS, VL6180X_IDENTIFICATION_TIME_LO);  // Read IDENTIFICATION_TIME register for VL6180X
  uint16_t mtime = ((uint16_t)timeh << 8 | timel);
  Serial.print("VL6180X manufactured "); Serial.print(2*mtime/3600); Serial.println(" hours after midnight "); 
  delay(1000); 

  if (c == 0xB4) // VL6180X WHO_AM_I should always be 0xB4
  {  
    Serial.println("VL6180X is online...");
    initVL6180X();
    Serial.println("VL6180X is initialized...");
    getalsGain();
    uint8_t testALSGain = readByte(VL6180X_ADDRESS, VL6180X_SYSALS_ANALOGUE_GAIN);
    Serial.print("ALS Gain register is 0x"); Serial.println(testALSGain, HEX);

    Serial.print("VL6180X ALS Gain is "); Serial.println(realalsGain, 2);
  }
  else
  {
    Serial.print("Could not connect to VL6180X: 0x");
    Serial.println(c, HEX);
    while(1) ; // Loop forever if communication doesn't happen
  }
}


void loop()
{  
    uint8_t status = readByte(VL6180X_ADDRESS, VL6180X_RESULT_INTERRUPT_STATUS_GPIO);  // read new sample data ready status register
    if(verboseMode) {
    Serial.print("Status = "); Serial.println(status);
    
    // first check for damage or error
    if(status & 0x40) Serial.println("laser safety error!");
    if(status & 0x80) Serial.println("PLL1 or PLL2 error!");
    Serial.println("  ");
    }
    
    if( !(status & 0x40) && !(status & 0x80) ) {
    if(verboseMode) Serial.println("No errors...");
    
    
    if(VL6180XMode == contRangeMode) {
    // Check error status
    uint8_t error_status = readByte(VL6180X_ADDRESS, VL6180X_RESULT_RANGE_STATUS); 
    
    if(verboseMode) {
    if(error_status & 0x00) Serial.println("No errors...");
    if(error_status & 0x10) Serial.println("VCSEL Continuity Test error!");
    if(error_status & 0x20) Serial.println("VCSEL Watchdog Test error!");
    if(error_status & 0x30) Serial.println("VCSEL Watchdog error!");
    if(error_status & 0x40) Serial.println("PLL1 Lock error!");
    if(error_status & 0x50) Serial.println("PLL2 Lock error!");
    if(error_status & 0x60) Serial.println("Early Convergence Estimate error!");
    if(error_status & 0x70) Serial.println("Max Convergence error!");
    if(error_status & 0x80) Serial.println("No Target Ignore error!");
    if(error_status & 0xB0) Serial.println("Max Signal to Noise Ratio error!");
    if(error_status & 0xC0) Serial.println("Raw Ranging Algo Underflow error!");
    if(error_status & 0xD0) Serial.println("Raw Ranging Algo Overflow error!");
    if(error_status & 0xE0) Serial.println("Ranging Algo Underflow error!");
    if(error_status & 0xF0) Serial.println("Ranging Algo Overflow error!");
    Serial.println("  ");
    }
    
    // See if range new data is ready
    uint8_t range_status = status & 0x07;  // extract range status component
    if(verboseMode) {Serial.print("range_status = "); Serial.println(range_status);}
   
    while(range_status != 0x04) { // wait for new range measurement ready status
    status = readByte(VL6180X_ADDRESS, VL6180X_RESULT_INTERRUPT_STATUS_GPIO); 
    range_status = status & 0x07;
    delay(1);
    }
    
    // OK, range data is ready, get range
    uint8_t range = readByte(VL6180X_ADDRESS, VL6180X_RESULT_RANGE_VAL);
    Serial.print("Current range is "); Serial.print(range); Serial.println( " mm");  // print out range
    
    writeByte(VL6180X_ADDRESS, VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);  // clear all data ready status interrupts
    }
    
    
    if (VL6180XMode == contALSMode) {

    // Check error status
    uint8_t error_status = readByte(VL6180X_ADDRESS, VL6180X_RESULT_ALS_STATUS);  
    
    if(verboseMode) {
    if((error_status >> 4) & 0x00) Serial.println("No errors...");
    if((error_status >> 4) & 0x01) Serial.println("ALS  overflow error!");
    if((error_status >> 4) & 0x02) Serial.println("ALS underflow error!");
    }
    
    uint8_t als_status = (status & 0x38) >> 3;    // extract als status component
    if(verboseMode) {Serial.print("als_status = "); Serial.println(als_status);}
    
    while(als_status != 0x04) { // wait for new als measurement ready status
    status = readByte(VL6180X_ADDRESS, VL6180X_RESULT_INTERRUPT_STATUS_GPIO); 
    als_status = (status & 0x38) >> 3;
    delay(1);
    }

    uint8_t rawData[2] = {0, 0};
    readBytes(VL6180X_ADDRESS, VL6180X_RESULT_ALS_VAL, 2, &rawData[0]); // two-byte als data
    uint16_t alsraw = (uint16_t) (((uint16_t) rawData[0] << 8) | rawData[1]); //get 16-bit als raw value
    als = 0.32f * ((float) alsraw / realalsGain) * (100.0f/100.0f);  // convert to absolute lux
    if(verboseMode) {Serial.print("Current raw ambient light intensity is "); Serial.print(alsraw); Serial.println( " counts"); } // print out ambient light intensity in lux
    Serial.print("Current ambient light intensity is "); Serial.print(als, 1); Serial.println( " lux");  // print out ambient light intensity in lux

    writeByte(VL6180X_ADDRESS, VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);  // clear all data ready status interrupts
    }
 
    if (VL6180XMode == interLeaveMode) {
    // Check error status
    uint8_t error_status = readByte(VL6180X_ADDRESS, VL6180X_RESULT_ALS_STATUS);  
    if(verboseMode) {
    if((error_status >> 4) & 0x00) Serial.println("No errors...");
    if((error_status >> 4) & 0x01) Serial.println("ALS  overflow error!");
    if((error_status >> 4) & 0x02) Serial.println("ALS underflow error!");
    }
    
    uint8_t als_status = (status & 0x38) >> 3;    // extract als status component
    if(verboseMode) {Serial.print("als_status = "); Serial.println(als_status);}
    
    while(als_status != 0x04) { // wait for new als measurement ready status
    status = readByte(VL6180X_ADDRESS, VL6180X_RESULT_INTERRUPT_STATUS_GPIO); 
    als_status = (status & 0x38) >> 3;
    delay(1);
    }

    uint8_t rawData[2] = {0, 0};
    readBytes(VL6180X_ADDRESS, VL6180X_RESULT_ALS_VAL, 2, &rawData[0]); // two-byte als data
    uint16_t alsraw = (uint16_t) (((uint16_t) rawData[0] << 8) | rawData[1]); //get 16-bit als raw value
    als = 0.32f * ((float) alsraw / realalsGain) * (100.0f/100.0f);  // convert to absolute lux
    if(verboseMode) {Serial.print("Current raw ambient light intensity is "); Serial.print(alsraw); Serial.println( " counts"); }  // print out ambient light intensity in lux
    Serial.print("Current ambient light intensity is "); Serial.print(als, 1); Serial.println( " lux");  // print out ambient light intensity in lux
  
    // Check error status
    error_status = readByte(VL6180X_ADDRESS, VL6180X_RESULT_RANGE_STATUS);  
    if(verboseMode) {
    if(error_status & 0x00) Serial.println("No errors...");
    if(error_status & 0x10) Serial.println("VCSEL Continuity Test error!");
    if(error_status & 0x20) Serial.println("VCSEL Watchdog Test error!");
    if(error_status & 0x30) Serial.println("VCSEL Watchdog error!");
    if(error_status & 0x40) Serial.println("PLL1 Lock error!");
    if(error_status & 0x50) Serial.println("PLL2 Lock error!");
    if(error_status & 0x60) Serial.println("Early Convergence Estimate error!");
    if(error_status & 0x70) Serial.println("Max Convergence error!");
    if(error_status & 0x80) Serial.println("No Target Ignore error!");
    if(error_status & 0xB0) Serial.println("Max Signal to Noise Ratio error!");
    if(error_status & 0xC0) Serial.println("Raw Ranging Algo Underflow error!");
    if(error_status & 0xD0) Serial.println("Raw Ranging Algo Overflow error!");
    if(error_status & 0xE0) Serial.println("Ranging Algo Underflow error!");
    if(error_status & 0xF0) Serial.println("Ranging Algo Overflow error!");
    Serial.println("  ");
    }
    
    // See if range new data is ready
    uint8_t range_status = status & 0x07;  // extract range status component
    if(verboseMode) {Serial.print("range_status = "); Serial.println(range_status);}
   
    while(range_status != 0x04) { // wait for new range measurement ready status
    status = readByte(VL6180X_ADDRESS, VL6180X_RESULT_INTERRUPT_STATUS_GPIO); 
    range_status = status & 0x07;
    delay(1);
    }
    
    // OK, range data is ready, get range
    uint8_t range = readByte(VL6180X_ADDRESS, VL6180X_RESULT_RANGE_VAL);
    Serial.print("Current range is "); Serial.print(range); Serial.println( " mm");  // print out range
    
    writeByte(VL6180X_ADDRESS, VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);  // clear all data ready status interrupts
 
    }
        
    }
    
    digitalWrite(myLed, !digitalRead(myLed)); 
    delay(500);
}

//===================================================================================================================
//====== Set of useful function to access acceleration. gyroscope, magnetometer, and temperature data
//===================================================================================================================


void getalsGain() {
  switch (ALSGain)
  {
   // Possible ALS gains
    case alsGain20:
          realalsGain = 20.; // get actual ALS gain from nominal index
          break;
    case alsGain10:
          realalsGain = 10.32;  
          break;
    case alsGain5:
          realalsGain = 5.21;  
          break;
    case alsGain2_5:
          realalsGain = 2.60;
          break;
    case alsGain1_67:
          realalsGain = 1.72;  
          break;
    case alsGain1_25:
          realalsGain = 1.28;  
          break;
    case alsGain1_0:
          realalsGain = 1.01;  
          break;
    case alsGain40:
          realalsGain = 40.;  
          break;
  }
}


void initVL6180X() {
  uint8_t reset = readByte(VL6180X_ADDRESS, VL6180X_SYSTEM_FRESH_OUT_OF_RESET);  // read fresh_out_of_reset bit
  if(reset == 1) {  // if if fresh_out_of_reset bit set, then device has been freshly initialized
  
 // SR03 settings AN4545 24/27 DocID026571 Rev 19 SR03 settings
// http://www.st.com/st-web-ui/static/active/en/resource/technical/document/application_note/DM00122600.pdf
// Below are the recommended settings required to be loaded onto the VL6180X during the
// initialisation of the device (see Section 1.3).
// Mandatory : private registers
writeByte(VL6180X_ADDRESS, 0x0207, 0x01);
writeByte(VL6180X_ADDRESS, 0x0208, 0x01);
writeByte(VL6180X_ADDRESS, 0x0096, 0x00);
writeByte(VL6180X_ADDRESS, 0x0097, 0xFD);
writeByte(VL6180X_ADDRESS, 0x00e3, 0x00);
writeByte(VL6180X_ADDRESS, 0x00e4, 0x04);
writeByte(VL6180X_ADDRESS, 0x00e5, 0x02);
writeByte(VL6180X_ADDRESS, 0x00e6, 0x01);
writeByte(VL6180X_ADDRESS, 0x00e7, 0x03);
writeByte(VL6180X_ADDRESS, 0x00f5, 0x02);
writeByte(VL6180X_ADDRESS, 0x00d9, 0x05);
writeByte(VL6180X_ADDRESS, 0x00db, 0xce);
writeByte(VL6180X_ADDRESS, 0x00dc, 0x03);
writeByte(VL6180X_ADDRESS, 0x00dd, 0xf8);
writeByte(VL6180X_ADDRESS, 0x009f, 0x00);
writeByte(VL6180X_ADDRESS, 0x00a3, 0x3c);
writeByte(VL6180X_ADDRESS, 0x00b7, 0x00);
writeByte(VL6180X_ADDRESS, 0x00bb, 0x3c);
writeByte(VL6180X_ADDRESS, 0x00b2, 0x09);
writeByte(VL6180X_ADDRESS, 0x00ca, 0x09);
writeByte(VL6180X_ADDRESS, 0x0198, 0x01);
writeByte(VL6180X_ADDRESS, 0x01b0, 0x17);
writeByte(VL6180X_ADDRESS, 0x01ad, 0x00);
writeByte(VL6180X_ADDRESS, 0x00ff, 0x05);
writeByte(VL6180X_ADDRESS, 0x0100, 0x05);
writeByte(VL6180X_ADDRESS, 0x0199, 0x05);
writeByte(VL6180X_ADDRESS, 0x01a6, 0x1b);
writeByte(VL6180X_ADDRESS, 0x01ac, 0x3e);
writeByte(VL6180X_ADDRESS, 0x01a7, 0x1f);
writeByte(VL6180X_ADDRESS, 0x0030, 0x00);

// onfigure range measurement for low power
// Specify range measurement interval in units of 10 ms from 0 (= 10 ms) - 254 (= 2.55 s)
writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD, 0x09); // 100 ms interval in steps of 10 ms
writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_VHV_REPEAT_RATE, 0xFF); // sets number of range measurements after which autocalibrate is performed
writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_VHV_RECALIBRATE, 0x01); // perform temperature calibration of the ranging sensor

// Set Early Convergence Estimate for lower power consumption
writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME, 0x32); // set max convergence time to 50 ms (steps of 1 ms)
writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_RANGE_CHECK_ENABLES, 0x10 | 0x01);  // enable (0x01) early convergence estimate
// This ECE is calculated as follows:
// [(1 - % below threshold) x 0.5 x 15630]/ range max convergence time
// This is ~123 ms for 50 ms max convergence time and 80% below threshold
// This is a sixteen bit (2 byte) register with the first byte MSByte and the second LSByte
writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x00); // set early convergence estimate to 5%
writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE + 1, 0x7B); // set early convergence estimate to 5%

// Configure ALS
writeByte(VL6180X_ADDRESS, VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x0A);   // set to 100 ms
// Following is a 16-bit register with the first MSByte reserved
writeByte(VL6180X_ADDRESS, VL6180X_SYSALS_INTEGRATION_PERIOD + 1, 0x63);        // set ALS integration time to 100 ms in steps of 1 ms

// The internal readout averaging sample period can be adjusted from 0 to 255. Increasing the sampling 
// period decreases noise but also reduces the effective max convergence time and increases power consumption:
// Effective max convergence time = max convergence time - readout averaging period (see
// Section 2.5: Range timing). Each unit sample period corresponds to around 64.5 μs additional
// processing time. The recommended setting is 48 which equates to around 4.3 ms
   writeByte(VL6180X_ADDRESS, VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD, 0x30);  // compromise between low noise and increased execution time
// Gain can be 0 = 20, 1 = 10, 2 = 5, 3 = 2.5, 4 = 1.67, 5 = 1.25, 6 = 1.0 and 7 = 40
// These are value possible for the lower nibble. The upper nibble must be 4
   writeByte(VL6180X_ADDRESS, VL6180X_SYSALS_ANALOGUE_GAIN, 0x40 | ALSGain);   // Sets light and dark gain (don't change upper nibble)
// Scalar (1 - 32, bits 4:0) to multiply raw ALS count for additonal gain, if necessary
   writeByte(VL6180X_ADDRESS, VL6180X_FIRMWARE_RESULT_SCALER, 0x01);

// Configure the interrupts
   writeByte(VL6180X_ADDRESS, VL6180X_SYSTEM_MODE_GPIO0, 0x00);                // set up GPIO 0 (set to high impedence for now)
   writeByte(VL6180X_ADDRESS, VL6180X_SYSTEM_MODE_GPIO1, 0x00);                // set up GPIO 1 (set to high impedence for now)
   writeByte(VL6180X_ADDRESS, VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x24);     // enable sample ready interrupt


// enable continuous range mode
if(VL6180XMode == contRangeMode) {
  writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_START, 0x03);  // start auto range mode
  
}
// enable continuous ALS mode
if(VL6180XMode == contALSMode) {
// Configure ALS
  writeByte(VL6180X_ADDRESS, VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x32);   // set to 100 ms
// Following is a 16-bit register with the first MSByte reserved
  writeByte(VL6180X_ADDRESS, VL6180X_SYSALS_INTEGRATION_PERIOD+1, 0x32);        // set ALS integration time to 50 ms in steps of 1 ms
  writeByte(VL6180X_ADDRESS, VL6180X_SYSALS_START, 0x03);                     // start auto range mode
}

if(VL6180XMode == interLeaveMode) {
// Configure ALS for interleaved mode at 10 Hz
  writeByte(VL6180X_ADDRESS, VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x0A);       // set to 100 ms
// Following is a 16-bit register with the first MSByte reserved
  writeByte(VL6180X_ADDRESS, VL6180X_SYSALS_INTEGRATION_PERIOD+1, 0x32);          // set ALS integration time to 50 ms in steps of 1 ms
  writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME, 0x1E);        // set max convergence time to 30 ms
  writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_RANGE_CHECK_ENABLES, 0x10 | 0x01);  // enable (0x01) early convergence estimate
// This ECE is calculated as follows:
// [(1 - % below threshold) x 0.5 x 15630]/ range max convergence time
// This is ~72 ms for 30 ms max convergence time and 80% below threshold
// This is a sixteen bit (2 byte) register with the first byte MSByte and the second LSByte
  writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x00); // set early convergence estimate to 5%
  writeByte(VL6180X_ADDRESS, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE + 1, 0x48); // set early convergence estimate to 5%
  writeByte(VL6180X_ADDRESS, VL6180X_INTERLEAVED_MODE_ENABLE, 0x01);  // eanble interleave mode
  writeByte(VL6180X_ADDRESS, VL6180X_SYSALS_START, 0x03);  // start continuous als measurement mode
// range read automatically performed immediately after each ALS measurement

// Clear reset bit
  writeByte(VL6180X_ADDRESS, VL6180X_SYSTEM_FRESH_OUT_OF_RESET, 0x00);        // reset fresh_out_of_reset bit to zero 

}

}

}


// I2C read/write functions for the VL6180X sensor
// VL6180X has 16-bit register addresses, so the MSB of the register is sent first, then the LSB

        void writeByte(uint8_t address, uint16_t subAddress, uint8_t data)
{
  Wire.beginTransmission(address);      // Initialize the Tx buffer
  Wire.write((subAddress >> 8) & 0xFF); // Put MSB of 16-bit slave register address in Tx buffer
  Wire.write(subAddress & 0xFF);        // Put LSB of 16-bit slave register address in Tx buffer
  Wire.write(data);                     // Put data in Tx buffer
  Wire.endTransmission();               // Send the Tx buffer
}

        uint8_t readByte(uint8_t address, uint16_t subAddress)
{
  uint8_t data; // `data` will store the register data   
  Wire.beginTransmission(address);         // Initialize the Tx buffer
  Wire.write((subAddress >> 8) & 0xFF);    // Put MSB of 16-bit slave register address in Tx buffer
  Wire.write(subAddress & 0xFF);           // Put LSB of 16-bit slave register address in Tx buffer
  Wire.endTransmission(I2C_NOSTOP);        // Send the Tx buffer, but send a restart to keep connection alive
//  Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
//  Wire.requestFrom(address, 1);  // Read one byte from slave register address 
  Wire.requestFrom(address, (size_t) 1);   // Read one byte from slave register address 
  data = Wire.read();                      // Fill Rx buffer with result
  return data;                             // Return data read from slave register
}

        void readBytes(uint8_t address, uint16_t subAddress, uint8_t count, uint8_t * dest)
{  
  Wire.beginTransmission(address);      // Initialize the Tx buffer
  Wire.write((subAddress >> 8) & 0xFF); // Put MSB of 16-bit slave register address in Tx buffer
  Wire.write(subAddress & 0xFF);        // Put LSB of 16-bit slave register address in Tx buffer
  Wire.endTransmission(I2C_NOSTOP);     // Send the Tx buffer, but send a restart to keep connection alive
//  Wire.endTransmission(false);          // Send the Tx buffer, but send a restart to keep connection alive
  uint8_t i = 0;
//        Wire.requestFrom(address, count);   // Read bytes from slave register address 
        Wire.requestFrom(address, (size_t) count);  // Read bytes from slave register address 
  while (Wire.available()) {
        dest[i++] = Wire.read(); }            // Put read results in the Rx buffer
}


