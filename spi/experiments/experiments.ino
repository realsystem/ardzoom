/*
  
 */
#include "bitlash.h"
#ifdef SPI_ENABLE
#include <SPI.h>
#endif

#ifdef SPI_ENABLE
int focusSS = 10;
int focusMOSI = 11;
int focusMISO = 12;
int focusSCK = 13;
#endif

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

void setup()  { 
  initBitlash(115200);

#ifdef SPI_ENABLE
  addBitlashFunction("wr", (bitlash_function) writeReg);
  addBitlashFunction("rr", (bitlash_function) readReg);
  SPI.begin();
#endif
} 

void loop()  {
  runBitlash();
}


