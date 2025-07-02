#include <esp_now.h>
#include <WiFi.h>

// Structure to hold incoming data
struct struct_message {
  char text[32];  // Example: A text message
  int number;     // Example: A number
};

// Callback function to handle received data
void OnDataRecv(const esp_now_recv_info_t *esp_now_info, const uint8_t *incomingData, int len) {
  struct_message receivedData;
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Text: ");
  Serial.println(receivedData.text);
  Serial.print("Number: ");
  Serial.println(receivedData.number);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback function for receiving data
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Receiver initialized");
}

void loop() {
  // Nothing to do here
}