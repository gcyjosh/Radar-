#include <Servo.h>

// --- Pin definitions ---
const int trigPin = 10;
const int echoPin = 11;
const int servoPin = 12;

// --- Sweep settings ---
const int SWEEP_MIN = 15;
const int SWEEP_MAX = 165;
const int STEP_DELAY = 30;      // ms per degree step (governs sweep speed)

// --- Ultrasonic settings ---
const unsigned long PULSE_TIMEOUT = 25000UL; // ~25ms -> ~4m max range, prevents blocking forever
const int MAX_DISTANCE_CM = 400;             // clamp unreasonable readings

Servo myServo;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
  myServo.attach(servoPin);
}

void loop() {
  for (int angle = SWEEP_MIN; angle <= SWEEP_MAX; angle++) {
    myServo.write(angle);
    delay(STEP_DELAY);
    sendReading(angle);
  }
  for (int angle = SWEEP_MAX; angle >= SWEEP_MIN; angle--) {
    myServo.write(angle);
    delay(STEP_DELAY);
    sendReading(angle);
  }
}

// Takes a reading and sends it in "angle,distance." format, newline-terminated for Processing
void sendReading(int angle) {
  int distance = readDistanceCM();
  Serial.print(angle);
  Serial.print(",");
  Serial.print(distance);
  Serial.println("."); // println appends "\n", matching Processing's bufferUntil('\n')
}

// Pings the HC-SR04 and returns distance in cm, with timeout + clamping
int readDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, PULSE_TIMEOUT);

  if (duration == 0) {
    // No echo received within timeout -> treat as "out of range"
    return MAX_DISTANCE_CM;
  }

  int distance = (int)(duration * 0.034 / 2);
  return constrain(distance, 0, MAX_DISTANCE_CM);
}
