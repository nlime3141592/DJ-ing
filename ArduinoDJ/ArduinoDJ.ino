#define PIN_BUS_S0 2
#define PIN_BUS_S1 3
#define PIN_BUS_S2 4
#define PIN_HALT 13

int halt = 0;
int values[8] = { 0 };

void setup()
{
  pinMode(A0, INPUT);
  pinMode(PIN_BUS_S0, OUTPUT);
  pinMode(PIN_BUS_S1, OUTPUT);
  pinMode(PIN_BUS_S2, OUTPUT);
  pinMode(PIN_HALT, INPUT_PULLUP);

  Serial.begin(9600);
}

void loop()
{
  muxAnalogInput(values, 0, 7, A0);
  halt = digitalRead(PIN_HALT) == 0 ? 1 : 0;

  if (!Serial.available())
    return;

  int rd = Serial.read(); // ignore
  Serial.print(values[0]);
  Serial.print('/');
  Serial.print(values[1]);
  Serial.print('/');
  Serial.print(values[2]);
  Serial.print('/');
  Serial.print(values[3]);
  Serial.print('/');
  Serial.print(values[4]);
  Serial.print('/');
  Serial.print(values[5]);
  Serial.print('/');
  Serial.print(values[6]);
  Serial.print('/');
  Serial.print(values[7]);
  Serial.print('/');
  Serial.print(halt);
  Serial.print('\n');
  delayMicroseconds(10);
}

void muxAnalogInput(int* dest, int addrBeg, int addrEnd, int pinAnalog)
{
  int m = addrEnd - addrBeg;

  for (int i = 0; i <= m; ++i)
  {
    int j = addrBeg + i;
    digitalWrite(PIN_BUS_S0, j & 1);
    digitalWrite(PIN_BUS_S1, j & 2);
    digitalWrite(PIN_BUS_S2, j & 4);
    dest[i] = 1023 - analogRead(pinAnalog);
  }
}