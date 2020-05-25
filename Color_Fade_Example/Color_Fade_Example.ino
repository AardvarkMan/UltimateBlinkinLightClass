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
  FadeInOut(0xff, 0x00, 0x00); // red
  FadeInOut(0xff, 0xff, 0xff); // white
  FadeInOut(0x00, 0x00, 0xff); // blue
}

void FadeInOut(byte red, byte green, byte blue){
  float r, g, b;
     
  for(int k = 0; k < 256; k=k+1) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
     
  for(int k = 255; k >= 0; k=k-2) {
    r = (k/256.0)*red;
    g = (k/256.0)*green;
    b = (k/256.0)*blue;
    setAll(r,g,b);
    showStrip();
  }
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
