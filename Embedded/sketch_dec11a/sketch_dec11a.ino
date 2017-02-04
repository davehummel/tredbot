    #include <FastLED.h>
    #define NUM_LEDS 5
    #define DATA_PIN 6
    #define BATLVL_PIN 9
    #define BTN_PIN 10
    #define SND_PIN 11
    
    #define PWRHOLD_PIN 5
    #define BTN_THRESH 1022
    
    enum class LightMode { Normal=0, Bright=1, Red=2, /*Move,*/ Power=3, Time=4, Off=5 };  
    CRGB leds[NUM_LEDS];
    uint16_t timeInSec = 0;
    LightMode lightMode = LightMode::Normal;
    LightMode prevMode = LightMode::Off;
    float powerLvl=0;

    uint16_t offsetTime = 0;
    uint16_t pauseStartTime = 0;
    bool isPaused = false;

    
    
void setup() {
  pinMode(13,OUTPUT);
  pinMode(PWRHOLD_PIN,OUTPUT);
  pinMode(BATLVL_PIN,INPUT);
  pinMode(BTN_PIN,INPUT);
  pinMode(SND_PIN,OUTPUT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  delay(1);
  digitalWrite (PWRHOLD_PIN,HIGH);
  digitalWrite (13,HIGH);
  leds[0] = CRGB::Black; 
  leds[1] = CRGB::Black; 
  leds[2] = CRGB::Black; 
  leds[3] = CRGB::Black; 
  leds[4] = CRGB::Black; 
  FastLED.show(); 
  delay(10);
   while (analogRead(BTN_PIN)>=BTN_THRESH){
    delay(1); // Waiting for the user to let go of the button on startup
   }
}

void loop() {
   digitalWrite (13,HIGH);
   timeInSec = millis()/500;
   if (timeInSec/60 > 30){
        digitalWrite (PWRHOLD_PIN,LOW); // shut down in 30 minutes
        delay(10000);
   }
   timeInSec = timeInSec - offsetTime;
   testPwr();
   testButton();
   doLights();
   doSound();
   delay(50);
}

void doSound(){
  if ((millis()/250)%2){
    tone(SND_PIN,600);
  }else{
    noTone(SND_PIN);
  }
}

   void doLights(){
    if (isPaused){
            for (uint8_t i = 0 ; i < 5 ; i++){
              leds[i].r =  leds[i].g =  leds[i].b = 30.0+cos((float)millis()/600.0*((float)i/10.0+1))*30.0; 
            }
            FastLED.show(); 
            return;
    }
//     if (lightMode == LightMode::Move){
//      
//     }
     if (lightMode == LightMode::Time){
          uint16_t tempTime = timeInSec;
          uint8_t seconds = tempTime % 10;
          tempTime = tempTime / 10;
          uint8_t tenSeconds = tempTime % 6;
          tempTime = tempTime / 6;
          uint8_t minutes = tempTime % 5;
          tempTime = tempTime / 5;
          uint8_t fiveMinutes = tempTime;

          seconds = seconds * seconds * 2;

           for (uint8_t i = 0 ; i < 5 ; i++){
                 if (tenSeconds == 5){
                   if (i == 0 || i == 4){
                     leds[i].r = 170; 
                   }else {
                     leds[i].r = seconds; 
                   }
                 }else {
                  if (i < tenSeconds ){
                      leds[i].r = 170; 
                  }else if (i == tenSeconds){
                      leds[i].r = seconds;
                  }else{
                      leds[i].r = 0;
                  }
                } 
                if (i < minutes) 
                  leds[i].b = 80;
                else
                  leds[i].b = 0;

                if (i < fiveMinutes) 
                  leds[i].g = 90;
                else
                  leds[i].g = 0;
            }
      FastLED.show(); 
     }
     if (prevMode == lightMode)
      return;
      
     prevMode = lightMode;
     
     switch(lightMode){

       case LightMode::Off:{
          leds[0] = CRGB::Black; 
          leds[1] = CRGB::Black; 
          leds[2] = CRGB::Black; 
          leds[3] = CRGB::Black; 
          leds[4] = CRGB::Black; 
      }break;
      case LightMode::Normal:{
          leds[0] = CRGB::Blue; 
          leds[1] = CRGB::Blue; 
          leds[2] = CRGB::Blue; 
          leds[3] = CRGB::Blue; 
          leds[4] = CRGB::Blue; 
      }break;
       case LightMode::Bright:{
          leds[0] = CRGB::White; 
          leds[1] = CRGB::White; 
          leds[2] = CRGB::White; 
          leds[3] = CRGB::White; 
          leds[4] = CRGB::White; 
      }break;
      case LightMode::Red:{
          leds[0] = CRGB::Red; 
          leds[1] = CRGB::Red; 
          leds[2] = CRGB::Red; 
          leds[3] = CRGB::Red; 
          leds[4] = CRGB::Red; 
      }break;
      case LightMode::Time:{
          leds[0] = CRGB::Black; 
          leds[1] = CRGB::Black; 
          leds[2] = CRGB::Black; 
          leds[3] = CRGB::Black; 
          leds[4] = CRGB::Black; 
      }break;
      case LightMode::Power:{
          uint8_t green = 0;
          uint8_t red = 0;
          uint8_t blue = 0;

          if (powerLvl<4.1){
            red = ( 4.1 - powerLvl ) * 255;
            if (powerLvl>3.5){
              green = ( powerLvl - 3.5 ) * 300;
            }
          }else{
            if (powerLvl>4.2){
              blue = 255;
              red = 255;
              green = 0;
            } else {
              blue = 500 * (powerLvl-4.1);
              green = 500 * (4.2 - powerLvl);
            }
          }


          for (uint8_t i = 0 ; i < 5 ; i++){
              leds[i].r = red; 
              leds[i].g = green; 
              leds[i].b = blue; 
          }
      }break;
   
     }
       FastLED.show(); 
   }

   void testPwr(){
    powerLvl = analogRead(BATLVL_PIN);
    powerLvl *= 2;    // we divided by 2, so multiply back
    powerLvl *= 3.3;  // Multiply by 3.3V, our reference voltage
    powerLvl /= 1024; // convert to voltage
    
    if (powerLvl<3.45){
        lightMode = LightMode::Off;
        doLights();
        for (uint8_t i = 0; i<4; i++){ 
          digitalWrite (13,LOW);
          delay(100);
          digitalWrite (13,HIGH);
          delay(100);
        }
        digitalWrite (PWRHOLD_PIN,LOW);
        delay(10000);
    }
   }

   void testButton(){
    
     if (analogRead(BTN_PIN)<BTN_THRESH){
        return;
     }
     for (uint8_t i=0; i <100 ; i++){
      delay(10);
      digitalWrite(13,i%5);
      if (analogRead(BTN_PIN)<BTN_THRESH){
        if (i < 10 && !isPaused){ // short hold
        if (lightMode == LightMode::Off){
            lightMode = LightMode::Normal;
          }else{
            lightMode = (LightMode)((uint8_t)lightMode+1);
          }
          return;
        } else { // long hold
           if (isPaused){
              isPaused = false;
              offsetTime += timeInSec - pauseStartTime;
           }else{
              isPaused = true;
              pauseStartTime = timeInSec;
           }
           return;
        }
      }
     }
   
       LightMode temp = lightMode;
       lightMode = LightMode::Power;
       doLights();
       delay(500);
       lightMode = temp;
       
       if (analogRead(BTN_PIN)<BTN_THRESH){ // just wanted to see power reading
         return;
       }
       // shut down hold (1.5 sec)
        digitalWrite (PWRHOLD_PIN,LOW);
        delay(10000);
   }


//  digitalWrite(13,true);
//        leds[0] = CRGB::Red; 
//        FastLED.show(); 
//   
//  delay(1000);
//    digitalWrite(13,false);
//        leds[0] = CRGB::Green; 
//               leds[1] = CRGB::Red; 
//        FastLED.show(); 
//    
//  delay(1000);
//           leds[0] = CRGB::Blue; 
//                   leds[1] = CRGB::Green; 
//               leds[2] = CRGB::Red; 
//        FastLED.show(); 
//    
//    digitalWrite(13,true);
//  delay(1000);
//      leds[0] = CRGB::White;
//       leds[1] = CRGB::Blue; 
//                   leds[2] = CRGB::Green; 
//               leds[3] = CRGB::Red; 
//      FastLED.show();
//    digitalWrite(13,false);
//  delay(1000);
//        leds[0] = CRGB::White;
//        leds[1] = CRGB::White;
//       leds[2] = CRGB::Blue; 
//                   leds[3] = CRGB::Green; 
//               leds[4] = CRGB::Red; 
//      FastLED.show();
//    digitalWrite(13,true);
//  delay(1000);
//          leds[0] = CRGB::White;
//          leds[1] = CRGB::White;
//        leds[2] = CRGB::White;
//       leds[3] = CRGB::Blue; 
//        leds[4] = CRGB::Green; 
//
//      FastLED.show();
//    digitalWrite(13,false);
//  delay(1000);
//            leds[0] = CRGB::White;
//          leds[1] = CRGB::White;
//        leds[2] = CRGB::White;
//       leds[3] = CRGB::White; 
//        leds[4] = CRGB::Blue; 
//
//      FastLED.show();
//    digitalWrite(13,true);
//  delay(200);
//          leds[0] = CRGB::White;
//          leds[1] = CRGB::White;
//        leds[2] = CRGB::White;
//       leds[3] = CRGB::White; 
//        leds[4] = CRGB::White; 
//
//      FastLED.show();
//      digitalWrite(13,false);
//  delay(200);
//      digitalWrite(13,true);
//  delay(200);
//      digitalWrite(13,false);
//  delay(200);
//      digitalWrite(13,true);
//  delay(200);
//      digitalWrite(13,false);
//  delay(200);
//    digitalWrite(5,false);
//     digitalWrite(13,true);
//  delay(100);
//      digitalWrite(13,false);
//  delay(100);
//      digitalWrite(13,true);
//  delay(100);
//      digitalWrite(13,false);
//  delay(100);
//      digitalWrite(13,true);
//  delay(100);
//      digitalWrite(13,false);
//        delay(100);
//      digitalWrite(13,true);

