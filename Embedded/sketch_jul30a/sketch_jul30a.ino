void setup() {
    pinMode(23,INPUT);
  Serial.begin(115200);
}

void loop() {
  Serial.println(analogRead(23));
  delay(1000);
}
