#include <WiFi.h>
#include <esp_now.h>

// Receiver's MAC Address (Station MAC Address)
uint8_t receiverMAC[] = {0x94, 0x54, 0xC5, 0xF1, 0xB0, 0x04};  // Replace with actual MAC

// Define the structure of the data
struct struct_message {
  char text[32];
  int value;
};

struct_message myData;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());  // Optional: print your own MAC

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register send callback
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Fill in data
  strcpy(myData.text, "Bismillahir Rahmanir Raheem, ALHAMDULILLAH");
  myData.value = random(0, 100);

  // Send data
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.print("Send Error: ");
    Serial.println(esp_err_to_name(result));
  }

  delay(2000);  // Wait 2 seconds
}
