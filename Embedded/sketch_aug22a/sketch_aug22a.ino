void setup() {
  Serial.begin(115200);
  Serial1.begin(460800);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

    delay(2000);
      Serial.println("Starting");
}

  boolean flip = HIGH;
void loop() {
  long time = millis();

  digitalWrite(13,flip);
  Serial1.print("C B 10 HI\n");
  Serial.print(">C B 10 HI");
  
  while (true){
    if (Serial1.available()){
      char in = Serial1.read();
      Serial.write(in);
      if (in=='\n')
        break;
    }
  }
  Serial.println();
  Serial.println(millis()-time);
  flip = !flip;
  delay(1000);
}
