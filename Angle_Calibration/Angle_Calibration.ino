//....................................MPU6050 define.....................................
#include <Adafruit_MPU6050.h>
Adafruit_MPU6050 mpu;

//........................................Servo define....................................

// Use the ESP32Servo library for ESP32
                 // Create a servo object to control the servo

float current_angle;
float target_angle;
float gyroZ, yaw_angle = 0;  // Yaw angle calculation
unsigned long prevTime, currTime;
float min_stop_dis = 10.0;
float dt;
float ref_angle;
float r;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1)
      ;
  }

  Serial.println("MPU6050 connected!");


}

void rotateByAngle(float theta) {

  ref_angle = theta - 90.00;
  //Serial.println(ref_angle);
  //Serial.println(theta);
  if (theta < 90.00) {
    while ((yaw_angle - ref_angle) > 1.00) {
      currTime = millis();
      dt = (currTime - prevTime) / 1000.0;
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      gyroZ = g.gyro.z * 180 / PI;

      if (abs(gyroZ) > 0.5) {
        yaw_angle += gyroZ * dt;  // Integrate to calculate yaw angle
      }
      Serial.print("gyroZ:   ");
      Serial.print(gyroZ);
      Serial.print("         yaw_angle :    ");
      Serial.print(yaw_angle);
      Serial.print("         ref_angle :   ");
      Serial.println(ref_angle);

      // int need=yaw_angle-ref_angle;
      // need=max(27,need);
      // int motorspeed = 255*need/45;  // Since max need 45 degree......(Rashed) // map(abs(ref_angle - yaw_angle),0,90,0,200)..(Saeed);
      // int error= 60-motorspeed/5;
      // analogWrite(ENA, motorspeed-error);
      // analogWrite(ENB, motorspeed);
      // Turn_right();
      prevTime = currTime;
    }
  } else {
    while ((ref_angle-yaw_angle) > 1.00) {
      currTime = millis();
      dt = (currTime - prevTime) / 1000.0;
      sensors_event_t a, g, temp;
      mpu.getEvent(&a, &g, &temp);
      gyroZ = g.gyro.z * 180 / PI;

      if (abs(gyroZ) > 0.5) {
        yaw_angle += gyroZ * dt;  // Integrate to calculate yaw angle
      }

      Serial.print("gyroZ:   ");
      Serial.print(gyroZ);
      Serial.print("         yaw_angle :    ");
      Serial.print(yaw_angle);
      Serial.print("         ref_angle :   ");
      Serial.println(ref_angle);


      // int need=ref_angle-yaw_angle;
      // need=max(27,need);
      // int motorspeed = 255*need/45;  // Since max need 45 degree......(Rashed) // map(abs(ref_angle - yaw_angle),0,90,0,200)..(Saeed);
      // int error= 60-motorspeed/5;
      // analogWrite(ENA, motorspeed-error);
      // analogWrite(ENB, motorspeed);
      // Turn_left();
      prevTime = currTime;
    }
  }
  Serial.println("Paisiiiiiiiiiiiiiiiiiiiiiiiiiiii");
  delay(1000);
}

void loop() {

  prevTime = millis();
      //target_angle=50;
      //rotateByAngle(target_angle);
      target_angle=130;
      rotateByAngle(target_angle);
      rotateByAngle(target_angle+90);
 
}
