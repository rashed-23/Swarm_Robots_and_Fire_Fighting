//.........................................Define motor control pins......................

#define IN1 4       // Motor A input pin 1 (controls direction)
#define IN2 16      // Motor A input pin 2 (controls direction)
#define IN3 17      // Motor B input pin 1 (controls direction)
#define IN4 5       // Motor B input pin 2 (controls direction)
#define ENA 2       // PWM pin to control speed of Motor A (left motor)
#define ENB 18      // PWM pin to control speed of Motor B (right motor)

void setup() {
  Serial.begin(115200);   // Start Serial Monitor at 115200 baud rate (for debugging)

  // Set all motor control pins as OUTPUT so we can send signals to L298N
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);   // Enable pin for left motor
  pinMode(ENB, OUTPUT);   // Enable pin for right motor
}

// Function to move both motors forward
void forward() {
  digitalWrite(IN1, HIGH); // Motor A forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); // Motor B forward
  digitalWrite(IN4, LOW);
}

// Function to move both motors backward
void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH); // Motor A backward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH); // Motor B backward
}

// Function to turn robot to the left
void Turn_left() {
  digitalWrite(IN1, HIGH); // Left motor forward
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  // Right motor stopped
  digitalWrite(IN4, LOW);
}

// Function to turn robot to the right
void Turn_right() {
  digitalWrite(IN1, LOW);  // Left motor stopped
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); // Right motor forward
  digitalWrite(IN4, LOW);
}

// Function to stop both motors
void motor_stop() {
  digitalWrite(IN1, LOW);  // Motor A stop
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  // Motor B stop
  digitalWrite(IN4, LOW);
}

void loop() {
  // Test Motor Sequence

  int motorspeed = 110; // Motor speed (0–255). Lower = slower, higher = faster

  // Set speed using PWM
  analogWrite(ENA, motorspeed); // Left motor speed
  analogWrite(ENB, motorspeed); // Right motor speed

  forward();          // Move forward
  delay(3000);        // Move for 3 seconds

  motor_stop();       // Stop motors
  delay(1000);        // Wait for 1 second

  Turn_right();       // Turn right
  delay(3000);        // Turn for 3 seconds

  motor_stop();       // Stop motors
  delay(1000);        // Wait for 1 second

  Turn_left();        // Turn left
  delay(3000);        // Turn for 3 seconds

  motor_stop();       // Stop motors
  delay(1000);        // Wait for 1 second

  // Loop repeats from beginning
}