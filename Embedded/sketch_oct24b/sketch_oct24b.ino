

void setup() {
  Serial.begin(115200);
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);
  pinMode(14,INPUT);
  pinMode(23,OUTPUT);
  digitalWrite(23,LOW);
}

uint8_t soft = 0;
void loop() {
 
   while(true){
        delay(1);
	int16_t reading = analogRead(14);
        Serial.println(reading);
        bool blast = reading < 500 && reading >50;
        soft = soft + random(-4,5);
        if (soft > 32)
           soft = 32;
        if (blast)
          analogWrite(23,soft*8);
        else
          analogWrite(23,soft/2);
   }
}
