void setup() {
  Serial1.begin(3500000);
  Serial.begin(115200);

}

void loop() {
  int chars = Serial1.available();
 
  if (chars>0){
     byte val = Serial1.read();
     Serial.write(val); 
     Serial1.write(val); 
     if (val == '\n' || val == '\r'){
       Serial.println(millis()); 
     }
  }

}
