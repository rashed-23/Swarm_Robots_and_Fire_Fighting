#include "WiFi.h"

void setup() {
  Serial.begin(115200);

  // Get the MAC address of the ESP32
  WiFi.begin();
  String macAddress = WiFi.macAddress();
  
  Serial.print("ESP32 MAC Address: ");
  Serial.println(macAddress);
}

void loop() {
  // Nothing here
}
