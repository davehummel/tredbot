void setup() {
  Serial1.begin(3500000);
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  delay(1000);
  Serial.println("Begin!");
  digitalWrite(13,HIGH);
}

bool flip = true;
  char out4[10];
void loop() {
  digitalWrite(13,flip);
//
//  uint32_t now = millis();
//  Serial1.write(5);
//  Serial1.write(1);
//  
//  byte b[4];
//  IntegerToBytes(now, b);
//  for (int i=0; i<4; ++i) {
//      Serial1.write(b[i]);
//  }

  while(!Serial1.available());
  while(Serial1.available()){
    Serial.print("|");
    Serial.print((int)Serial1.read());
  }
  flip = !flip;
}


void IntegerToBytes(long val, byte b[4]) {
  b[3] = (byte )((val >> 24) & 0xff);
  b[2] = (byte )((val >> 16) & 0xff);
  b[1] = (byte )((val >> 8) & 0xff);
  b[0] = (byte )(val & 0xff);
}

