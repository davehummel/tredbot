#define THRESH 1500
#define SHORTPULSE 10

#define COLORCOUNT 10
int colors[COLORCOUNT * 3] = {255,255,255 ,0,255,0, 255,20,0, 230,255,20 ,200,0,255, 100,200,255, 0,200,255, 0,255,10, 0,0,0, 255,100,50};

int topColorSel = 5;
int botColorSel = 4;
int cycleSel = 20;

double pulse = 1;

void setup() {
  pinMode(23, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(20, OUTPUT);
  pinMode(13, OUTPUT);
 
}

void loop() {


  long time = millis();


      digitalWrite(13,false);


  pulse = cycleSel * SHORTPULSE;


  double val = (sin(time/pulse)+1.0l)/2.0l;

  int r = val*colors[topColorSel*3] + (1.0l-val)*colors[botColorSel*3];
    int g = val*colors[topColorSel*3+1] + (1.0l-val)*colors[botColorSel*3+1];
      int b = val*colors[topColorSel*3+2] + (1.0l-val)*colors[botColorSel*3+2];

   setColor (r,g,b);
}

void setColor (uint8_t r, uint8_t g, uint8_t b){
  analogWrite(20,r);
  analogWrite(22,g);
  analogWrite(23,b);
}

