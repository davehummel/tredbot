void setup() {
  
  pinMode(13, OUTPUT);
  pinMode(3, OUTPUT); 
 
}

void loop() {
delay(1);
tone(3,(millis()%10000)/3);
  
}
