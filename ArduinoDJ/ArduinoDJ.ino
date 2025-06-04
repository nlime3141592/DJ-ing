#define PIN_BUS_S0 2
#define PIN_BUS_S1 3
#define PIN_BUS_S2 4
#define PIN_HALT 13

#define FLAG_HALT 0b00000001

uint8_t flagTimer = 0;
uint8_t flags = 0;
uint8_t analogBuffer[48] = { 0 };

void setup()
{
  pinMode(A0, INPUT);
  pinMode(PIN_BUS_S0, OUTPUT);
  pinMode(PIN_BUS_S1, OUTPUT);
  pinMode(PIN_BUS_S2, OUTPUT);
  pinMode(PIN_HALT, INPUT_PULLUP);

  Serial.begin(115200);
}

void loop()
{
  setFlag(PIN_HALT, FLAG_HALT);

  muxAnalogInput(analogBuffer, 0, 7, A0);

  if (!Serial.available())
    return;

  int rd = Serial.read(); // ignore
  Serial.write(flags);
  Serial.write((const char*)analogBuffer, 8);
  delayMicroseconds(10);
}

void setFlag(int pinDigital, uint8_t mask)
{
  if (digitalRead(pinDigital))
    flags &= ~mask;
  else
    flags |= mask;
}

void muxAnalogInput(uint8_t* dest, int idxBeg, int idxEnd, int pinAnalog)
{
  int m = idxEnd - idxBeg;

  for (int i = 0; i <= m; ++i)
  {
    int j = idxBeg + i;
    digitalWrite(PIN_BUS_S0, j & 1);
    digitalWrite(PIN_BUS_S1, j & 2);
    digitalWrite(PIN_BUS_S2, j & 4);
    delayMicroseconds(1);
    dest[i] = (uint8_t)(255 - (analogRead(pinAnalog) >> 2));
  }
}
