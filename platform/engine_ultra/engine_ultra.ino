/*
  
 */
#include "bitlash.h"
#include "Ultrasonic.h"

//PINs for motors with reverse and break
#define motorsDefaultPWM 250
#define motorsEN 10
#define motorsPin2 5
#define motorsPin7 4
#define motorsPin10 3
#define motorsPin15 2

//robot defs
#define ms_div 10
#define ultraZone 25
#define robotDefaultDirection 2

//ultrasonic defs
#define fUltraTrig 13
#define fUltraEcho 12
#define bUltraTrig 7
#define bUltraEcho 8

//DC motors PWM
int motorsPWM = motorsDefaultPWM;

//robot vars
int robotDirection = robotDefaultDirection, avoidObstacle = 0;

//ultrasonic obj
Ultrasonic fUltrasonic(fUltraTrig, fUltraEcho);
Ultrasonic bUltrasonic(bUltraTrig, bUltraEcho);

//DC motors functions
numvar leftMotorForward() {
  digitalWrite(motorsPin2, 0);
  digitalWrite(motorsPin7, 1);
  delay(30);
  return 0;
}

numvar leftMotorBackward() {
  digitalWrite(motorsPin2, 1);
  digitalWrite(motorsPin7, 0);
  delay(30);
  return 0;
}

numvar leftMotorBreak() {
  delay(30);
  digitalWrite(motorsPin2, 0);
  digitalWrite(motorsPin7, 0);
  return 0;
}

numvar rightMotorForward() {
  digitalWrite(motorsPin10, 0);
  digitalWrite(motorsPin15, 1);
  delay(30);
  return 0;
}

numvar rightMotorBackward() {
  digitalWrite(motorsPin10, 1);
  digitalWrite(motorsPin15, 0);
  delay(30);
  return 0;
}

numvar rightMotorBreak() {
  delay(30);
  digitalWrite(motorsPin10, 0);
  digitalWrite(motorsPin15, 0);
  return 0;
}

//Ultrasonic functions
float checkUltra(int i) {
  float dist_cm = 2000;
  switch (i) {
    case 1:
      dist_cm = fUltrasonic.Ranging(CM);
      break;
    case 2:
      dist_cm = bUltrasonic.Ranging(CM);
      break;
    default:
      return -1;
  }
  return dist_cm;
}

boolean my_delay(int ms, int u) {
  int i, ms_tmp = ms/ms_div;
  for (i = 0; i < ms_tmp; i++) {
    if (checkUltra(u) > ultraZone) delay(ms_div);
    else return false;
  }
  return true;
}

//main
numvar motorRun() {
  if ((checkUltra(1) > ultraZone) && (checkUltra(2) > ultraZone)) {
    analogWrite(motorsEN, motorsPWM);
    switch (robotDirection) {
      //turn slightly right
      case 4:
        leftMotorBackward();
        rightMotorForward();
        delay(random(100, 150));
        leftMotorBreak();
        rightMotorBreak();
        robotDirection = 2;
        break;
      //go slightly back
      case 3:
        leftMotorBackward();
        rightMotorBackward();
        my_delay(100, 2);
        leftMotorBreak();
        rightMotorBreak();
        robotDirection = 5;//random(4,6);
        break;
      //go go go
      case 2:
        leftMotorForward();
        rightMotorForward();
        if (my_delay(1000, 1)) {
          robotDirection = 2;
          break;
        }
        leftMotorBreak();
        rightMotorBreak();
        robotDirection = 3;
        break;
      //turn slightly left
      case 5:
        leftMotorForward();
        rightMotorBackward();
        delay(100);
        leftMotorBreak();
        rightMotorBreak();
        robotDirection = 2;
        break;
    }
    analogWrite(motorsEN, 0);
  }
  return 0;
}

void setup()  {
  initBitlash(115200);
  pinMode(motorsEN, OUTPUT);
  pinMode(motorsPin2, OUTPUT);
  pinMode(motorsPin7, OUTPUT);
  pinMode(motorsPin10, OUTPUT);
  pinMode(motorsPin15, OUTPUT);
  addBitlashFunction("motors", (bitlash_function) motorRun);
} 

void loop()  {
  runBitlash();
  motorRun();
}


