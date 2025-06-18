#include <Arduino.h>

#include "djdef.h"
#include "djinput.h"

#include "74166.h"
#include "744051.h"

// 마지막 통신 시간 기록 변수
static uint32_t communicateTimestamp;

static uint8_t dWheelState1 = 0;
static uint8_t dWheelState2 = 0;

static IC744051 icEq;

void setup()
{
  IO_SETUP_TEMPO_1;
  IO_SETUP_TEMPO_2;
  IO_SETUP_VF_1;
  IO_SETUP_VF_2;
  IO_SETUP_XF;
  IO_SETUP_EQ;
  IO_SETUP_BUTTON_ROTARY;
  IO_SETUP_SOFTWARE_CLK;
  IO_SETUP_74166_SIN;
  IO_SETUP_BUS_S0;
  IO_SETUP_BUS_S1;
  IO_SETUP_BUS_S2;
  IO_SETUP_VF_1;
  IO_SETUP_VF_2;
  IO_SETUP_BUTTON_CTRL;
  IO_SETUP_BUTTON_DECK_1;
  IO_SETUP_BUTTON_DECK_2;

  init74166(PIN_74166_SIN);

  icEq.pinS0 = PIN_BUS_S0;
  icEq.pinS1 = PIN_BUS_S1;
  icEq.pinS2 = PIN_BUS_S2;
  icEq.pinOut = PIN_EQ;

  Serial.begin(BAUD_RATE);

  // 시간 기록
  communicateTimestamp = micros();
}

void update()
{
  // 1. 74HC4051 기반 다중화된 아날로그 입력 처리
  ictrl.interface.eq0 = readAnalogMultiplexerUint8(&icEq, 0);
  ictrl.interface.eq1 = readAnalogMultiplexerUint8(&icEq, 1);
  ictrl.interface.eq2 = readAnalogMultiplexerUint8(&icEq, 2);
  ictrl.interface.eq3 = readAnalogMultiplexerUint8(&icEq, 3);
  ictrl.interface.eq4 = readAnalogMultiplexerUint8(&icEq, 4);
  ictrl.interface.eq5 = readAnalogMultiplexerUint8(&icEq, 5);
  ictrl.interface.fx0 = readAnalogMultiplexerUint8(&icEq, 6);
  ictrl.interface.fx1 = readAnalogMultiplexerUint8(&icEq, 7);

  // 2. 일반적인 아날로그 입력 처리
  ictrl.interface.vf0 = analogReadUint8(PIN_VF_1, true);
  ictrl.interface.vf1 = analogReadUint8(PIN_VF_2, true);
  ictrl.interface.xf = analogReadUint8(PIN_XF);

  // 3. 74HC166 기반 다중화된 버튼 입력 처리
  inputParallel(PIN_74166_SIN, PIN_SOFTWARE_CLK);

  ictrl.interface.btnFlag0 = 0;
  ideck1.interface.btnFlag0 = 0;
  ideck2.interface.btnFlag0 = 0;

  for (int i = 0; i < 8; ++i) // 직렬 출력 8회 수행
  {
    ictrl.interface.btnFlag0 |= (readSerial(PIN_BUTTON_CTRL) << i);
    ideck1.interface.btnFlag0 |= (readSerial(PIN_BUTTON_DECK_1) << i);
    ideck2.interface.btnFlag0 |= (readSerial(PIN_BUTTON_DECK_2) << i);

    clock74166(PIN_SOFTWARE_CLK);
  }

  // 4. 로터리 엔코더의 회전 변화율 계산
  ideck1.interface.dWheel += rotaryRead(&dWheelState1, (ictrl.interface.btnFlag0 >> 2) & 0x03);
  ideck2.interface.dWheel += rotaryRead(&dWheelState2, ictrl.interface.btnFlag0 & 0x03);
}

void communicate()
{
  uint32_t currentTimestamp = micros();

  // 마지막으로 통신한 시간이 기준 시간 이하면 통신하지 않음
  if (currentTimestamp - communicateTimestamp < COMMUNICATION_PERIOD_MICROSECONDS)
    return;

  // 통신 시작
  communicateTimestamp = currentTimestamp;

  // PC가 1바이트를 전송함으로써 데이터를 전송을 시작함
  if (!Serial.available())
    return;

  Serial.read(); // ignore

  // 구조체의 byte stream을 직렬 포트로 출력
  Serial.write((const char*)ictrl.rawdata, sizeof(InputController));
  Serial.write((const char*)ideck1.rawdata, sizeof(InputDeck));
  Serial.write((const char*)ideck2.rawdata, sizeof(InputDeck));

  // 통신 주기 내에 변한 로터리 엔코더의 변화량 초기화
  ictrl.interface.dSelect = 0;
  ideck1.interface.dWheel = 0;
  ideck2.interface.dWheel = 0;
}

void loop()
{
  // 하드웨어 갱신은 매 loop() 마다 수행
  update();

  // 일정 주기마다 PC와 통신 수행
  communicate();
}
