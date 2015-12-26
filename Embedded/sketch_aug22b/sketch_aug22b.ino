void setup() {
 Serial1.begin(3500000);
 Serial.begin(115200);
 delay(1000);
 Serial.println("Starting !:");
 pinMode(13,OUTPUT);
}

  boolean flip = HIGH;
  char pong[] = "p0ng\n";
void loop() {
     for (int i = 0 ; i < 1000 ; i++){
    long time =0;
    digitalWrite(13,flip);
    char input[255];
    getLine(input);
    time = millis();
    Serial1.write(pong);
    getLine(input);
    time = millis() - time;
    Serial.print("Time=");
    Serial.print(time);
    Serial.print(" Input=");
    Serial.println(input);
    flip = ! flip;
    }
    
    Serial.println("Done!");
    delay(10000);
}

void getLine(char* line){
  uint8_t lineIndex = 0;
  while (true){
    if (Serial1.available()){
      char in = Serial1.read();
      Serial.print(in);
      if (in == '\n'||in == '\r'){
        line[lineIndex] = '\0';
        return;
      }
      line[lineIndex] = in;
      lineIndex++;
    }
  }
}
