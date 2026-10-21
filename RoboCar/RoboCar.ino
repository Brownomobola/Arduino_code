int AIN1 = 9;
int AIN2 = 8;
int BIN1 = 6;
int BIN2 = 4;

int STBY = 3;

int PWMA = 11;
int PWMB = 5;

int delayTime = 2000;
int currSeq = 1;
unsigned long prevTime = 0;
unsigned long currTime = 0;

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

void check() {
  currTime = millis();
  if ((currTime - prevTime) >= delayTime) {
    prevTime = currTime; // reset the timer
    currSeq = (++currSeq) % 4; // change the sequence
  }
}

void setup() {
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  pinMode(STBY, OUTPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  digitalWrite(STBY, HIGH);

  analogWrite(PWMA, 200);
  analogWrite(PWMB, 200);
}

void loop() {
  // put your main code here, to run repeatedly:
  while (currSeq == 1) {
    forward();
    check();
  }

  while (currSeq == 2) {
    reverse();
    check();
  }

  while (currSeq == 3) {
    left();
    check();
  }

  while (currSeq == 0) {
    right();
    check();
  }
}
