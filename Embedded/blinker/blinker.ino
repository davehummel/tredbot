void setup() {
  pinMode(0,OUTPUT);
  pinMode(1,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(13,OUTPUT);
}

void loop() {
  digitalWrite(0,false);
  digitalWrite(1,false);
  digitalWrite(2,false);
  digitalWrite(13,false);
  delay(1000);
  digitalWrite(13,true);
  digitalWrite(0,true);
  delay(1000);
  digitalWrite(1,true);
  delay(1000);
  digitalWrite(2,true);
  delay(1000);
}
