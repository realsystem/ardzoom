#include <EEPROM.h>
#include <looper.h>
#include <bitlash.h>
#include <Ultrasonic.h>


//general defs
#define serialPortSpeed 115200

//DC motors defs
#define motorsDefaultPWM 255
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
#define goInterval 35
#define turnDelayDefault 200

//ultrasonic defs
#define fUltraTrig 13
#define fUltraEcho 12
#define bUltraTrig 7
#define bUltraEcho 8
#define ultraMaxRange 2000
#define checkUltraInterval 10
#define ultraForward 1
#define ultraBackward 2
#define ultraZone 17

//DC motors vars
int motorsPWM = motorsDefaultPWM, motorsSpeedPrevious = 0;

//robot vars
int robotPrevDirection = robotDefaultDirection, robotNextDirection = robotDefaultDirection, avoidObstacle = 0;
int turnLeftCounter = 0, turnRightCounter = 0, turnRightCounter2 = 0, turnReverseLeftCounter = 0, turnReverseRightCounter = 0;
boolean blockForward = false, blockBackward = false, globalBlockOper = false;
boolean ultraBlockForward = false, ultraBlockBackward = false;
float currentUltraForward = ultraMaxRange, currentUltraBackward = ultraMaxRange;
int ultraSector[3], maxUltraSector, logAddr = 0;
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
  for (i = 1; i <= sizeof(arr); i++) {
    if (arr[i] > tmp) {
      tmp = arr[i];
      j = i;
    }
  }
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
  if ((!blockForward) && (!blockBackward)) {
    motorsSpeed(motorsDefaultPWM);
    if (!globalBlockOper) {
      switch (robotNextDirection) {
      case turnRight:
        if (turnRightCounter2 == 1) {
          robotNextDirection = goForward;
          turnRightCounter2 = 0;
        }
        if ((turnRightCounter == 1) && (turnRightCounter2 == 0)) {
          turnRightCounter = 0;
          ultraSector[2] = currentUltraForward;
          robotPrevDirection = robotNextDirection;
          robotNextDirection = turnLeft;
          break;
        }
        if (robotPrevDirection == thinking) {
          turnRobot(robotNextDirection);
          globalBlockOper = true;
          robotPrevDirection = robotNextDirection;
          turnRightCounter2 ++;
          break;
        }
        turnRobot(robotNextDirection);
        globalBlockOper = true;
        if ((robotPrevDirection != thinking) && (turnRightCounter2 == 0)) {
          turnRightCounter ++;
        }
        break;
      case goBackward:
        break;
      case goForward:
        if (!ultraBlockForward) {
          leftMotorForward();
          rightMotorForward();
        }
        else {
          motorsBreak();
          robotPrevDirection = robotNextDirection;
          ultraSector[1] = currentUltraForward;
          robotNextDirection = turnRight;
        }
        break;
      case turnLeft:
        if (turnLeftCounter == 1) {
          ultraSector[0] = currentUltraForward;
          turnLeftCounter = 0;
          robotNextDirection = thinking;
          break;
        }
        if (robotPrevDirection == turnRight) {
          turnRobot(robotNextDirection);
          globalBlockOper = true;
          robotPrevDirection = robotNextDirection;
          break;
        }
        if (turnReverseLeftCounter == 2) {
          robotNextDirection = goForward;
          turnReverseLeftCounter = 0;
          break;
        }
        turnRobot(robotNextDirection);
        globalBlockOper = true;
        if (robotPrevDirection != thinking) {
          turnLeftCounter ++;
        }
        else turnReverseLeftCounter ++;
        break;
      case thinking:
        robotPrevDirection = robotNextDirection;
        maxUltraSector = getMax(ultraSector);
        /*EEPROM.write(logAddr, maxUltraSector);
        if (logAddr < 1024) logAddr ++;
        else logAddr = 0;*/
        switch(maxUltraSector) {
        case 0:
          robotNextDirection = goForward;
          break;
        case 1:
          robotNextDirection = turnLeft;//reverse
          break;
        case 2:
          robotNextDirection = turnRight;
          break;
        }
        break;
      }
    }
    else {
      //if (globalNoOperCounter >= 1) {
        //Serial.println("globalNoOperCounter >= 1");
        //globalNoOperCounter = 0;
        globalBlockOper = false;
      //}
      //globalNoOperCounter ++;
    }
  }
  else {
    motorsSpeed(0);
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
      if (turnRightCounter > random(4, 9)) {
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
      }
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
      if (turnLeftCounter > random(4, 9)) {
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
      }
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
  /*int i, tmp_val;
  Serial.print("EEPROM:");
  for (i = 0; i < 128; i ++) {
    tmp_val = EEPROM.read(i);
    Serial.print(tmp_val);
    Serial.print(" ");
  }
  Serial.println("");*/
  Serial.print("curMicrosUltraProc=");
  Serial.println(curMicrosUltraProc);
  Serial.print("curMicrosTurnRobot=");
  Serial.println(curMicrosTurnRobot);
  Serial.print("curMicrosRandomRun=");
  Serial.println(curMicrosRandomRun);
}

numvar pauseRun() {
  if (blockForward) blockForward = false;
  else blockForward = true;
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
  addBitlashFunction("pause", (bitlash_function) pauseRun);
  myScheduler.addTask(checkUltraProc, checkUltraInterval);
  //myScheduler.addTask(randomRun, goInterval*2);
  myScheduler.addTask(avoidRun, goInterval);
} 

void loop()  {
  runBitlash();
  myScheduler.scheduler();
}



