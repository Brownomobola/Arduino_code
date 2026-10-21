const int pirPin = 3;
volatile bool motionDetected = false;

void setup() {
  pinMode(pirPin, INPUT);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(pirPin), pirISR, RISING);
}

void loop() {
  if (motionDetected) {
    Serial.println("Motion Detected!");
    motionDetected = false;
  }
}

void pirISR() {
  motionDetected = true;
}
  