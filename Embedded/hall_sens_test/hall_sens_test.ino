void setup() {
  pinMode(A0,INPUT);

}

void loop() {
  delay(100);
  Serial.println(analogRead(A0));
}
