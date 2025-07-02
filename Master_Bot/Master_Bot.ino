//....................................MPU6050 define......................................
#include <ESP.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <NewPing.h>  // Sonar sensor
#include <WiFi.h>
#include <esp_now.h>
#include <ESP32Servo.h>
Adafruit_MPU6050 mpu;

//........................................Servo define....................................

// Use the ESP32Servo library for ESP32
#define servo_control_pin 15  // Define the pin for the servo signal
Servo servo;                  // Create a servo object to control the servo

//.........................................Define motor control pins......................
#define IN1 4
#define IN2 16
#define IN3 17
#define IN4 5
#define ENA 2   // PWM for left motor
#define ENB 18  // PWM for right motor
#define trigger 13
#define echo 12
#define min_dis 50
#define out_to_cam 23
#define in_from_cam 14
#define Reset_pin 34

// #define trig 9
NewPing sonar(trigger, echo, 300);


// Communication functions and structure

//Slave's MacAddress : 94:54:C5:F1:B0:04
uint8_t receiverMAC[] = { 0x94, 0x54, 0xC5, 0xF1, 0xB0, 0x04 };
struct struct_message {
  float dist;
  float value;
};

struct_message myData;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

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


  ///// WiFi
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Attach the servo to the defined pin
  servo.attach(servo_control_pin);
  servo.write(45);
  delay(1000);

  // Configure MPU6050 settings
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);     // Sensitivity ±500°/s
  mpu.setFilterBandwidth(MPU6050_BAND_94_HZ);  // Noise filtering
  prevTime = millis();

  // Set motor pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  //esp to cam communication
  pinMode(out_to_cam, OUTPUT);
  pinMode(in_from_cam, INPUT);

  // Example condition to trigger a restart
  pinMode(Reset_pin, INPUT_PULLUP);  // Use GPIO 34 as a reset button
  digitalWrite(out_to_cam, LOW);
}

// Send Message
void send_message(float r, float target_angle) {

  myData.dist = r;
  myData.value = target_angle;

  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }
  //esp_now_deinit();
}

float distance() {
  float dis = sonar.ping_cm();
  return dis;
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
      // Serial.print("gyroZ:   ");
      // Serial.print(gyroZ);
      // Serial.print("         yaw_angle :    ");
      // Serial.print(yaw_angle);
      // Serial.print("         ref_angle :   ");
      // Serial.println(ref_angle);

      // int need = yaw_angle - ref_angle;
      // need = max(27, need);
      // int motorspeed = 255 * need / 45;  // Since max need 45 degree......(Rashed) // map(abs(ref_angle - yaw_angle),0,90,0,200)..(Saeed);
      // int error = 60 - motorspeed / 5;
      int motorspeed=150;
      analogWrite(ENA, motorspeed);
      analogWrite(ENB, motorspeed);

      Turn_right();

      //  delay(100);
      //  motor_stop();

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

      // Serial.print("gyroZ:   ");
      // Serial.print(gyroZ);
      // Serial.print("         yaw_angle :    ");
      // Serial.print(yaw_angle);
      // Serial.print("         ref_angle :   ");
      // Serial.println(ref_angle);
      // int need = ref_angle - yaw_angle;
      // need = max(27, need);
      // int motorspeed = 255 * need / 45;  // Since max need 45 degree......(Rashed) // map(abs(ref_angle - yaw_angle),0,90,0,200)..(Saeed);
      // int error = 60 - motorspeed / 5;
      int motorspeed=150;
      analogWrite(ENA, motorspeed);
      analogWrite(ENB, motorspeed);
      Turn_left();

      // delay(100);
      // motor_stop();

      prevTime = currTime;
    }
  }
  motor_stop();
  delay(1000);
}

float feedback;
float error, previous_error = 0;
float P, I = 0, D;
float PID_out;
unsigned long time1 = millis();
unsigned long time2;
float kp =100, ki =4, kd =2; 
void PID_Algo(int ref)
{
 sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  time2 = millis();

  float dt = (time2 - time1) / 1000.0;

  float gyroZ = g.gyro.z * 180 / PI;
  //if (gyroZ > 0.05)                  // in feedback small value has significance
  feedback += dt * gyroZ;

  error = ref - feedback;
  P = kp * error;
  I += error * dt;
  float derivative = (error - previous_error) / dt;
  D = kd * derivative;
  PID_out = P + (ki * I) + D;
  int motor_speed_change = constrain(PID_out, -105, 105);
  int motor_speed = 150;
  int motorR = motor_speed - motor_speed_change;
  int motorL = motor_speed + motor_speed_change;
  // Serial.print("R = ");
  // Serial.print(motorR);
  // Serial.print("   L = ");
  // Serial.println(motorL);
  analogWrite(ENA, motorR);
  analogWrite(ENB, motorL);

  previous_error = error;
  time1 = time2;
}

void loop() {

  bool object = false;
  if (!object) {
    for (int i = 45; i <= 135; i++) {
      float dis = distance();
      Serial.println(dis);
      //delay(200);
      if (dis < min_dis and dis!=0) {

        if (!dis) {
          delay(25);
          dis = distance();
        }

        Serial.println("Object detected!");
        delay(500);
        servo.write(90);
        delay(500);
        target_angle = i;
        r = dis;
        object = true;
        Serial.print("Target Angle: ");
        Serial.println(target_angle);
        break;
      } else {
        servo.write(i);
        delay(20);
      }
    }
  }
  if (!object) {
    for (int i = 135; i >= 45; i--) {
      float dis = distance();
      if (dis < min_dis and dis!=0) {
        if (!dis) {
        delay(25);
        dis = distance();
      }
      Serial.println(dis);
        delay(500);
        servo.write(90);
        delay(500);
        Serial.println("Object detected!");
        object = true;
        r = dis;
        target_angle = i;
        Serial.print("Target Angle: ");
        Serial.println(target_angle);
        break;
      } 
      else {
        servo.write(i);
        delay(20);
      }
    }
  }
  prevTime = millis();
  if (object) {
    //Test perpose
    // send_message(r, target_angle);
    //return;
    rotateByAngle(target_angle);
    delay(100);
    feedback=target_angle;
    float D = distance();

    //// Filtering if sudden 0
    if (!D) {
      delay(25);
      D = distance();
    }
    Serial.print("DIS111: ");
    Serial.println(D);
    delay(50);
    forward();
    delay(20);

    while (D > min_stop_dis or D==0.00) {
      time1 = millis();
      delay(20);
      D = distance();
      //// Filtering if sudden 0
      if (!D) {
        delay(25);
        D = distance();
      }
      Serial.print("DIS: ");
      Serial.println(D);
      PID_Algo(target_angle);
    }
    motor_stop();
    delay(1000);


    // digitalWrite(out_to_cam, HIGH);
    int read_from_cam = digitalRead(in_from_cam);
    delay(1000);

    Serial.print("Read_from_cam = ");
    Serial.println(read_from_cam);

    while (read_from_cam != LOW) {
      read_from_cam = digitalRead(in_from_cam);
      delay(100);
      Serial.print("Read_from_cam = ");
      Serial.println(read_from_cam);
    }

    send_message(r, target_angle);

    rotateByAngle(target_angle + 90);
    forward();
    delay(1000);
    motor_stop();
    Serial.println("Yessssss");
    servo.write(45);
    while (1)
      delay(500);
  }
}
