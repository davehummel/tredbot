#include <Adafruit_NeoPixel.h>

  Adafruit_NeoPixel strip = Adafruit_NeoPixel(2, 6, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(13,OUTPUT);
  pinMode(5,OUTPUT);
  strip.begin();
  strip.show(); 
}

void loop() {

  digitalWrite(5,true);
  digitalWrite(13,true);
  strip.setPixelColor(1, 255, 0, 0);
  delay(1000);
    digitalWrite(13,false);
   strip.setPixelColor(0, 0, 255, 0);
  delay(1000);
    strip.setPixelColor(0, 0, 0, 255);
    digitalWrite(13,true);
  delay(1000);
    strip.setPixelColor(0, 255, 255, 255);
    digitalWrite(13,false);
  delay(1000);
    digitalWrite(13,true);
  delay(1000);
    digitalWrite(13,false);
  delay(1000);
    digitalWrite(13,true);
  delay(200);
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
  delay(5000);

}
