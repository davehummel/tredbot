/***************************************************************************
  This is a library for the LSM9DS0 Accelerometer and magnentometer/compass

  Designed specifically to work with the Adafruit LSM9DS0 Breakouts

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/
#include <Adafruit_LSM9DS0.h>

/***************************************************************************
 CONSTRUCTOR
 ***************************************************************************/
// default
Adafruit_LSM9DS0::Adafruit_LSM9DS0( int32_t sensorID ) {
  _i2c = true;
  _lsm9dso_sensorid_accel = sensorID + 1;
  _lsm9dso_sensorid_mag = sensorID + 2;
  _lsm9dso_sensorid_gyro = sensorID + 3;
  _lsm9dso_sensorid_temp = sensorID + 4;
}


bool Adafruit_LSM9DS0::begin()
{
  if (_i2c) {
    Wire.begin();
  }

  uint8_t id = read8(XMTYPE, LSM9DS0_REGISTER_WHO_AM_I_XM);
  //Serial.print ("XM whoami: 0x");
  // Serial.println(id, HEX);
  if (id != LSM9DS0_XM_ID) 
    return false;

  id = read8(GYROTYPE, LSM9DS0_REGISTER_WHO_AM_I_G);
  // Serial.print ("G whoami: 0x");
  // Serial.println(id, HEX);
  if (id != LSM9DS0_G_ID) 
    return false;

  // Enable the accelerometer continous
  write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG1_XM, 0b00001111); // 100hz XYZ
  write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG5_XM, 0b11110000);
  // enable mag continuous
  write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG7_XM, 0b00000000);
  // enable gyro continuous
  write8(GYROTYPE, LSM9DS0_REGISTER_CTRL_REG1_G, 0x0F); // on XYZ
  // enable the temperature sensor (output rate same as the mag sensor)
  uint8_t tempReg = read8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG5_XM);
  write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG5_XM, tempReg | (1<<7));
  
  /*
  for (uint8_t i=0; i<0x30; i++) {
    Serial.print("$"); Serial.print(i, HEX);
    Serial.print(" = 0x"); 
    Serial.println(read8(XMTYPE, i), HEX);
  }
  */

  // Set default ranges for the various sensors  
  setupAccel(LSM9DS0_ACCELRANGE_2G);
  setupMag(LSM9DS0_MAGGAIN_2GAUSS);
  setupGyro(LSM9DS0_GYROSCALE_245DPS);

  return true;
}

/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/
void Adafruit_LSM9DS0::read()
{
  /* Read all the sensors. */
  readAccel();
  readMag();
  readGyro();
  readTemp();
}

void Adafruit_LSM9DS0::readAccel() {
  // Read the accelerometer
  byte buffer[6];
  readBuffer(XMTYPE, 
       0x80 | LSM9DS0_REGISTER_OUT_X_L_A, 
       6, buffer);
  
  uint8_t xlo = buffer[0];
  int16_t xhi = buffer[1];
  uint8_t ylo = buffer[2];
  int16_t yhi = buffer[3];
  uint8_t zlo = buffer[4];
  int16_t zhi = buffer[5];
  
  // Shift values to create properly formed integer (low byte first)
  xhi <<= 8; xhi |= xlo;
  yhi <<= 8; yhi |= ylo;
  zhi <<= 8; zhi |= zlo;
  accelData.x = xhi;
  accelData.y = yhi;
  accelData.z = zhi;
}

void Adafruit_LSM9DS0::readMag() {
  // Read the magnetometer
  byte buffer[6];
  readBuffer(XMTYPE, 
       0x80 | LSM9DS0_REGISTER_OUT_X_L_M, 
       6, buffer);
  
  uint8_t xlo = buffer[0];
  int16_t xhi = buffer[1];
  uint8_t ylo = buffer[2];
  int16_t yhi = buffer[3];
  uint8_t zlo = buffer[4];
  int16_t zhi = buffer[5];
  
  // Shift values to create properly formed integer (low byte first)
  xhi <<= 8; xhi |= xlo;
  yhi <<= 8; yhi |= ylo;
  zhi <<= 8; zhi |= zlo;
  magData.x = xhi;
  magData.y = yhi;
  magData.z = zhi;
}

void Adafruit_LSM9DS0::readGyro() {
  // Read gyro
  byte buffer[6];
  readBuffer(GYROTYPE, 
       0x80 | LSM9DS0_REGISTER_OUT_X_L_G, 
       6, buffer);
  
  uint8_t xlo = buffer[0];
  int16_t xhi = buffer[1];
  uint8_t ylo = buffer[2];
  int16_t yhi = buffer[3];
  uint8_t zlo = buffer[4];
  int16_t zhi = buffer[5];
  
  // Shift values to create properly formed integer (low byte first)
  xhi <<= 8; xhi |= xlo;
  yhi <<= 8; yhi |= ylo;
  zhi <<= 8; zhi |= zlo;
  
  gyroData.x = xhi;
  gyroData.y = yhi;
  gyroData.z = zhi;
}

void Adafruit_LSM9DS0::readTemp() {
  // Read temp sensor
  byte buffer[2];
  readBuffer(XMTYPE, 
       0x80 | LSM9DS0_REGISTER_TEMP_OUT_L_XM, 
       2, buffer);
  uint8_t xlo = buffer[0];
  int16_t xhi = buffer[1];

  xhi <<= 8; xhi |= xlo;
  
  // Shift values to create properly formed integer (low byte first)
  temperature = xhi;
}

void Adafruit_LSM9DS0::setupAccel ( lsm9ds0AccelRange_t range )
{
  uint8_t reg = read8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG2_XM);
  reg &= ~(0b00111000);
  reg |= range;
  write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG2_XM, reg );
  
 
}

void Adafruit_LSM9DS0::setupMag ( lsm9ds0MagGain_t gain )
{
  uint8_t reg = read8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG6_XM);
  reg &= ~(0b01100000);
  reg |= gain;
  write8(XMTYPE, LSM9DS0_REGISTER_CTRL_REG6_XM, reg );

  
}

void Adafruit_LSM9DS0::setupGyro ( lsm9ds0GyroScale_t scale )
{
  uint8_t reg = read8(GYROTYPE, LSM9DS0_REGISTER_CTRL_REG4_G);
  reg = 0b0000,0000;
  write8(GYROTYPE, LSM9DS0_REGISTER_CTRL_REG4_G, reg );
  reg = 0x88;
  write8(GYROTYPE, LSM9DS0_REGISTER_CTRL_REG3_G, reg );
  reg = 0x00;
  write8(GYROTYPE, LSM9DS0_REGISTER_CTRL_REG2_G, reg );

}

void Adafruit_LSM9DS0::highPassOn(bool on)
{
    write8(GYROTYPE, LSM9DS0_REGISTER_CTRL_REG4_G, on? 0b00010000:0b00000000 );
}

void Adafruit_LSM9DS0::printRegisters(){
    Serial.println("Gyro Registers");
    for (int i = 0 ; i < 5 ; i++){
      uint8_t reg = read8(GYROTYPE, 0x20 + i);
      Serial.write("Reg ");
      Serial.print(i+1);
      Serial.write(" : ");
      Serial.println(reg,BIN);
    }
}

void Adafruit_LSM9DS0::selfTest(bool on){
   
}




/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/
void Adafruit_LSM9DS0::write8(boolean type, byte reg, byte value)
{
  byte address, _cs;

  if (type == GYROTYPE) {
    address = LSM9DS0_ADDRESS_GYRO;
    _cs = _csg;
  } else {
    address = LSM9DS0_ADDRESS_ACCELMAG;
    _cs = _csxm;
  }
  if (_i2c) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
  }
}

byte Adafruit_LSM9DS0::read8(boolean type, byte reg)
{
  uint8_t value;

  readBuffer(type, reg, 1, &value);

  return value;
}

byte Adafruit_LSM9DS0::readBuffer(boolean type, byte reg, byte len, uint8_t *buffer)
{
  byte address, _cs;

  if (type == GYROTYPE) {
    address = LSM9DS0_ADDRESS_GYRO;
    _cs = _csg;
  } else {
    address = LSM9DS0_ADDRESS_ACCELMAG;
    _cs = _csxm;
  }

  if (_i2c) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(address, (byte)len);

    // Wait around until enough data is available
    while (Wire.available() < len);

    for (uint8_t i=0; i<len; i++) {
      buffer[i] = Wire.read();
    }
    Wire.endTransmission();
  } 
  return len;
}

