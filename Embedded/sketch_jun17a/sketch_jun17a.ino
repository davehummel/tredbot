
 void setup() {
   Serial.begin(115200);
 }

 
 void loop()                     
 {
   Serial.println(touchRead(A1));
   delay(100);
 }
