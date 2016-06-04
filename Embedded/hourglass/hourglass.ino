#define DIRPIN 22
#define PWMPIN 23
#define LEDPIN 13


void setup() {
   pinMode(LEDPIN,OUTPUT);
   pinMode(PWMPIN,OUTPUT);
       analogWriteResolution(8);
       analogWriteFrequency(PWMPIN, 187500); 
   pinMode(DIRPIN,OUTPUT);
   digitalWrite(DIRPIN,LOW);
}

uint32_t pwmH = 90;
uint32_t pwmL = 40;
uint32_t intH = 9000;
uint32_t intL = 1000;
uint32_t dirInt = 1000;

elapsedMicros time;

void loop() {
  
  if (dirInt == 0 )
    digitalWrite(DIRPIN,false);
  else
    digitalWrite(DIRPIN, ((time/dirInt)%2));
    
  uint32_t calc = time%(intH+intL);
  if (calc > intH){
        digitalWrite(LEDPIN,LOW);
    analogWrite(23,pwmL);
  }else{
        digitalWrite(LEDPIN,HIGH);
    analogWrite(23,pwmH);
  }
  
}
