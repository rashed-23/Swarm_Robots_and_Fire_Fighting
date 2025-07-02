#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

float gyroZ, yawAngle = 0;  // Yaw angle calculation
unsigned long prevTime, currTime;
float dt;
float mx=-1;
void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 initialization failed!");
    while (1)
      ;
  }
  Serial.println("MPU6050 connected!");

  // Configure MPU6050 settings
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);     // Sensitivity ±500°/s
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);  // Noise filtering

  prevTime = millis();
  
}

void loop() {

  // Read sensor data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Read Z-axis gyroscope data (yaw rotation)
  gyroZ = g.gyro.z * 180 / PI;  // Convert rad/s to deg/s

  // Apply threshold to remove drift (adjust if necessary)
  currTime = millis();
  dt = (currTime - prevTime) / 1000.0;  // Convert to second

  if (abs(gyroZ) > 0.5) {
    yawAngle += gyroZ * dt;  // Fltering for small garbage 
  }                          
  Serial.print("ZayroZZZ: ");
  Serial.println(gyroZ);

  // Print yaw angle
  Serial.print("Yaw Angle: ");
  Serial.println(yawAngle);

  mx=max(mx,abs(gyroZ));
  prevTime = currTime;
  Serial.print("MAX = ");
  Serial.println(mx);
  delay(50);  // Small delay for stability
}
