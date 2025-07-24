// Define pins for HC-SR04
const int trigPin = 13;
const int echoPin = 12;
void setup() {
Serial.begin(115200);
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
}
void loop() {
// Send a 10us HIGH pulse to trigger the sensor
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
// Measure the time for echo to return
long duration = pulseIn(echoPin, HIGH);
// Calculate distance (Speed of sound = 34300 cm/s)
float distance = duration * 0.0343 / 2;
// Print distance to Serial Monitor
Serial.print("Distance: ");
Serial.print(distance);
Serial.println(" cm");
delay(500);
}