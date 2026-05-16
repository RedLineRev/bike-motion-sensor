#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 33
#define NEOPIXEL_COUNT 1

Adafruit_NeoPixel pixel(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    pixel.begin();
    pixel.setBrightness(50);
}

void loop() {
    pixel.setPixelColor(0, pixel.Color(0, 255, 0)); // green on
    pixel.show();
    delay(500);
    pixel.setPixelColor(0, pixel.Color(0, 0, 0));   // off
    pixel.show();
    delay(500);
}
