#include <FastLED.h>

#define LED_PIN     1
#define NUM_LEDS    50
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2811, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
}

void loop() {
  TwinkleRandom(20, 100, false);
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
 
  for (int i=0; i<Count; i++) {
     setPixel(random(NUM_LEDS),random(0,255),random(0,255),random(0,255));
     showStrip();
     delay(SpeedDelay);
     if(OnlyOne) {
       setAll(0,0,0);
     }
   }
 
  delay(SpeedDelay);
}

void showStrip() {
   FastLED.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}
