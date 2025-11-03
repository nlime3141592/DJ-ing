#include <Keyboard.h>

uint8_t buffer0[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uint8_t buffer1[8] = { 0, 1, 0, 0, 0, 0, 0, 0 };

uint8_t buffer[32] = { 0 };
uint8_t* pBuffer0;
uint8_t* pBuffer1;
uint8_t* pBuffer2;
uint8_t* pBuffer3;

uint8_t digitalKeyTable[8] = { 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18 };

void setup()
{
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);

  pBuffer0 = buffer;
  pBuffer1 = buffer + 8;
  pBuffer2 = buffer + 16;
  pBuffer3 = buffer + 24;
}

void loop()
{
  // 본 코드는 Soft Real Time을 따릅니다.
  // 본 루프가 100 마이크로초 이내에 실행될 수 있도록 코딩하는 것이 권장됩니다.
  delayMicroseconds(100);

  ///////////////////////////////////////////////////////////////////////////////////////////////
  // buffer0[2] = !digitalRead(6) ? digitalKeyTable[0] : 0x00;
  // buffer0[3] = !digitalRead(7) ? digitalKeyTable[1] : 0x00;
  // buffer0[4] = !digitalRead(8) ? digitalKeyTable[2] : 0x00;
  // buffer0[5] = !digitalRead(9) ? digitalKeyTable[3] : 0x00;

  pBuffer0[2] = !digitalRead(6) ? digitalKeyTable[0] : 0x00;
  pBuffer0[3] = !digitalRead(7) ? digitalKeyTable[1] : 0x00;
  pBuffer0[4] = !digitalRead(8) ? digitalKeyTable[2] : 0x00;
  pBuffer0[5] = !digitalRead(9) ? digitalKeyTable[3] : 0x00;

  ///////////////////////////////////////////////////////////////////////////////////////////////
  // buffer1[2] = analogRead(A0) >> 2;
  // buffer1[3] = analogRead(A1) >> 2;
  // buffer1[4] = analogRead(A2) >> 2;
  // buffer1[5] = analogRead(A3) >> 2;

  pBuffer1[2] = analogRead(A0) >> 2;
  pBuffer1[3] = analogRead(A1) >> 2;
  pBuffer1[4] = analogRead(A2) >> 2;
  pBuffer1[5] = analogRead(A3) >> 2;

  ///////////////////////////////////////////////////////////////////////////////////////////////
  // Keyboard.sendReport((KeyReport*)&buffer0);
  // Keyboard.sendReport((KeyReport*)&buffer1);
  Keyboard.sendBuffer(buffer, sizeof(buffer));
}
