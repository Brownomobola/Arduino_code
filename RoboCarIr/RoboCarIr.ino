int AIN1 = 9;
int AIN2 = 8;
int BIN1 = 7;
int BIN2 = 6;

int STBY = 3;

int PWMA = 11;
int PWMB = 10;

const int leftIrPin = 5;
const int rightIrPin = 4;
int leftIrState = 0;
int rightIrState = 0;

//int delayTime = 2000;
//int currSeq = 1;
//unsigned long prevTime = 0;
//unsigned long currTime = 0;

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

/*
void check() {
  currTime = millis();
  if ((currTime - prevTime) >= delayTime) {
    prevTime = currTime; // reset the timer
    currSeq = (++currSeq) % 4; // change the sequence
  }
}
*/

void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(leftIrPin, INPUT);
  pinMode(rightIrPin, INPUT);

  digitalWrite(STBY, HIGH);

  analogWrite(PWMA, 200);
  analogWrite(PWMB, 200);
}

void loop() {
  leftIrState  = digitalRead(leftIrPin);
  rightIrState = digitalRead(rightIrPin);

  if (leftIrState == LOW && rightIrState == LOW) { // The car is following the line
    forward();
  }
  else if (leftIrState == LOW && rightIrState == HIGH) { // The car is beginning to deviate to the left
    right();
  }
  else if (leftIrState == HIGH && rightIrState == LOW) { // The car is beginning to deviate to the right
    left();
  }
  else if (leftIrState == HIGH && rightIrState == HIGH) { // A petty check to make it stop on marked lines
    digitalWrite(STBY, LOW);
  }
}
