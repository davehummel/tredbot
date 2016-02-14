void setup() {
  Serial1.begin(115200);
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  delay(1000);
  Serial1.println("Begin!");
  digitalWrite(13,HIGH);
}

bool flip = true;
  char out4[10];
void loop() {
  digitalWrite(13,flip);
  while (Serial.available()){
    Serial1.write( Serial.read());
  };
  while (Serial1.available()){
    Serial.write( Serial1.read());
  }
  delay(1000);
  flip = ! flip;
}


void IntegerToBytes(long val, byte b[4]) {
  b[3] = (byte )((val >> 24) & 0xff);
  b[2] = (byte )((val >> 16) & 0xff);
  b[1] = (byte )((val >> 8) & 0xff);
  b[0] = (byte )(val & 0xff);
}

