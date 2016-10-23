#define THRESH 1000
#define SHORTPULSE 10

#define COLORCOUNT 10
int colors[COLORCOUNT * 3] = {255,255,255 ,0,255,0, 255,20,0, 230,255,20 ,200,0,255, 100,200,255, 0,200,255, 0,255,10, 0,0,0, 255,100,50};

int topColorSel = 0;
int botColorSel = 1;
int cycleSel = 2;

double pulse = 1;

void setup() {
  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(21, OUTPUT);
  pinMode(13, OUTPUT);
 
}

void loop() {
  bool touch0 = touchRead(0)>THRESH;
  bool touch1 = touchRead(1)>THRESH;
  bool touch2 = touchRead(19)>THRESH;


  if (touch0){
    digitalWrite(13,true);
    topColorSel++;
    if (topColorSel >= COLORCOUNT)
      topColorSel = 0;
     delay (200);
  }

  if (touch1){
    digitalWrite(13,true);
    botColorSel++;
    if (botColorSel >= COLORCOUNT)
      botColorSel = 0;
     delay (200);
  }

  long time = millis();

  if (touch2 && (time%200 == 0)){
    cycleSel = cycleSel*1.5;
    if (cycleSel >= 2000){
      cycleSel = 2;
      digitalWrite(13,true);   
      delay(500);    
      digitalWrite(13,false);
    };
  }

      digitalWrite(13,false);


  pulse = cycleSel * SHORTPULSE;


  double val = (sin(time/pulse)+1.0l)/2.0l;

  int r = val*colors[topColorSel*3] + (1.0l-val)*colors[botColorSel*3];
    int g = val*colors[topColorSel*3+1] + (1.0l-val)*colors[botColorSel*3+1];
      int b = val*colors[topColorSel*3+2] + (1.0l-val)*colors[botColorSel*3+2];

   setColor (r,g,b);
}

void setColor (uint8_t r, uint8_t g, uint8_t b){
  analogWrite(21,r);
  analogWrite(22,g);
  analogWrite(23,b);
}

