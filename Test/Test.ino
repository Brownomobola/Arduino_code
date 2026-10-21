#include <Servo.h>
const int servoPin = 2;
int servoPos;

Servo myServo;

void setup() {
  myServo.attach(servoPin);
}

void loop() {
  myServo.write(0);

  delay(500);

  myServo.write(90);

  delay(500);

  myServo.write(180);

  delay(500);
}
