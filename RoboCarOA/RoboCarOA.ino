#include <Servo.h>
Servo servo;

// ---------------------------------------------------------
// HARDWARE CONFIGURATION
// ---------------------------------------------------------

// Motor Driver 
const int AIN1 = 9;
const int AIN2 = 8;
const int BIN1 = 7;
const int BIN2 = 6;
const int STBY = 3;

const int PWMA = 11;
// CRITICAL FIX: Moved PWMB to Pin 5 because Servo.h disables PWM on Pin 10
const int PWMB = 5; 

// Ultrasonic Sensor
const int trigPin = A1;
const int echoPin = A2;
const float soundSpeed = 0.0343;

// Servo Motor
const int servoPin = 2;
const int posLeft = 150;
const int posRight = 30;
const int posCenter = 90;

// ---------------------------------------------------------
// STATE MACHINE & TIMERS
// ---------------------------------------------------------

enum State {
  DRIVING,
  SCANNING,
  REVERSING,
  TURNING
};

State currState = DRIVING;

// Global timer trackers passed by reference to checkTime()
unsigned long pingTimer = 0;
unsigned long scanTimer = 0;
unsigned long maneuverTimer = 0;

// Tracks which step of the servo sweep we are currently executing
int scanStep = 0; 

// Environmental data
float distanceLeft = 0.0;
float distanceRight = 0.0;
float distanceCenter = 0.0;
const float safeDistance = 50.0;

// ---------------------------------------------------------
// MOVEMENT FUNCTIONS
// ---------------------------------------------------------

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

// ---------------------------------------------------------
// SENSOR & TIME LOGIC
// ---------------------------------------------------------

/**
 * @brief Universal non-blocking timer. 
 * @param delayTime The milliseconds to wait.
 * @param tracker The global unsigned long variable tracking this specific event.
 * @return true if the time has elapsed, false otherwise.
 */
bool checkTime(unsigned long delayTime, unsigned long &tracker) {
  if (millis() - tracker >= delayTime) {
    tracker = millis(); // Reset the tracker directly
    return true;
  }
  return false;
}

/**
 * @brief Fires the HC-SR04 and calculates distance.
 * Includes a 24,000µs timeout to prevent the Arduino from freezing.
 */
float getDistance(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  // 24000 timeout equals roughly a 400cm max range
  long duration = pulseIn(echo, HIGH, 24000);
  
  if (duration == 0) return 999.0; // Return a safe large number if no echo (open space)
  
  return (duration * soundSpeed) / 2.0;
}

// ---------------------------------------------------------
// MAIN PROGRAM
// ---------------------------------------------------------

void setup() {
  Serial.begin(9600);

  // Motor Pins
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(PWMB, OUTPUT);

  // Sensor Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialization
  servo.attach(servoPin);
  servo.write(posCenter);
  digitalWrite(STBY, HIGH);
  analogWrite(PWMA, 180);
  analogWrite(PWMB, 180);
}

void loop() {
  switch (currState) {
    
    // ---------------------------------------------------
    // 1. NORMAL DRIVING
    // Drives forward, pinging the center every 50ms.
    // ---------------------------------------------------
    case DRIVING:
      forward();
      
      if (checkTime(50, pingTimer)) {
        distanceCenter = getDistance(trigPin, echoPin);
        
        if (distanceCenter < safeDistance) {
          brake(); 
          currState = SCANNING;
          scanStep = 0; // Reset the scan sequence
        }
      }
      break;

    // ---------------------------------------------------
    // 2. SCANNING (Non-Blocking Sequence)
    // Sweeps the servo and takes readings without delays.
    // ---------------------------------------------------
    case SCANNING:
      // Step 0: Move servo to the left
      if (scanStep == 0) {
        servo.write(posLeft);
        scanTimer = millis(); 
        scanStep++; // Move to next step
      } 
      // Step 1: Wait 500ms for servo to settle, then read Left, move Right
      else if (scanStep == 1 && checkTime(500, scanTimer)) {
        distanceLeft = getDistance(trigPin, echoPin);
        servo.write(posRight);
        scanStep++;
      } 
      // Step 2: Wait 500ms, read Right, move Center
      else if (scanStep == 2 && checkTime(500, scanTimer)) {
        distanceRight = getDistance(trigPin, echoPin);
        servo.write(posCenter);
        scanStep++;
      }
      // Step 3: Wait 250ms, make a decision based on the data
      else if (scanStep == 3 && checkTime(500, scanTimer)) {

        distanceCenter = getDistance(trigPin, echoPin);

        if (distanceCenter > safeDistance) {
          currState = DRIVING;
        }
        
        // Trap check
        else if (distanceLeft < safeDistance && distanceRight < safeDistance) {
          reverse();
          currState = REVERSING;
        } 
        // Escape Left
        else if (distanceLeft > distanceRight) {
          left();
          currState = TURNING;
        } 
        // Escape Right
        else {
          right();
          currState = TURNING;
        }
        
        maneuverTimer = millis(); // Start the clock for the chosen maneuver
      }
      break;

    // ---------------------------------------------------
    // 3. REVERSING 
    // Backs up for 500ms, then forces a spin to escape.
    // ---------------------------------------------------
    case REVERSING:
      if (checkTime(500, maneuverTimer)) {
        right(); // Force a pivot
        currState = TURNING;
        maneuverTimer = millis();
      }
      break;

    // ---------------------------------------------------
    // 4. TURNING 
    // Pivots for 400ms, then stops and re-evaluates.
    // ---------------------------------------------------
    case TURNING:
      if (checkTime(500, maneuverTimer)) {
        brake();
        currState = SCANNING; // Re-scan the new direction to ensure it is actually clear
        scanStep = 0; 
      }
      break;
  }
}