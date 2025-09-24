#include <Arduino.h>

// 참고자료: https://www.youtube.com/watch?v=j05vj8zRP1o&t=103s

uint8_t buffer[8] = { 0 };

#define PIN_A 4
#define PIN_S 5
#define PIN_D 6
#define PIN_W 7

#define HID_KEY_A 4
#define HID_KEY_S 22
#define HID_KEY_D 7
#define HID_KEY_W 26

int flag = 0;

uint8_t hidKeyHorizontal[4] = { 0, HID_KEY_D, HID_KEY_A, 0 };
uint8_t hidKeyVertical[4] = { 0, HID_KEY_W, HID_KEY_S, 0 };

void setup()
{
  Serial.begin(9600);

  pinMode(PIN_A, INPUT);
  pinMode(PIN_S, INPUT);
  pinMode(PIN_D, INPUT);
  pinMode(PIN_W, INPUT);
}

void loop()
{
  int flagHorizontal = 0;
  int flagVertical = 0;

  flagHorizontal |= digitalRead(PIN_A);
  flagHorizontal <<= 1;
  flagHorizontal |= digitalRead(PIN_D);

  flagVertical |= digitalRead(PIN_S);
  flagVertical <<= 1;
  flagVertical |= digitalRead(PIN_W);
  
  buffer[3] = hidKeyHorizontal[flagHorizontal];
  buffer[2] = hidKeyVertical[flagVertical];

  Serial.write(buffer, 8);
  
  delay(10);
}

void keyRelease()
{
  buffer[0] = 0;
  buffer[1] = 0;
  buffer[2] = 0;
  Serial.write(buffer, 8);
}