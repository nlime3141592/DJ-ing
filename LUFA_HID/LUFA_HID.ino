/**
 * You should have a LUFAConfig.h for this to work.
 */
#include "LUFAConfig.h"

/**
 * Include LUFA.h after LUFAConfig.h
 */
#include <LUFA.h>

/**
 * Finally include the LUFA device setup header
 */
#include "DJController.h"

void setup()
{
  pinMode(10, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);

  pinMode(16, OUTPUT);
  digitalWrite(16, 0);

	SetupHardware(); // ask LUFA to setup the hardware
	GlobalInterruptEnable(); // enable global interrupts

  delay(1000);
}

int blink = 0;

void loop()
{
  delay(10);
  HID_Device_USBTask(&HID_Device_Keyboard);
  // HID_Device_USBTask(&HID_Device_Mixer);
  // HID_Device_USBTask(&HID_Device_Deck1);
  // HID_Device_USBTask(&HID_Device_Deck2);
  USB_USBTask();
}
