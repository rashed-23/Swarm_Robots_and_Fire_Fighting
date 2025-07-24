#include<NewPing.h>
#define trig 13
#define echo 12
NewPing sonar(trig,echo);
void setup() {
Serial.begin(9600);
// put your setup code here, to run once:
}
void loop() {
unsigned int distance = sonar.ping_cm();
Serial.print("Distance: ");
Serial.print(distance);
Serial.println(" cm");
delay(1000);
}