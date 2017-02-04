#define MASK_2048_CHANID 0x7800
#define MASK_2048_SXPOS 0x07FF
#define MASK_2048_PHASE 0x8000

void setup() {
  pinMode(13,OUTPUT);
    pinMode(10,OUTPUT);
//  digitalWrite(23,LOW);
//    pinMode(0,OUTPUT);
//  digitalWrite(0,LOW);
//  delay(2000);
//  digitalWrite(23,HIGH);
//  delayMicroseconds(116);
//  for (int i = 0 ; i < 4  ; i++){
//    digitalWrite(0,HIGH);
// delayMicroseconds(116);
//    digitalWrite(0,LOW);
// delayMicroseconds(116);
//  }
// delay(10000);
  Serial1.begin(115200);

  Serial.begin(115200);
}
union REMOTE_FRAME{
  struct {
  uint16_t fades;
  uint16_t servo[7];
  } data;
  uint8_t raw[16];
};

boolean parseServo(uint16_t input, uint8_t &channel, uint16_t &value){
  channel = (input&MASK_2048_CHANID)>>11;
  value = (input&MASK_2048_SXPOS);
  return (input&MASK_2048_PHASE)!=0;
}

unsigned long time;
int inData[16];
int inByte;
int xx = 0;
void loop() {

// while(Serial1.available() >= 16)
//  {
//      if (Serial1.read()!=0){
//        Serial.print("E");
//        continue;
//      }
//      if (Serial1.read()!=0){
//        Serial.print("E");
//        continue;
//      }

//
//    for(xx = 0; xx < 15; xx++)
//    {
//      inByte = Serial1.read();
//      inData[xx] = inByte;
//      Serial.print(inByte);
//      Serial.print(" ");
//    }
//    Serial.println();
//  }

   while(Serial1.available() >= 16)
    {
      REMOTE_FRAME frame;
      for (uint8_t i = 0 ;i <8; i ++ ){
        uint8_t temp[2];

        Serial1.readBytes(temp ,2);
       frame.raw[i*2+0] =temp[1];
       frame.raw[i*2+1] = temp[0];

      }
      Serial.print(frame.data.fades);
            Serial.print(">");
      uint8_t channel=0;
      uint16_t value=0;
      for (uint8_t x = 0 ; x<7 ; x++){
              parseServo(frame.data.servo[x],channel,value);
              if (channel == 0)
                analogWrite(10,value/18);
              if (channel == 4)
                digitalWrite(13,value>500);
             Serial.print(channel);
                    Serial.print("=");
                    Serial.print(value);
      Serial.print("  ");
      }
    Serial.println();
    }

}
