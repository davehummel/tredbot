void setup() {
  Serial.begin(115200);
  Serial1.begin(3500000);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

    delay(2000);
      Serial.println("Starting");
      

}

  boolean flip = HIGH;
  boolean printC1 = false;
  boolean printC2 = false;
  boolean doTime = false;
 
void loop() {
 long time;
    Serial1.print("X101|");
    Serial1.print(time = millis() );
    Serial1.println("|START!!");
  
  flip = !flip;
  delay(1000);
}

/*
void loop() {
 long time;
    Serial1.print("X101|");
    Serial1.print(time = millis() );
    Serial1.println("|START!!");
    boolean again = true;
    while(again){
      while (Serial1.available()){
     
        char in = Serial1.read();
        if (in == 'T'){
          doTime = true;
        }
        if (in == '\n'){
            Serial.println(millis()-time);
            Serial1.print("X102|");
            Serial1.print( millis() );
            Serial1.println("|END!!");
            again = false;
            break;
        }
      }
  }
  
  if (doTime){
    doTime = false;
    Serial1.print("T1|");
    Serial1.print(millis());
    Serial1.println("|");
  }
  
  flip = !flip;
  
}*/
