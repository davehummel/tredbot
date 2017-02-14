void setup() {
  Serial.begin(115200);
pinMode(23,INPUT);
pinMode(22,INPUT);
}

void loop() {
  Serial.print(analogRead(23));
    Serial.print(' ');
    Serial.println(analogRead(22));
  delay(1000);
}
