#include <Servo.h>
Servo servo;

// Motor Driver config
const int AIN1 = 9;
const int AIN2 = 8;
const int BIN1 = 7;
const int BIN2 = 6;
const int STBY = 3;
const int PWMA = 11;
const int PWMB = 5;
 
// IR sensor config
const int leftIrPin = 5;
const int rightIrPin = 4;
int leftIrState = 0;
int rightIrState = 0;

// Servo config
const int servoPin = 2;
const int servoPosLeft = 150;
const int servoPosRight = 30;
const int servoPosCentre = 90;

// Ultra Sonic sensor config
const int trigPin = A1;
const int echoPin = A2;
const float soundSpeed = 0.0343;
long duration = 0;
float distanceLeft = 0;
float distanceCentre = 0;
float distanceRight = 0;

unsigned long prevTime = 0;
unsigned long currTime = 0;

enum Mode {
  NORMAL_DRIVING,
  ESCAPE_REVERSE,
  ESCAPE_TURNING,
  ESCAPE_SCANNING
};

Mode currState = NORMAL_DRIVING;

unsigned long manueverStartTime = 0;


void reverse() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

void forward() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

void left() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
}

void right() {
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}

void brake() {
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, LOW);
}

/**
 * Checks if the delayTime has elapsed.
 * 
 * Returns True if the time has elapsed; else False
 */
bool checkTime(int delayTime) {
  currTime = millis();
  if ((currTime - prevTime) >= delayTime) {
    prevTime = currTime; // reset the timer
    return true;
  }
  return false;
}


/**
 * Calculates the distance to an object using the HC-SR04 sensor.
 * 
 * This function triggers the ultrasonic sensor, measures the echo 
 * duration, and applies the speed of sound to calculate the distance.
 */
float getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(10);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echo, HIGH, 24000);

  float distance = (duration * soundSpeed) / 2.0;
  Serial.println(distance);

  return distance;
}

/**
 * Sweeps the front area of the Robot car for obstacles.
 *
 * Writes the distance of the obstacles found to global
 * variables.
*/
void obstacleCheck() {
  servo.write(servoPosLeft);
  delay(500);
  distanceLeft = getDistance(trigPin, echoPin);
  delay(500);

  servo.write(servoPosCentre);
  delay(500);
  distanceCentre = getDistance(trigPin, echoPin);
  delay(500);

  servo.write(servoPosRight);
  delay(500);
  distanceRight = getDistance(trigPin, echoPin); 
  delay(500);

  servo.write(servoPosCentre);
}

/**
 * A helper function to check if a direction is blocked
 * 
 * Returns a bool
*/
bool isBlocked(float distance) {
  if (distance < 20.0) {
    return true;
  }
  return false;
}


/**
 * Chooses a direction to move the vehicle based on a series of 
 * conditions
*/
void chooseDir() {
  if (isBlocked(distanceLeft) && isBlocked(distanceCentre) && isBlocked(distanceRight)) {
    reverse();
    currState = ESCAPE_REVERSE;
    manueverStartTime = millis();
    return;
  } 
  
  else if (isBlocked(distanceCentre)) {
    reverse();
    delay(500);
    brake();
    obstacleCheck();
    if (distanceLeft > distanceRight) {
      left();
      delay(500);
      Serial.println("Turning left");
    } else {
      right();
      delay(500);
      Serial.println("Turning right");
    }
    return;
  } 
  
  else if (!isBlocked(distanceLeft)) {
    left();
    delay(500);
    return;
  } 
  
  else if (!isBlocked(distanceRight)) {
    right();
    delay(500);
    return;
  } 
  
  else {
    forward();
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  servo.attach(servoPin);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(leftIrPin, INPUT);
  pinMode(rightIrPin, INPUT);

  digitalWrite(STBY, HIGH);

  servo.write(90);

  obstacleCheck();

  analogWrite(PWMA, 200);
  analogWrite(PWMB, 200);
}

void loop() {
  switch (currState) {
    case NORMAL_DRIVING:
      if (checkTime(100)) {
        distanceCentre = getDistance(trigPin, echoPin);
      }
      chooseDir();
      break;

    case ESCAPE_REVERSE:
      if (millis() - manueverStartTime >= 500) {
        right();
        currState = ESCAPE_TURNING;
        manueverStartTime = millis();
      }
      break;

    case ESCAPE_TURNING:
      if (millis() - manueverStartTime >= 500) {
        brake();
        currState = ESCAPE_SCANNING;
      }
      break;

    case ESCAPE_SCANNING:
      obstacleCheck();
      forward();
      Serial.println("Moving Forward");
      currState = NORMAL_DRIVING;
      break;
  }
}