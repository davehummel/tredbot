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

uint32_t pwmH = 200;
uint32_t pwmL = 0;
uint32_t intH = 500;
uint32_t intL = 500;
uint32_t dirInt = 500;

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
