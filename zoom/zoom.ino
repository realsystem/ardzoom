/*
  
 */
#include "bitlash.h"
#include "Ultrasonic.h"
#ifdef SPI_ENABLE
#include <SPI.h>
#endif

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
//the same pins are for two DC motors with reverse and break
int stepperEN = 10;
int stepperPin2 = 5;
int stepperPin7 = 4;
int stepperPin10 = 3;
int stepperPin15 = 2;

//DC motors
int motorsPWM = 250;

#ifdef SPI_ENABLE
int focusSS = 10;
int focusMOSI = 11;
int focusMISO = 12;
int focusSCK = 13;
#endif

int stepperPWM = stepperDefaultPWM;
int zoomPWM = zoomDefaultPWM;
int sensorValue = 0;
int newVal;
int robotDirection = 2;

boolean dir = true, canCheck = true;

//ultrasonic
#define fUltraTrig 13
#define fUltraEcho 12
#define bUltraTrig 7
#define bUltraEcho 8
Ultrasonic fUltrasonic(fUltraTrig, fUltraEcho);
Ultrasonic bUltrasonic(bUltraTrig, bUltraEcho);

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

#ifdef SPI_ENABLE
unsigned int readRegister(byte thisRegister, int bytesToRead ) {
  byte inByte = 0;
  unsigned int result = 0;

  byte dataToSend = thisRegister;
  digitalWrite(focusSS, LOW);
  SPI.transfer(dataToSend);
  result = SPI.transfer(0x00);
  bytesToRead--;
  if (bytesToRead > 0) {
    result = result << 8;
    inByte = SPI.transfer(0x00);
    result = result | inByte;
    bytesToRead--;
  }
  digitalWrite(focusSS, HIGH);
  return(result);
}

void writeRegister(byte thisRegister, byte thisValue) {
  byte dataToSend = thisRegister;

  digitalWrite(focusSS, LOW);

  SPI.transfer(dataToSend);
  SPI.transfer(thisValue);

  digitalWrite(focusSS, HIGH);
}

numvar readReg() {
  byte reg = getarg(1);
  byte cnt = getarg(2);
  Serial.println("arg1:");
  Serial.println(reg);
  Serial.println("arg2:");
  Serial.println(cnt);
  byte val = readRegister(reg, cnt);
  Serial.println("val:");
  Serial.println(val);
}

numvar writeReg() {
  byte reg = getarg(1);
  byte val = getarg(2);
  Serial.println("arg1:");
  Serial.println(reg, BIN);
  Serial.println("arg2:");
  Serial.println(val, BIN);
  writeRegister(reg, val);
}
#endif

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

//DC motors

numvar leftMotorForward() {
  digitalWrite(stepperPin2, 0);
  digitalWrite(stepperPin7, 1);
  delay(30);
  return 0;
}

numvar leftMotorBackward() {
  digitalWrite(stepperPin2, 1);
  digitalWrite(stepperPin7, 0);
  delay(30);
  return 0;
}

numvar leftMotorBreak() {
  delay(30);
  digitalWrite(stepperPin2, 0);
  digitalWrite(stepperPin7, 0);
  return 0;
}

numvar rightMotorForward() {
  digitalWrite(stepperPin10, 0);
  digitalWrite(stepperPin15, 1);
  delay(30);
  return 0;
}

numvar rightMotorBackward() {
  digitalWrite(stepperPin10, 1);
  digitalWrite(stepperPin15, 0);
  delay(30);
  return 0;
}

numvar rightMotorBreak() {
  delay(30);
  digitalWrite(stepperPin10, 0);
  digitalWrite(stepperPin15, 0);
  return 0;
}

float checkUltra(int i) {
  float dist_cm;
  switch (i) {
    case 1:
      dist_cm = fUltrasonic.Ranging(CM);
      if (dist_cm < 3000) {
        Serial.println("forwUltra:");
        Serial.println(dist_cm);
        return dist_cm;
      }
      break;
    case 2:
      dist_cm = bUltrasonic.Ranging(CM);
      if (dist_cm < 3000) {
        Serial.println("backUltra:");
        Serial.println(dist_cm);
        return dist_cm;
      }
      break;
    default:
      Serial.println("Ultra error");
      return -1;
  }
}

#define ms_div 200
#define ultraZone 15

void my_delay(int ms, int u) {
  int ms_tmp = ms/ms_div;
  int i;
  Serial.println("ms_tmp:");
  Serial.println(ms_tmp);
  for (i = 0; i < ms_div; i++) {
    if (checkUltra(u) > ultraZone) delay(ms_tmp);
    else break;
  }
}

numvar motorRun() {
  Serial.println("RUN");
  if ((checkUltra(1) > ultraZone) && (checkUltra(2) > ultraZone)) {
    analogWrite(stepperEN, motorsPWM);
  
    switch (robotDirection) {
      case 3:
        leftMotorBackward();
        rightMotorBackward();
        delay(100);
        leftMotorBreak();
        rightMotorBreak();
        robotDirection = 1;
        break;
      case 2:
        leftMotorForward();
        rightMotorForward();
        my_delay(10000, 1);
        leftMotorBreak();
        rightMotorBreak();
        robotDirection = 3;
        break;
      case 1:
        leftMotorForward();
        rightMotorBackward();
        delay(500);
        leftMotorBreak();
        rightMotorBreak();
        robotDirection = 2;
        break;
    }
   
    analogWrite(stepperEN, 0);
  }
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
  
  addBitlashFunction("motors", (bitlash_function) motorRun);
  
#ifdef SPI_ENABLE
  addBitlashFunction("wr", (bitlash_function) writeReg);
  addBitlashFunction("rr", (bitlash_function) readReg);
  SPI.begin();
#endif
} 

void loop()  {
  runBitlash();
  /*checkUltra(1);
  delay(1000);
  checkUltra(2);
  delay(1000);*/
  motorRun();
  //delay(5000);
  //if (canCheck) checkSensor();
}


