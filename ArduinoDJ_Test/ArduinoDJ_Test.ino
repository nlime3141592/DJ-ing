#define PIN_DECK_1 A0 // Tempo Slider #1
#define PIN_DECK_2 A1 // Tempo Slider #2
#define PIN_VF_1 A2 // Volume Fader #1
#define PIN_VF_2 A3 // Volume Fader #2
#define PIN_XF A4 // Cross Fader
#define PIN_EQ A5 // Equalizer

#define PIN_ROTARY_CLK 2
#define PIN_ROTARY_DT 3

#define PIN_CLK 4

#define PIN_BUS_S0 5
#define PIN_BUS_S1 6
#define PIN_BUS_S2 7

#define PIN_IO_ENABLE 8

#define PIN_BUTTON_CTRL 11
#define PIN_BUTTON_DECK_1 12
#define PIN_BUTTON_DECK_2 13

#define DIR_ROTARY_CW 1
#define DIR_ROTARY_CCW -1
#define DIR_ROTARY_IDLE 0
#define DIR_ROTARY_UNDEFINED 0

int8_t rotaryPatterns[16] = {
  DIR_ROTARY_IDLE,
  DIR_ROTARY_CW,
  DIR_ROTARY_CCW,
  DIR_ROTARY_UNDEFINED,
  DIR_ROTARY_CCW,
  DIR_ROTARY_IDLE,
  DIR_ROTARY_UNDEFINED,
  DIR_ROTARY_CW,
  DIR_ROTARY_CW,
  DIR_ROTARY_UNDEFINED,
  DIR_ROTARY_IDLE,
  DIR_ROTARY_CCW,
  DIR_ROTARY_UNDEFINED,
  DIR_ROTARY_CCW,
  DIR_ROTARY_CW,
  DIR_ROTARY_IDLE
};

uint8_t rotaryStates[8] = { 0 };
uint8_t buttonValues[8] = { 0 };
uint8_t analogValues[16] = { 0 };

void setup()
{
  pinMode(PIN_DECK_1, INPUT);
  pinMode(PIN_DECK_2, INPUT);
  pinMode(PIN_VF_1, INPUT);
  pinMode(PIN_VF_2, INPUT);
  pinMode(PIN_XF, INPUT);
  pinMode(PIN_EQ, INPUT);

  pinMode(PIN_ROTARY, INPUT);

  pinMode(PIN_CLK, OUTPUT);

  pinMode(PIN_BUS_S0, OUTPUT);
  pinMode(PIN_BUS_S1, OUTPUT);
  pinMode(PIN_BUS_S2, OUTPUT);

  pinMode(PIN_IO_ENABLE, OUTPUT);

  pinMode(PIN_BUTTON_CTRL, INPUT);
  pinMode(PIN_BUTTON_DECK_1, INPUT);
  pinMode(PIN_BUTTON_DECK_2, INPUT);

  Serial.begin(115200);
}

uint8_t analogReadUint8(uint8_t pin, bool toComplement = false)
{
  uint8_t value = (uint8_t)(analogRead(PIN_EQ) >> 2);

  if (toComplement)
    return 255 - value;
  else
    return value;
}

void communicate()
{
  if (!Serial.available())
    return;

  Serial.read(); // Ignore
  Serial.write((const char*)rotaryStates, 8);
  Serial.write((const char*)buttonValues, 8);
  Serial.write((const char*)analogValues, 16);

  analogValues[13] = 0;
  analogValues[14] = 0;
  analogValues[15] = 0;
}

int rotaryRead(uint8_t* state)
{
  *state = *state << 1 | digitalRead(PIN_ROTARY_CLK);
  *state = *state << 1 | digitalRead(PIN_ROTARY_DT);
  *state &= 15;

  return rotaryPatterns[*state];
}

void loop()
{
  digitalWrite(PIN_CLK, 0);
  digitalWrite(PIN_CLK, 1);
  digitalWrite(PIN_IO_ENABLE, 1);

  for (int i = 0; i < 8; ++i)
  {
    // Control for 74HC166
    buttonValues[0] = (buttonValues[0] << 1) | digitalRead(PIN_BUTTON_CTRL);
    buttonValues[1] = (buttonValues[1] << 1) | digitalRead(PIN_BUTTON_DECK_1);
    buttonValues[2] = (buttonValues[2] << 1) | digitalRead(PIN_BUTTON_DECK_2);
    buttonValues[7] = (buttonValues[3] << 1) | digitalRead(PIN_ROTARY);
    digitalWrite(PIN_CLK, 0);
    digitalWrite(PIN_CLK, 1);

    // Control for 74HC4051
    digitalWrite(PIN_BUS_S0, i & 1);
    digitalWrite(PIN_BUS_S1, i & 2);
    digitalWrite(PIN_BUS_S2, i & 4);
    analogValues[i] = analogReadUint8(PIN_EQ, true);
  }

  analogValues[8] = analogReadUint8(PIN_DECK_1, true);
  analogValues[9] = analogReadUint8(PIN_DECK_2, true);
  analogValues[10] = analogReadUint8(PIN_VF_1, true);
  analogValues[11] = analogReadUint8(PIN_VF_2, true);
  analogValues[12] = analogReadUint8(PIN_XF, true);

  // communicate 전까지 반드시 0, 1, -1 중 하나일 수 밖에 없는 구조, 멀티스레딩 가능?
  analogValues[13] += rotaryRead(analogValues + 13);
  analogValues[14] += rotaryRead(analogValues + 14);
  analogValues[15] += rotaryRead(analogValues + 15);

  digitalWrite(PIN_IO_ENABLE, 0); // Active-Low

  communicate();

  analogValues[13] = 0;
  analogValues[14] = 0;
  analogValues[15] = 0;
}
