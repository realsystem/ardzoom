#include <looper.h>
#include <bitlash.h>
#include <Ultrasonic.h>


//general defs
#define serialPortSpeed 115200

//DC motors defs
#define motorsDefaultPWM 250
#define motorsEN 10
#define motorsPin2 5
#define motorsPin7 4
#define motorsPin10 3
#define motorsPin15 2

//robot defs
#define goForward 2
#define goBackward 3
#define turnRight 4
#define turnLeft 5
#define robotDefaultDirection goForward
#define goInterval 70

//ultrasonic defs
#define fUltraTrig 13
#define fUltraEcho 12
#define bUltraTrig 7
#define bUltraEcho 8
#define ultraMaxRange 2000
#define checkUltraInterval 50
#define ultraForward 1
#define ultraBackward 2
#define ultraZone 20

//DC motors vars
int motorsPWM = motorsDefaultPWM;

//robot vars
int robotNextDirection = robotDefaultDirection, avoidObstacle = 0;
boolean blockForward = false, blockBackward = false;
boolean ultraBlockForward = false, ultraBlockBackward = false;

//ultrasonic obj
Ultrasonic fUltrasonic(fUltraTrig, fUltraEcho);
Ultrasonic bUltrasonic(bUltraTrig, bUltraEcho);

//looper obj
looper myScheduler;

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
  digitalWrite(motorsPin2, 0);
  digitalWrite(motorsPin7, 0);
  delay(30);
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
  digitalWrite(motorsPin10, 0);
  digitalWrite(motorsPin15, 0);
  delay(30);
  return 0;
}

//Ultrasonic functions
float checkUltra(int ultraNum) {
  float dist_cm = ultraMaxRange;
//  Serial.println("checkUltra");
  switch (ultraNum) {
  case ultraForward:
    dist_cm = fUltrasonic.Ranging(CM);
//    Serial.println("ultraForward");
//    Serial.print("dist_cm=");
//    Serial.println(dist_cm, 1);
    break;
  case ultraBackward:
    dist_cm = bUltrasonic.Ranging(CM);
//    Serial.println("ultraBackward");
//    Serial.print("dist_cm=");
//    Serial.println(dist_cm, 1);
    break;
  default:
    return -1;
  }
  return dist_cm;
}

void checkUltraProc(void) {
//  Serial.println("checkUltraProc");
  if (checkUltra(ultraForward) <= ultraZone) {
    ultraBlockForward = true;
//    Serial.println("ultraBlockForward = true");
  }
  else ultraBlockForward = false;
  if (checkUltra(ultraBackward) <= ultraZone) {
    ultraBlockBackward = true;
//    Serial.println("ultraBlockBackward = true");
  }
  else ultraBlockBackward = false;
}

void goSlightlyBackward(int backWardDelay) {
  leftMotorBackward();
  rightMotorBackward();
  delay(backWardDelay);
  leftMotorBreak();
  rightMotorBreak();
}

//main
void motorRun(void) {
//  Serial.println("motorRun");
  if ((!blockForward) && (!blockBackward)) {
    analogWrite(motorsEN, motorsPWM);
    switch (robotNextDirection) {
    case turnRight:
      goSlightlyBackward(random(50, 200));
      rightMotorForward();
      leftMotorBackward();
      delay(random(30, 100));
      leftMotorBreak();
      rightMotorBreak();
      robotNextDirection = goForward;
      break;
    case goBackward:
//      Serial.println("goBackward");
      if (!ultraBlockBackward) {
//        Serial.println("motorsBackward");
        leftMotorBackward();
        rightMotorBackward();
      }
      else {
//        Serial.println("motorsBreak");
        leftMotorBreak();
        rightMotorBreak();
        robotNextDirection = goForward;
      }
      break;
    case goForward:
//      Serial.println("goForward");
      if (!ultraBlockForward) {
//        Serial.println("motorsForward");
        leftMotorForward();
        rightMotorForward();
      }
      else {
//        Serial.println("motorsBreak");
        leftMotorBreak();
        rightMotorBreak();
        robotNextDirection = random(turnRight, turnLeft + 1);
      }
      break;
    case turnLeft:
      goSlightlyBackward(random(50, 200));
      leftMotorForward();
      rightMotorBackward();
      delay(random(30, 100));
      leftMotorBreak();
      rightMotorBreak();
      robotNextDirection = goForward;
      break;
    }
  }
  else {
    analogWrite(motorsEN, 0);//TODO: add special rule to disable motors
  }
//  Serial.println("--");
}

void setup()  {
  initBitlash(serialPortSpeed);
  pinMode(motorsEN, OUTPUT);
  pinMode(motorsPin2, OUTPUT);
  pinMode(motorsPin7, OUTPUT);
  pinMode(motorsPin10, OUTPUT);
  pinMode(motorsPin15, OUTPUT);
  randomSeed(analogRead(0));
  addBitlashFunction("m", (bitlash_function) motorRun);
  myScheduler.addTask(checkUltraProc, checkUltraInterval);
  myScheduler.addTask(motorRun, goInterval);
} 

void loop()  {
  runBitlash();
  myScheduler.scheduler();
  //motorRun();
}



