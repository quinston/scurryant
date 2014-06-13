#include <AFMotor.h>

int lastReading = 0;
/* Readings below this mean that the robot should react*/
int reactionCeiling = 0;

/* Sonic sensor pins */
int sonicTrig = 3, sonicGnd = 4, sonic5V = 5;

/* Motor pins */
int motorLR = 8, motorLB = 9, motorRR = 10, motorRB = 11;

AF_DCMotor* wheelL, *wheelR;

void setup() {
  Serial.begin(9600);
  
  
  pinMode(sonic5V, OUTPUT);
  pinMode(sonicGnd, OUTPUT);
  pinMode(sonicTrig, INPUT);
  
  pinMode(motorLR, OUTPUT);
    pinMode(motorLB, OUTPUT);
      pinMode(motorRR, OUTPUT);
        pinMode(motorRB, OUTPUT);
  
  wheelL = new AF_DCMotor(2, MOTOR12_1KHZ);
  
  wheelL->setSpeed(200);
  
}

int timeToCm(int t) {
  //http://www.crcibernetica.com/ultrasonic-range-finder-sdm-io/
  return (t*340.23/2/10000)-3;
}

/* Returns distance of remote obstacle in cm */
int echolocate() {
  
  /* Power the pins */
  digitalWrite(sonic5V, HIGH);
  digitalWrite(sonicGnd, LOW);
  
  
  /* Do some location */
  pinMode(sonicTrig, OUTPUT);
  digitalWrite(sonicTrig, LOW);
  delay(25);
  digitalWrite(sonicTrig, HIGH);
  int time = micros();
  pinMode(sonicTrig, INPUT);
  int pulseSize = pulseIn(sonicTrig, LOW, 18000); //expect 15us, timeout 18ms
  time = micros() - time - pulseSize; // time since sending the pulse and receiving the whole pulse
  
  return timeToCm(time);  
}

void calibrate(int numSamples) {
      int d = 0;
      for (int i = 0; i  != numSamples; ++i) {
        d += echolocate();
      }
      reactionCeiling = d / numSamples;
}

void scram(int ms) {
  wheelL->run(FORWARD);
  delay(ms);
  wheelL->run(RELEASE);
  
}

void loop() {
  int a  = echolocate();
  Serial.println(a);

  if (a < reactionCeiling) {
    Serial.println("ALERT!");
    scram(1000);
  }

  if (Serial.available()) {
    int c = Serial.read();
    /* Calibrate! */
    if (c == 'c') {
      calibrate(10);
    }
  }

  delay(50);
}

