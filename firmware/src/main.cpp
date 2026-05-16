#include <Arduino.h>
#include <Wire.h>

void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Boot!");
    Wire.begin();
    Serial.println("I2C ready.");
}

void loop() {
    Serial.println("Scanning I2C bus...");
    int found = 0;
    for (byte addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.print("Device found at address 0x");
            Serial.println(addr, HEX);
            found++;
        }
    }
    if (found == 0) Serial.println("No I2C devices found.");
    else Serial.println("Scan complete.");
    delay(3000);
}
