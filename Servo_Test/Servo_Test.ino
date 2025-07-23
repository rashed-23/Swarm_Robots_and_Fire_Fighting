#include <ESP32Servo.h>

#define servo_control_pin 15

Servo servo;

void setup() {
  Serial.begin(115200);
  servo.attach(servo_control_pin);
  servo.write(0);
  delay(1000);
}

void loop() {
  for (int i = 0; i <= 180; i++) {
    servo.write(i);
    delay(20);
  }

  for (int i = 180; i >= 0; i--) {
    servo.write(i);
    delay(20);
  }
}
