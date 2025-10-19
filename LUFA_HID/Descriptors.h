#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

#include "LUFAConfig.h"

#include <avr/pgmspace.h>

#include <LUFA/LUFA/Drivers/USB/USB.h>

typedef struct
{
    USB_Descriptor_Configuration_Header_t Config;

    // HID Keyboard Interface
    USB_Descriptor_Interface_t HID_Interface_Keyboard;
    USB_HID_Descriptor_HID_t HID_Data_Keyboard;
    USB_Descriptor_Endpoint_t HID_EP_Keyboard;

    // HID Analog 1, Mixer
    // USB_Descriptor_Interface_t HID_Interface_mixer;
    // USB_HID_Descriptor_HID_t HID_Data_mixer;
    // USB_Descriptor_Endpoint_t HID_EP_mixer;

    // HID Analog 2, Deck 1
    // USB_Descriptor_Interface_t HID_Interface_deck1;
    // USB_HID_Descriptor_HID_t HID_Data_deck1;
    // USB_Descriptor_Endpoint_t HID_EP_deck1;

    // HID Analog 3, Deck 2
    // USB_Descriptor_Interface_t HID_Interface_deck2;
    // USB_HID_Descriptor_HID_t HID_Data_deck2;
    // USB_Descriptor_Endpoint_t HID_EP_deck2;

} USB_Descriptor_Configuration_t;

typedef struct
{
  uint8_t buffer[8];
} HID_Data_Keyboard_t;

typedef struct
{
  uint16_t buffer[16];
} HID_Data_Mixer_t;

typedef struct
{
  uint16_t buffer[16];
} HID_Data_Deck_t;

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
  const uint16_t wIndex,
  const void** const DescriptorAddress)
  ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);

// bool CALLBACK_HID_Device_CreateHIDReport(
//     USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
//     uint8_t* const ReportID,
//     const uint8_t ReportType,
//     void* ReportData,
//     uint16_t* const ReportSize);

// void CALLBACK_HID_Device_ProcessHIDReport(
//     USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
//     const uint8_t ReportID,
//     const uint8_t ReportType,
//     const void* ReportData,
//     const uint16_t ReportSize);
#endif

