int vPrev = 512;
int fPrev = 0;

void setup()
{
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);

  Serial.begin(9600);
  Serial.println(vPrev);
}

void loop()
{
  int vNext = analogRead(A0);

  if (vPrev == vNext)
    return;

  vPrev = vNext;

  int fNext = ~analogRead(A1) & 512;
  fPrev = fNext;

  Serial.println((fNext << 1) + vNext);
}
