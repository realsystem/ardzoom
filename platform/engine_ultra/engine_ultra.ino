#include <AFMotor.h>
#include <looper.h>
#include <bitlash.h>
#include <Ultrasonic.h>


//general defs
#define serialPortSpeed 115200

//DC motors defs
#define motorsDefaultPWM 0
#define motorsEN 10
#define motorsPin2 3
#define motorsPin7 2
#define motorsPin10 5
#define motorsPin15 4

//robot defs
#define thinking 1
#define goForward 2
#define goBackward 3
#define turnRight 4
#define turnLeft 5
#define robotDefaultDirection goForward
#define goInterval 70
#define turnDelayDefault 200

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
int motorsPWM = motorsDefaultPWM, motorsSpeedPrevious = 0;

//robot vars
int robotPrevDirection = robotDefaultDirection, robotNextDirection = robotDefaultDirection, avoidObstacle = 0;
int turnLeftCounter = 0, turnRightCounter = 0, turnRightCounter2 = 0, turnReverseLeftCounter = 0, turnReverseRightCounter = 0;
boolean blockForward = false, blockBackward = false, globalBlockOper = false;
boolean ultraBlockForward = false, ultraBlockBackward = false;
float currentUltraForward = ultraMaxRange, currentUltraBackward = ultraMaxRange;
int ultraSector[3];
unsigned long curMicrosUltraProc, curMicrosTurnRobot, curMicrosRandomRun, globalNoOperCounter = 0;

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
}

numvar leftMotorBackward() {
  digitalWrite(motorsPin2, 1);
  digitalWrite(motorsPin7, 0);
  delay(30);
}

numvar leftMotorBreak() {
  digitalWrite(motorsPin2, 0);
  digitalWrite(motorsPin7, 0);
  delay(30);
}

numvar rightMotorForward() {
  digitalWrite(motorsPin10, 0);
  digitalWrite(motorsPin15, 1);
  delay(30);
}

numvar rightMotorBackward() {
  digitalWrite(motorsPin10, 1);
  digitalWrite(motorsPin15, 0);
  delay(30);
}

numvar rightMotorBreak() {
  digitalWrite(motorsPin10, 0);
  digitalWrite(motorsPin15, 0);
  delay(30);
}

void motorsBreak(void) {
  leftMotorBreak();
  rightMotorBreak();
}

void goSlightlyBackward(int backwardDelay) {
  leftMotorBackward();
  rightMotorBackward();
  delay(backwardDelay);
  motorsBreak();
}

void motorsSpeed(int spd) {
  if (spd != 0) {
    //for (motorsPWM = motorsSpeedPrevious; motorsPWM <= spd; motorsPWM ++) {
      analogWrite(motorsEN, motorsPWM);
    //}
    }
    else {
      //for (motorsPWM = motorsDefaultPWM; motorsPWM >= motorsSpeedPrevious; motorsPWM --) {
        analogWrite(motorsEN, 0);
      //}
    //motorsSpeedPrevious = spd;
  }
}

//Ultrasonic functions
float checkUltra(int ultraNum) {
  float dist_cm = ultraMaxRange;
//  Serial.println("checkUltra");
  switch (ultraNum) {
  case ultraForward:
    dist_cm = fUltrasonic.Ranging(CM);
    /*Serial.println("ultraForward");
    Serial.print("dist_cm=");
    Serial.println(dist_cm, 1);*/
    break;
  case ultraBackward:
    dist_cm = bUltrasonic.Ranging(CM);
    /*Serial.println("ultraBackward");
    Serial.print("dist_cm=");
    Serial.println(dist_cm, 1);*/
    break;
  default:
    return -1;
  }
  return dist_cm;
}

void checkUltraProc(void) {
  unsigned long tmp = micros();
  currentUltraForward = checkUltra(ultraForward);
  currentUltraBackward = checkUltra(ultraBackward);
  if (currentUltraForward <= ultraZone) {
    ultraBlockForward = true;
  }
  else ultraBlockForward = false;
  if (currentUltraBackward <= ultraZone) {
    ultraBlockBackward = true;
  }
  else ultraBlockBackward = false;
  curMicrosUltraProc = micros() - tmp;
}

int getMax(int * arr) {
  int tmp = arr[0], j = 0, i = 0;
  /*Serial.print("arr[");
  Serial.print(i);
  Serial.print("]=");
  Serial.println(arr[i]);*/
  for (i = 1; i <= sizeof(arr); i++) {
    /*Serial.print("arr[");
    Serial.print(i);
    Serial.print("]=");
    Serial.println(arr[i]);*/
    if (arr[i] > tmp) {
      tmp = arr[i];
      j = i;
    }
  }
  //Serial.print("getMax=");
  //Serial.println(j);
  return j;
}

//turn to direction without walking
void turnRobot(int turnDirection) {
  unsigned long tmp = micros();
  switch (turnDirection) {
    case turnRight:
      leftMotorForward();
      rightMotorBackward();
      break;
    case turnLeft:
      rightMotorForward();
      leftMotorBackward();
      break;
  }
  curMicrosTurnRobot = micros() - tmp;
}

//avoiding
void avoidRun(void) {
  Serial.println("=================================================");
  Serial.println("avoidRun");
  if (!globalBlockOper) {
    Serial.println("!globalBlockOper");
  if ((!blockForward) && (!blockBackward)) {
    motorsSpeed(motorsDefaultPWM);
    switch (robotNextDirection) {
    case turnRight:
    Serial.println("turnRight");
      if (turnRightCounter2 == 1) {
        Serial.println("turnRightCounter2 == 1");
        robotNextDirection = goForward;
        turnRightCounter2 = 0;
      }
      if ((turnRightCounter == 1) && (turnRightCounter2 == 0)) {
        Serial.println("(turnRightCounter == 1) && (turnRightCounter2 == 0)");
        turnRightCounter = 0;
        ultraSector[2] = currentUltraForward;
        Serial.print("ultraSector[2]=");
        Serial.println(ultraSector[2]);
        robotPrevDirection = robotNextDirection;
        robotNextDirection = turnLeft;
        break;
      }
      if (robotPrevDirection == thinking) {
        Serial.println("robotPrevDirection == thinking");
        Serial.println("turnRobot");
        turnRobot(robotNextDirection);
        globalBlockOper = true;
        robotPrevDirection = robotNextDirection;
        turnRightCounter2 ++;
        break;
      }
      Serial.println("turnRobot");
      turnRobot(robotNextDirection);
      globalBlockOper = true;
      if ((robotPrevDirection != thinking) && (turnRightCounter2 == 0)) {
        turnRightCounter ++;
      }
      break;
    case goBackward:
      break;
    case goForward:
    Serial.println("goForward");
      if (!ultraBlockForward) {
        Serial.println("!ultraBlockForward");
        leftMotorForward();
        rightMotorForward();
      }
      else {
        Serial.println("ultraBlockForward");
        motorsBreak();
        robotPrevDirection = robotNextDirection;
        ultraSector[1] = currentUltraForward;
        Serial.print("ultraSector[1]=");
        Serial.println(ultraSector[1]);
        robotNextDirection = turnRight;
      }
      break;
    case turnLeft:
    Serial.println("turnLeft");
      if (turnLeftCounter == 1) {
        Serial.println("turnLeftCounter == 1");
        ultraSector[0] = currentUltraForward;
        Serial.print("ultraSector[0]=");
        Serial.println(ultraSector[0]);
        turnLeftCounter = 0;
        robotNextDirection = thinking;
        break;
      }
      if (robotPrevDirection == turnRight) {
        Serial.println("robotPrevDirection == turnRight");
        Serial.println("turnRobot");
        turnRobot(robotNextDirection);
        globalBlockOper = true;
        robotPrevDirection = robotNextDirection;
        break;
      }
      if (turnReverseLeftCounter == 3) {
        Serial.println("turnReverseLeftCounter == 3");
        robotNextDirection = goForward;
        turnReverseLeftCounter = 0;
        break;
      }
      Serial.println("turnRobot");
      turnRobot(robotNextDirection);
      globalBlockOper = true;
      if (robotPrevDirection != thinking) {
        turnLeftCounter ++;
      }
      else turnReverseLeftCounter ++;
      break;
    case thinking:
    Serial.println("thinking");
      robotPrevDirection = robotNextDirection;
      switch(getMax(ultraSector)) {
      case 0:
      Serial.println("0");
        robotNextDirection = goForward;
        break;
      case 1:
      Serial.println("1");
        robotNextDirection = turnLeft;//reverse
        break;
      case 2:
      Serial.println("2");
        robotNextDirection = turnRight;
        break;
      }
      break;
    }
  }
  else {
    motorsSpeed(0);
  }
  }
  else {
    Serial.println("globalNoOperCounter");
    if (globalNoOperCounter >= 1) {
      Serial.println("globalNoOperCounter >= 1");
      globalNoOperCounter = 0;
      globalBlockOper = false;
    }
    globalNoOperCounter ++;
  }
}

//main random walking
void randomRun(void) {
  unsigned long tmp = micros();
  if (!globalBlockOper) {
  if ((!blockForward) && (!blockBackward)) {
    motorsSpeed(motorsDefaultPWM);
    switch (robotNextDirection) {
    case turnRight:
      /*if (turnRightCounter > random(4, 9)) {
        turnRightCounter = 0;
        switch (robotPrevDirection) {
          case goForward:
            robotNextDirection = goBackward;
            break;
          case goBackward:
            robotNextDirection = goForward;
            break;
        }
        break;
      }*/
      //goSlightlyBackward(random(30, 70));
      turnRobot(robotNextDirection);
      globalBlockOper = true;
      robotNextDirection = goForward;
      turnRightCounter ++;
      break;
    case goBackward:
      if (!ultraBlockBackward) {
        leftMotorBackward();
        rightMotorBackward();
      }
      else {
        motorsBreak();
        robotPrevDirection = robotNextDirection;
        robotNextDirection = random(turnRight, turnLeft + 1);
      }
      break;
    case goForward:
      if (!ultraBlockForward) {
        leftMotorForward();
        rightMotorForward();
      }
      else {
        motorsBreak();
        robotPrevDirection = robotNextDirection;
        robotNextDirection = random(turnRight, turnLeft + 1);
      }
      break;
    case turnLeft:
      /*if (turnLeftCounter > random(4, 9)) {
        turnLeftCounter = 0;
        switch (robotPrevDirection) {
          case goForward:
            robotNextDirection = goBackward;
            break;
          case goBackward:
            robotNextDirection = goForward;
            break;
        }
        break;
      }*/
      //goSlightlyBackward(random(30, 70));
      turnRobot(robotNextDirection);
      globalBlockOper = true;
      robotNextDirection = goForward;
      turnLeftCounter ++;
      break;
    }
  }
  else {
    motorsSpeed(0);
  }
  }
  else {
    if (globalNoOperCounter >= 1) {
      globalNoOperCounter = 0;
      globalBlockOper = false;
    }
    globalNoOperCounter ++;
  }
  curMicrosRandomRun = micros() - tmp;
}

numvar listMicros(void) {
  Serial.print("curMicrosUltraProc=");
  Serial.println(curMicrosUltraProc);
  Serial.print("curMicrosTurnRobot=");
  Serial.println(curMicrosTurnRobot);
  Serial.print("curMicrosRandomRun=");
  Serial.println(curMicrosRandomRun);
}

void setup()  {
  initBitlash(serialPortSpeed);
  pinMode(motorsEN, OUTPUT);
  pinMode(motorsPin2, OUTPUT);
  pinMode(motorsPin7, OUTPUT);
  pinMode(motorsPin10, OUTPUT);
  pinMode(motorsPin15, OUTPUT);
  randomSeed(analogRead(0));
  addBitlashFunction("lst", (bitlash_function) listMicros);
  addBitlashFunction("av", (bitlash_function) avoidRun);
  myScheduler.addTask(checkUltraProc, checkUltraInterval);
  //myScheduler.addTask(randomRun, goInterval);
  //myScheduler.addTask(avoidRun, goInterval);
} 

void loop()  {
  runBitlash();
  myScheduler.scheduler();
}



