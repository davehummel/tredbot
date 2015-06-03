void setup() {
  // put your setup code here, to run once:
  pinMode(12,OUTPUT);
  digitalWrite(12,LOW);
}

void loop() {
  delay(5000);
  digitalWrite(12,HIGH);
  delay(5000);
  digitalWrite(12,LOW);
}
