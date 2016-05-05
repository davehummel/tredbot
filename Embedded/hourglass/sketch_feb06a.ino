void setup() {
   pinMode(13,OUTPUT);
   pinMode(23,OUTPUT);
       analogWriteFrequency(23, 375000); 
   pinMode(22,OUTPUT);
   digitalWrite(22,LOW);
   analogWriteResolution(12);
   Serial.begin(115200);
}

uint16_t pwmH = 2000;
uint16_t pwmL = 500;
uint16_t intH = 195;
uint16_t intL = 5;
uint16_t dirInt = 1;

void loop() {
  
//  if (Serial.available() == 6){
//    char buffer[6];
//    buffer[5] = '\0';
//    Serial.readBytes(buffer,6);
//    
//    Serial.println(buffer);
//    if (buffer[0] == 'a' || buffer[0] == 'A'){
//        skip =  (buffer[1]-'0')*1000 + (buffer[2]-'0')*100 + (buffer[3]-'0')*10 + (buffer[4]-'0');
//    } else if (buffer[0] == 'd' || buffer[0] == 'D'){
//        dirInt =  (buffer[1]-'0')*1000 + (buffer[2]-'0')*100 + (buffer[3]-'0')*10 + (buffer[4]-'0');
//    } else {
//       pwm =  (buffer[1]-'0')*1000 + (buffer[2]-'0')*100 + (buffer[3]-'0')*10 + (buffer[4]-'0');
//    }
//    
//    Serial.print("PWM=");
//    Serial.println((int)pwm);
//    
//    Serial.print("Active Dur=");
//    Serial.println((int)skip);
//    
//    Serial.print("Dir Switch interval=");
//    Serial.println((int)dirInt);
//
//  }
  uint32_t time = millis();
  if (dirInt == 0 )
    digitalWrite(22,false);
  else
    digitalWrite(22, ((time/dirInt)%2));
    
  time = time%(intH+intL);
  if (time > intH){
        digitalWrite(13,LOW);
    analogWrite(23,pwmL);
  }else{
        digitalWrite(13,HIGH);
    analogWrite(23,pwmH);
  }
  
 
 // Serial.println(analogRead(A0));
  
}
