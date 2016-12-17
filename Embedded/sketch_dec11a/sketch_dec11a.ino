    #include <FastLED.h>
    #define NUM_LEDS 5
    #define DATA_PIN 6

    CRGB leds[NUM_LEDS];
    
void setup() {
  pinMode(13,OUTPUT);
  pinMode(5,OUTPUT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  delay(1);
  leds[0] = CRGB::Black; 
  leds[1] = CRGB::Black; 
  leds[2] = CRGB::Black; 
  leds[3] = CRGB::Black; 
  leds[4] = CRGB::Black; 
  
          FastLED.show(); 
  delay(100);
}

void loop() {

  digitalWrite(5,true);
  digitalWrite(13,true);
        leds[0] = CRGB::Red; 
        FastLED.show(); 
   
  delay(1000);
    digitalWrite(13,false);
        leds[0] = CRGB::Green; 
               leds[1] = CRGB::Red; 
        FastLED.show(); 
    
  delay(1000);
           leds[0] = CRGB::Blue; 
                   leds[1] = CRGB::Green; 
               leds[2] = CRGB::Red; 
        FastLED.show(); 
    
    digitalWrite(13,true);
  delay(1000);
      leds[0] = CRGB::White;
       leds[1] = CRGB::Blue; 
                   leds[2] = CRGB::Green; 
               leds[3] = CRGB::Red; 
      FastLED.show();
    digitalWrite(13,false);
  delay(1000);
        leds[0] = CRGB::White;
        leds[1] = CRGB::White;
       leds[2] = CRGB::Blue; 
                   leds[3] = CRGB::Green; 
               leds[4] = CRGB::Red; 
      FastLED.show();
    digitalWrite(13,true);
  delay(1000);
          leds[0] = CRGB::White;
          leds[1] = CRGB::White;
        leds[2] = CRGB::White;
       leds[3] = CRGB::Blue; 
        leds[4] = CRGB::Green; 

      FastLED.show();
    digitalWrite(13,false);
  delay(1000);
            leds[0] = CRGB::White;
          leds[1] = CRGB::White;
        leds[2] = CRGB::White;
       leds[3] = CRGB::White; 
        leds[4] = CRGB::Blue; 

      FastLED.show();
    digitalWrite(13,true);
  delay(200);
          leds[0] = CRGB::White;
          leds[1] = CRGB::White;
        leds[2] = CRGB::White;
       leds[3] = CRGB::White; 
        leds[4] = CRGB::White; 

      FastLED.show();
      digitalWrite(13,false);
  delay(200);
      digitalWrite(13,true);
  delay(200);
      digitalWrite(13,false);
  delay(200);
      digitalWrite(13,true);
  delay(200);
      digitalWrite(13,false);
  delay(200);
    digitalWrite(5,false);
     digitalWrite(13,true);
  delay(100);
      digitalWrite(13,false);
  delay(100);
      digitalWrite(13,true);
  delay(100);
      digitalWrite(13,false);
  delay(100);
      digitalWrite(13,true);
  delay(100);
      digitalWrite(13,false);
        delay(100);
      digitalWrite(13,true);
  delay(10000);

}
