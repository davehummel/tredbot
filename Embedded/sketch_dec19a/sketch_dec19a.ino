void setup() {
 pinMode(30,OUTPUT);
 pinMode(13,OUTPUT);
}

void loop() {
  digitalWrite(30,HIGH);
    digitalWrite(13,HIGH);
  delay(10000);
  digitalWrite(30,LOW);
    digitalWrite(13,LOW);
  delay(10000);
}
