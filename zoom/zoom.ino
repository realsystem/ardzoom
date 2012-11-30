/*
  
 */
#include "bitlash.h"

#define arSize 17
#define minVal 805
#define maxVal 1000
#define zoomStep 1000
#define zoomDefaultPWM 120
#define stepperStep 4
#define stepperStepDelay 30
#define stepperDefaultPWM 35

//PINs for DC motor on zooming
#define zoomPin 9
#define zoomPin1 8
#define zoomPin2 7
#define sensorPin A1

//PINs for focus sensor
#define focusSensorPower 11
#define focusSensor 2

//PINs for Stepper motor on autofocus
int stepperEN = 10;
int stepperPin2 = 5;
int stepperPin7 = 4;
int stepperPin10 = 3;
int stepperPin15 = 2;

int stepperPWM = stepperDefaultPWM;
int zoomPWM = zoomDefaultPWM;
int sensorValue = 0;
int newVal;

boolean dir = true, canCheck = true;

//zoom
numvar zoomIN(int val) {
  digitalWrite(zoomPin2, 1);
  digitalWrite(zoomPin1, 0);
  analogWrite(zoomPin, val);
  return 0;
}

numvar zoomOUT(int val) {
  digitalWrite(zoomPin2, 0);
  digitalWrite(zoomPin1, 1);
  analogWrite(zoomPin, val);
  return 0;
}

numvar zoomOFF() {
  digitalWrite(zoomPin2, 1);
  digitalWrite(zoomPin1, 1);
  analogWrite(zoomPin, 0);
  return 0;
}

numvar zoomRev() {
  zoomOFF();
  if (dir)
    dir = false;
  else
    dir = true;
  return 0;
}

numvar zoomON() {
  if (dir) zoomIN(zoomPWM);
  else zoomOUT(zoomPWM);
  return 0;
}

void sort(int *a, int n) {
  for (int i = 1; i < n; ++i) {
    int j = a[i];
    int k;
    for (k = i - 1; (k >= 0) && (j < a[k]); k--) {
      a[k + 1] = a[k];
    }
    a[k + 1] = j;
  }
}

void printArray(int *a, int n) {
  for (int i = 0; i < n; i++) {
    Serial.print(a[i], DEC);
    Serial.print(' ');
  }
  Serial.println();
}

numvar checkSensor() {
  int values[arSize], i, mid;
  for (i = 0; i < arSize; i++) {
    values[i] = analogRead(sensorPin);
    delay(10);
  }
  //printArray(values, arSize);
  sort(values, arSize);
  //printArray(values, arSize);
  mid = arSize/2;
  //Serial.println("new:");
  newVal = values[mid];
  //Serial.println(newVal);
  if ((newVal <= minVal) || (newVal >= maxVal)) {
    //Serial.println("stop");
    zoomOFF();
  }
  return 0;
}

numvar setPWM(int val) {
  zoomPWM = val;
  return 0;
}

numvar upPWM() {
  zoomPWM +=10;
  return 0;
}

numvar downPWM() {
  zoomPWM -=10;
  return 0;
}

numvar zoomLeft() {
  canCheck = false;
  zoomIN(zoomPWM);
  delay(500);
  canCheck = true;
  return 0;
}

numvar zoomRight() {
  canCheck = false;
  zoomOUT(zoomPWM);
  delay(500);
  canCheck = true;
  return 0;
}

numvar zoomStepLeft() {
  canCheck = false;
  zoomIN(zoomPWM);
  delay(zoomStep);
  zoomOFF();
  canCheck = true;
  return 0;
}

numvar zoomStepRight() {
  canCheck = false;
  zoomOUT(zoomPWM);
  delay(zoomStep);
  zoomOFF();
  canCheck = true;
  return 0;
}

numvar stepperOff() {
  analogWrite(stepperEN, 0);
  return 0;
}

numvar stepperLeft() {
  analogWrite(stepperEN, stepperPWM);
  //for (int i = 0; i < 19; i++) {
    digitalWrite(stepperPin2, 0);
    digitalWrite(stepperPin7, 1);
    digitalWrite(stepperPin10, 0);
    digitalWrite(stepperPin15, 1);
    delay(10);
    digitalWrite(stepperPin2, 1);
    digitalWrite(stepperPin7, 0);
    digitalWrite(stepperPin10, 0);
    digitalWrite(stepperPin15, 1);
    delay(10);
    digitalWrite(stepperPin2, 1);
    digitalWrite(stepperPin7, 0);
    digitalWrite(stepperPin10, 1);
    digitalWrite(stepperPin15, 0);
    delay(10);
    digitalWrite(stepperPin2, 0);
    digitalWrite(stepperPin7, 1);
    digitalWrite(stepperPin10, 1);
    digitalWrite(stepperPin15, 0);
    delay(10);
  //}
  analogWrite(stepperEN, 0);
  return 0;
}

numvar stepperRight() {
  analogWrite(stepperEN, stepperPWM);
  //for (int i = 0; i < 19; i++) {
    digitalWrite(stepperPin2, 0);
    digitalWrite(stepperPin7, 1);
    digitalWrite(stepperPin10, 0);
    digitalWrite(stepperPin15, 1);
    delay(10);
    digitalWrite(stepperPin2, 0);
    digitalWrite(stepperPin7, 1);
    digitalWrite(stepperPin10, 1);
    digitalWrite(stepperPin15, 0);
    delay(10);
    digitalWrite(stepperPin2, 1);
    digitalWrite(stepperPin7, 0);
    digitalWrite(stepperPin10, 1);
    digitalWrite(stepperPin15, 0);
    delay(10);
    digitalWrite(stepperPin2, 1);
    digitalWrite(stepperPin7, 0);
    digitalWrite(stepperPin10, 0);
    digitalWrite(stepperPin15, 1);
    delay(10);
  //}
  analogWrite(stepperEN, 0);
  return 0;
}

numvar readFocusSensor() {
  digitalWrite(focusSensorPower, 1);
  boolean val = digitalRead(focusSensor);
  digitalWrite(focusSensorPower, 0);
  Serial.println("reading focus sensor:");
  Serial.println(val);
  return 0;
}

void setup()  { 
  //pinMode(zoomPin, OUTPUT); 
  //pinMode(zoomPin1, OUTPUT); 
  //pinMode(zoomPin2, OUTPUT);
  
  pinMode(stepperEN, OUTPUT);
  pinMode(stepperPin2, OUTPUT);
  pinMode(stepperPin7, OUTPUT);
  pinMode(stepperPin10, OUTPUT);
  pinMode(stepperPin15, OUTPUT);
  
  //pinMode(focusSensorPower, OUTPUT);
  //pinMode(focusSensor, INPUT);
  
  initBitlash(115200);
  addBitlashFunction("zon", (bitlash_function) zoomON);
  addBitlashFunction("zoff", (bitlash_function) zoomOFF);
  addBitlashFunction("check", (bitlash_function) checkSensor);
  addBitlashFunction("zrev", (bitlash_function) zoomRev);
  addBitlashFunction("up", (bitlash_function) upPWM);
  addBitlashFunction("down", (bitlash_function) downPWM);
  addBitlashFunction("left", (bitlash_function) zoomLeft);
  addBitlashFunction("right", (bitlash_function) zoomRight);
  addBitlashFunction("sl", (bitlash_function) zoomStepLeft);
  addBitlashFunction("sr", (bitlash_function) zoomStepRight);
  
  addBitlashFunction("stl", (bitlash_function) stepperLeft);
  addBitlashFunction("str", (bitlash_function) stepperRight);
  addBitlashFunction("sto", (bitlash_function) stepperOff);
  
  addBitlashFunction("rdf", (bitlash_function) readFocusSensor);
} 

void loop()  {
  runBitlash();
  //if (canCheck) checkSensor();
}


