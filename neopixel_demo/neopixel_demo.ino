#include <Adafruit_NeoPixel.h>// https://github.com/adafruit/Adafruit_NeoPixel

#define PIN 26
#define LED_NUM 4
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200); //start serial port on 115200bps
  strip.begin();
  strip.setBrightness(50); //set maximum brightness: 0-255
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  /*                  Pixel number, first led is 0.
                      |              Red value, can be from 0 to 255.
                      |              |    Green value
                      |              |    |     Blue value.               
                      |              |    |     |                             */
  strip.setPixelColor(0, strip.Color(255, 0,    0)); //set pixel 1 to RED
  strip.setPixelColor(1, strip.Color(255, 255,  255)); //set pixel 2 to WHITE
  
  //Set pixels 3 and 4 to green and blue, or any other color that you want.
  //You can just copy and paste the "two strip.setPixelColor..."
  
  strip.show(); //tell pixels to display set colors
  delay(200); //wait 200us (0.2s)
}
