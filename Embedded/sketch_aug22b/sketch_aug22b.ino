void setup() {
 Serial1.begin(460800);
 pinMode(13,OUTPUT);
}

  boolean flip = HIGH;
void loop() {
    long time =0;
    
    digitalWrite(13,flip);
    while(!Serial1.available()){}
    while (Serial1.available()){
      Serial1.read();
    }
    
    Serial1.print("Cats");
    flip = ! flip;
}
