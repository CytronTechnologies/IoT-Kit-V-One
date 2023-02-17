// Onboard NeoPixel simple sketch for Cytron Maker Feather AIoT S3
// Cannot be controlled in V-ONE

#include <Adafruit_NeoPixel.h>

#define PIN 46
#define NUMPIXELS 1 
#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {
  pixels.clear(); // Set all pixel colors to 'off'

  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255

    pixels.setPixelColor(i, pixels.Color(0, 150, 0));  // moderately bright green color
    pixels.show();
    delay(DELAYVAL);
    pixels.setPixelColor(i, pixels.Color(150, 0, 0));  // moderately bright red color
    pixels.show();
    delay(DELAYVAL);
    pixels.setPixelColor(i, pixels.Color(0, 0, 150));  // moderately bright blue color
    pixels.show();
    delay(DELAYVAL);
  }
}
