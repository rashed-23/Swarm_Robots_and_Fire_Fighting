//....................................MPU6050 define........................................
#include <ESP.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <NewPing.h>  // Sonar sensor
#include <WiFi.h>
#include <esp_now.h>
Adafruit_MPU6050 mpu;

bool fire_detected = false;
float current_angle;
float target_angle;
float gyroZ, yaw_angle = 0;  // Yaw angle calculation
unsigned long prevTime, currTime;
float dt;
float ref_angle;
float r;
float x = 23.0;
float min_stop_dis = 10.0;



//.........................................Define motor control pins......................
#define IN1 4
#define IN2 16
#define IN3 17
#define IN4 5
#define ENA 2   // PWM for left motor
#define ENB 18  // PWM for right motor

#define pump 13

#define p1 12
#define p2 14

//.........................................Define Sonar pins..............................................
#define trigger 15
#define echo 23

//#define trig 9
NewPing sonar(trigger, echo);




//................................................RESET....................................................
#define Reset_pin 34



//......................................... Define Bazzer pin ...............................................

#define bazzer 19




//....................................  Communication functions and structure ..............................

struct struct_message {
  float dist;
  float value;
};

struct_message receivedData;

void OnDataRecv(const esp_now_recv_info_t *esp_now_info, const uint8_t *incomingData, int len) {
  struct_message receivedData;
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  r = receivedData.dist;
  target_angle = receivedData.value;
  fire_detected = true;
}






void setup() {
  Serial.begin(115200);
  Wire.begin();


  //................................................ MPU6050 setup ....................................
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1)
      ;
  }

  Serial.println("MPU6050 connected!");


  // Configure MPU6050 settings
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);     // Sensitivity ±500°/s
  mpu.setFilterBandwidth(MPU6050_BAND_94_HZ);  // Noise filtering
  prevTime = millis();



  //........................................... Set motor pins as outputs .................................
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(pump, OUTPUT);
  pinMode(p1, OUTPUT);
  pinMode(p2, OUTPUT);

  digitalWrite(p1, LOW);
  digitalWrite(p2, HIGH);

  //......................................... Define Bazzer pin ...............................................
  pinMode(bazzer, OUTPUT);
  digitalWrite(bazzer,LOW);

  // ......................................Example condition to trigger a restart ...........................

  pinMode(Reset_pin, INPUT_PULLUP);  // Use GPIO 34 as a reset button




  //.......................................... Receive Message and the call back function ........................

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


// .......................................... distance measurment ...................................................
float distance() {
  float dis = sonar.ping_cm();
  return dis;
}



// .............................................For Rotating Angle ..................................................
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
      int motorspeed = 150;  // map(abs(ref_angle - yaw_angle),0,90,0,200);
      analogWrite(ENA, motorspeed);
      analogWrite(ENB, motorspeed);
      Turn_right();
      prevTime = currTime;
    }
  } else {
    while ((ref_angle - yaw_angle) > 1.00) {
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
      int motorspeed = 150;  // map(abs(ref_angle - yaw_angle),0,90,0,200);
      analogWrite(ENA, motorspeed);
      analogWrite(ENB, motorspeed);
      Turn_left();
      prevTime = currTime;
    }
  }
  motor_stop();
  delay(1000);
}

///// .....................................PID...........................................


float feedback;
float error, previous_error = 0;
float P, I = 0, D;
float PID_out;
unsigned long time1 = millis();
unsigned long time2;
float kp = 100, ki = 4, kd = 2;

void PID_Algo(int ref) {
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


///////........................................ Main Loop function ......................................
void loop() {
  prevTime = millis();
  if (fire_detected) {
    //Test_perpose
    Serial.print("Radius: ");
    Serial.println(r);
    Serial.print("Target Angle: ");
    Serial.println(target_angle);
    Serial.println("Alhamdulillah");
    // return;
    digitalWrite(bazzer, HIGH);
    feedback = target_angle;
    if (target_angle > 90) {
      target_angle = target_angle - 90;
      float rad_angle = target_angle * PI / 180.0;
      float y = (r * sin(rad_angle)) / (r * cos(rad_angle) + x);
      target_angle = atan(y) * 180.0 / PI;
      target_angle = target_angle + 90;
    } else {
      float rad_angle = target_angle * PI / 180.0;
      float y = (r * sin(rad_angle)) / (r * cos(rad_angle) + x);
      target_angle = atan(y) * 180.0 / PI;
    }

    Serial.print("Target Angle: ");
    Serial.println(target_angle);

    rotateByAngle(target_angle);

    delay(100);
    forward();
    delay(20);
    float D = distance();

    if (D == 0.00) {
      delay(20);
      D = distance();
      delay(20);
    }
    Serial.print("Dis01: ");
    Serial.println(D);
    while (D > min_stop_dis or D == 0.00) {
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

    digitalWrite(pump, HIGH);
    delay(1000);
    digitalWrite(pump, LOW);

    rotateByAngle(target_angle + 90);
    forward();
    delay(1000);
    motor_stop();
    delay(1000);
    rotateByAngle(target_angle + 180);
    motor_stop();
    delay(1000);
    // servo.write(0);
    while (1) { delay(500); }
  } else {
    Serial.println("I am returning");
  }
}
