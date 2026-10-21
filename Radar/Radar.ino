#include <Servo.h>
Servo servo;

const int servoPin = 9;

const int trigPin = A2;
const int echoPin = A1;

const int sweepAngle = 10;
const int maxAngle = 160;
const int minAngle = 10;
int currAngle = 10;
bool forwardPass = true;

float distance;

const float soundSpeed = 0.0343;
unsigned long scanTime = 0; 

float getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH);

  if (duration == 0) return 999;
  
  return (duration * soundSpeed) / 2.0;
}

bool checkTime(unsigned long delayTime, unsigned long &tracker) {
  if ((millis() - tracker) >= delayTime) {
    tracker = millis();
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  servo.attach(servoPin);
  servo.write(currAngle);
}

void loop() {
  if (checkTime(500, scanTime)) {
    distance = getDistance(trigPin, echoPin);
    Serial.print("Object at servo position ");
    Serial.print(currAngle);
    Serial.print(" is ");
    Serial.print(distance);
    Serial.println("cm away");
    if (forwardPass) {
    currAngle += sweepAngle;} else {
      currAngle -= sweepAngle;
    }
    scanTime = millis();
  }

  if (currAngle > maxAngle) {
    forwardPass = false;
  } else if (currAngle < minAngle) {
    forwardPass = true;
  }
  servo.write(currAngle);
}
