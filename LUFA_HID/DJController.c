#include "DJController.h"

USB_ClassInfo_HID_Device_t HID_Device_Keyboard =
{
  .Config =
  {
    .InterfaceNumber = 0,
    .ReportINEndpoint = { .Address = 1, .Size = sizeof(HID_Data_Keyboard_t), .Banks = 1 },
    .PrevReportINBuffer = NULL,
    .PrevReportINBufferSize = 0
  }
};

USB_ClassInfo_HID_Device_t HID_Device_Mixer =
{
  .Config =
  {
    .InterfaceNumber = 1,
    .ReportINEndpoint = { .Address = 2, .Size = sizeof(HID_Data_Mixer_t), .Banks = 1 },
    .PrevReportINBuffer = NULL,
    .PrevReportINBufferSize = 0
  }
};

USB_ClassInfo_HID_Device_t HID_Device_Deck1 =
{
  .Config =
  {
    .InterfaceNumber = 2,
    .ReportINEndpoint = { .Address = 3, .Size = sizeof(HID_Data_Deck_t), .Banks = 1 },
    .PrevReportINBuffer = NULL,
    .PrevReportINBufferSize = 0
  }
};

USB_ClassInfo_HID_Device_t HID_Device_Deck2 =
{
  .Config =
  {
    .InterfaceNumber = 3,
    .ReportINEndpoint = { .Address = 4, .Size = sizeof(HID_Data_Deck_t), .Banks = 1 },
    .PrevReportINBuffer = NULL,
    .PrevReportINBufferSize = 0
  }
};

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
	/* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
	XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
	XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

	/* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
	XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
	XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

	/* Hardware Initialization */
	USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
}

void EVENT_USB_Device_ConfigurationChanged(void)
{
    HID_Device_ConfigureEndpoints(&HID_Device_Keyboard);
    // HID_Device_ConfigureEndpoints(&HID_Device_Mixer);
    // HID_Device_ConfigureEndpoints(&HID_Device_Deck1);
    // HID_Device_ConfigureEndpoints(&HID_Device_Deck2);
    USB_Device_EnableSOFEvents();
}

void EVENT_USB_Device_ControlRequest(void)
{
  HID_Device_ProcessControlRequest(&HID_Device_Keyboard);
  // HID_Device_ProcessControlRequest(&HID_Device_Mixer);
  // HID_Device_ProcessControlRequest(&HID_Device_Deck1);
  // HID_Device_ProcessControlRequest(&HID_Device_Deck2);
}
