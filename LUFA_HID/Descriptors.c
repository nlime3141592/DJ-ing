/** \file
 *
 *  USB Device Descriptors, for library use when in USB device mode. Descriptors are special
 *  computer-readable structures which the host requests upon device enumeration, to determine
 *  the device's capabilities and functions.
 */

#include "Descriptors.h"

#include "DJController.h"

// HID Report Descriptor for Keyboard
const uint8_t PROGMEM HIDReport_Keyboard[] = {
    0x06, 0x00, 0xFF,       // Usage Page (Vendor Defined)
    0x09, 0x01,             // Usage (Vendor Usage 1)
    0xA1, 0x01,             // Collection (Application)
    0x15, 0x00,             // Logical Minimum 0
    0x26, 0xFF, 0x00,       // Logical Maximum 255
    0x75, 0x08,             // Report Size: 8 bits
    0x95, 0x40,             // Report Count: 64 bytes
    0x09, 0x01,             // Usage
    0x81, 0x02,             // Input (Data, Var, Abs)
    0x09, 0x01,             // Usage
    0x91, 0x02,             // Output (Data, Var, Abs)
    0xC0                    // End Collection
};

const USB_Descriptor_HIDReport_Datatype_t PROGMEM t[] =
{
  HID_RI_USAGE_PAGE(8, 0x01),                     \
			HID_RI_USAGE(8, 0x06),                          \
			HID_RI_COLLECTION(8, 0x01),                     \
				HID_RI_USAGE_PAGE(8, 0x07),                 \
				HID_RI_USAGE_MINIMUM(8, 0xE0),              \
				HID_RI_USAGE_MAXIMUM(8, 0xE7),              \
				HID_RI_LOGICAL_MINIMUM(8, 0x00),            \
				HID_RI_LOGICAL_MAXIMUM(8, 0x01),            \
				HID_RI_REPORT_SIZE(8, 0x01),                \
				HID_RI_REPORT_COUNT(8, 0x08),               \
				HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE), \
				HID_RI_REPORT_COUNT(8, 0x01),               \
				HID_RI_REPORT_SIZE(8, 0x08),                \
				HID_RI_INPUT(8, HID_IOF_CONSTANT),          \
				HID_RI_USAGE_MINIMUM(8, 0x00),              \
				HID_RI_USAGE_MAXIMUM(8, 0xFF),              \
				HID_RI_REPORT_COUNT(8, 0x06),               \
				HID_RI_REPORT_SIZE(8, 0x08),                \
				HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_ARRAY | HID_IOF_ABSOLUTE), \
			HID_RI_END_COLLECTION(0)
};

// HID Report Descriptor for Mixer
const USB_Descriptor_HIDReport_Datatype_t PROGMEM HIDReport_Mixer[] =
{
  HID_RI_USAGE_PAGE(8, 0xFF00),     // Vendor-defined
  HID_RI_USAGE(8, 0x01),            // Mixer input
  HID_RI_COLLECTION(8, 0x01),

  HID_RI_LOGICAL_MINIMUM(16, 0x0000),
  HID_RI_LOGICAL_MAXIMUM(16, 0x03FF),   // 10bit ADC, 필요시 1023
  HID_RI_REPORT_SIZE(8, 16),
  HID_RI_REPORT_COUNT(8, 16),           // 16채널
  HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

  HID_RI_END_COLLECTION(0)
};

// HID Report Descriptor for Deck
const USB_Descriptor_HIDReport_Datatype_t PROGMEM HIDReport_Deck[] =
{
  HID_RI_USAGE_PAGE(8, 0xFF00),     // Vendor-defined
  HID_RI_USAGE(8, 0x01),            // Deck input
  HID_RI_COLLECTION(8, 0x01),

  HID_RI_LOGICAL_MINIMUM(16, 0x0000),
  HID_RI_LOGICAL_MAXIMUM(16, 0x03FF),   // 10bit ADC, 필요시 1023
  HID_RI_REPORT_SIZE(8, 16),
  HID_RI_REPORT_COUNT(8, 16),           // 16채널
  HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

  HID_RI_END_COLLECTION(0)
};

// Device Descriptor
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},
    .USBSpecification       = VERSION_BCD(1, 1, 0),

    // HID Class Device는 Device Descriptor의 Class를 정의하지 않는다.
    .Class                  = USB_CSCP_NoDeviceClass,
    // HID Class Device는 HID Interface에 있는 SubClass와 Protocol 값을 사용하므로 Device Descriptor의 Class는 사용하지 않는다.
    .SubClass               = USB_CSCP_NoDeviceSubclass,
    .Protocol               = USB_CSCP_NoDeviceProtocol,

    .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

    // C/C++의 HID API는 Vendor ID와 Product ID를 이용해 장치를 연다. 형식은 아래와 같다.
    // hid_open(vid, pid, nullptr);
    .VendorID               = 0x16c0,
    .ProductID              = 0x05df,

    .ReleaseNumber          = VERSION_BCD(0, 0, 1),

    .ManufacturerStrIndex   = 0x01,
    .ProductStrIndex        = 0x02,
    .SerialNumStrIndex      = 0x03,
    // .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
    .NumberOfConfigurations = 1
};

const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor =
{
  .Config =
  {
    .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},
    .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
    .TotalInterfaces        = 1,
    .ConfigurationNumber    = 1,
    .ConfigurationStrIndex  = NO_DESCRIPTOR,
    .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),
    .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
  },

  // Interface 0: HID Keyboard
  .HID_Interface_Keyboard =
  {
    .Header = { .Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface },
    .InterfaceNumber = 0,
    .AlternateSetting = 0,
    .TotalEndpoints = 1,
    .Class = HID_CSCP_HIDClass,
    .SubClass = HID_CSCP_BootSubclass,
    // .SubClass = HID_CSCP_NonBootSubclass,
    .Protocol = HID_CSCP_KeyboardBootProtocol,
    // .Protocol = HID_CSCP_NonBootProtocol,
    .InterfaceStrIndex = 0
  },

  .HID_Data_Keyboard =
  {
    .Header = { .Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID },
    .HIDSpec = VERSION_BCD(1, 1, 1),
    .CountryCode = 0,
    .TotalReportDescriptors = 1,
    .HIDReportType = HID_DTYPE_Report,
    .HIDReportLength = sizeof(HIDReport_Keyboard)
  },

  .HID_EP_Keyboard =
  {
    .Header = { .Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint },
    .EndpointAddress = (ENDPOINT_DIR_IN | 1), // EP1 IN
    .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
    .EndpointSize = sizeof(HID_Data_Keyboard_t),
    .PollingIntervalMS = 10
  },

  // Interface 1: Analog 1, Mixer
  // .HID_Interface_mixer =
  // {
  //   .Header = { .Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface },
  //   .InterfaceNumber = 1,
  //   .AlternateSetting = 0,
  //   .TotalEndpoints = 1,
  //   .Class = HID_CSCP_HIDClass,
  //   .SubClass = HID_CSCP_NonBootSubclass,
  //   .Protocol = HID_CSCP_NonBootProtocol,
  //   .InterfaceStrIndex = NO_DESCRIPTOR
  // },

  // .HID_Data_mixer =
  // {
  //   .Header = { .Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID },
  //   .HIDSpec = VERSION_BCD(1, 1, 1),
  //   .CountryCode = 0,
  //   .TotalReportDescriptors = 1,
  //   .HIDReportType = HID_DTYPE_Report,
  //   .HIDReportLength = sizeof(HIDReport_Mixer)
  // },

  // .HID_EP_mixer =
  // {
  //   .Header = { .Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint },
  //   .EndpointAddress = (ENDPOINT_DIR_IN | 2),  // EP2 IN
  //   .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
  //   .EndpointSize = sizeof(HID_Data_Mixer_t),
  //   .PollingIntervalMS = 10
  // },

  // Interface 2: Analog 2, Deck 1
  // .HID_Interface_deck1 =
  // {
  //   .Header = { .Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface },
  //   .InterfaceNumber = 2,
  //   .AlternateSetting = 0,
  //   .TotalEndpoints = 1,
  //   .Class = HID_CSCP_HIDClass,
  //   .SubClass = HID_CSCP_NonBootSubclass,
  //   .Protocol = HID_CSCP_NonBootProtocol,
  //   .InterfaceStrIndex = NO_DESCRIPTOR
  // },

  // .HID_Data_deck1 =
  // {
  //   .Header = { .Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID },
  //   .HIDSpec = VERSION_BCD(1, 1, 1),
  //   .CountryCode = 0,
  //   .TotalReportDescriptors = 1,
  //   .HIDReportType = HID_DTYPE_Report,
  //   .HIDReportLength = sizeof(HIDReport_Deck) // TODO: Report 배열 길이를 대입해야 함.
  // },

  // .HID_EP_deck1 =
  // {
  //   .Header = { .Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint },
  //   .EndpointAddress = (ENDPOINT_DIR_IN | 3),  // EP3 IN
  //   .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
  //   .EndpointSize = sizeof(HID_Data_Deck_t),
  //   .PollingIntervalMS = 10
  // },

  // Interface 3: Analog 3, Deck 2
  // .HID_Interface_deck2 =
  // {
  //   .Header = { .Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface },
  //   .InterfaceNumber = 3,
  //   .AlternateSetting = 0,
  //   .TotalEndpoints = 1,
  //   .Class = HID_CSCP_HIDClass,
  //   .SubClass = HID_CSCP_NonBootSubclass,
  //   .Protocol = HID_CSCP_NonBootProtocol,
  //   .InterfaceStrIndex = NO_DESCRIPTOR
  // },

  // .HID_Data_deck2 =
  // {
  //   .Header = { .Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID },
  //   .HIDSpec = VERSION_BCD(1, 1, 1),
  //   .CountryCode = 0,
  //   .TotalReportDescriptors = 1,
  //   .HIDReportType = HID_DTYPE_Report,
  //   .HIDReportLength = sizeof(HIDReport_Deck)
  // },

  // .HID_EP_deck2 =
  // {
  //   .Header = { .Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint },
  //   .EndpointAddress = (ENDPOINT_DIR_IN | 4),  // EP4 IN
  //   .Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
  //   .EndpointSize = sizeof(HID_Data_Deck_t),
  //   .PollingIntervalMS = 10
  // }
};

/** This function is called by the library when in device mode, and must be overridden (see library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
	const uint8_t  DescriptorType   = (wValue >> 8);
	const uint8_t  DescriptorNumber = (wValue & 0xFF);

	const void* Address = NULL;
	uint16_t    Size    = NO_DESCRIPTOR;

	switch (DescriptorType)
	{
		case DTYPE_Device:
			Address = &DeviceDescriptor;
			Size    = sizeof(USB_Descriptor_Device_t);
			break;
		case DTYPE_Configuration:
			Address = &ConfigurationDescriptor;
			Size    = sizeof(USB_Descriptor_Configuration_t);
			break;
    case HID_DTYPE_Report:
      switch (wIndex)
      {
        case 0:
          Address = &HIDReport_Keyboard;
          Size = sizeof(HIDReport_Keyboard);
          break;
        case 1:
          Address = &HIDReport_Mixer;
          Size = sizeof(HIDReport_Mixer);
          break;
        case 2:
        case 3:
          Address = &HIDReport_Deck;
          Size = sizeof(HIDReport_Deck);
          break;
      }
      break;
	}

	*DescriptorAddress = Address;
	return Size;
}

bool CALLBACK_HID_Device_CreateHIDReport(
    USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
    uint8_t* const ReportID,
    const uint8_t ReportType,
    void* ReportData,
    uint16_t* const ReportSize)
{
  digitalWrite(16, 1);

  if (HIDInterfaceInfo == &HID_Device_Keyboard)
  {
    HID_Data_Keyboard_t* KeyboardReport = (HID_Data_Keyboard_t*)ReportData;
    memset(KeyboardReport, 0, sizeof(HID_Data_Keyboard_t));
    *ReportSize = sizeof(HID_Data_Keyboard_t);

    if (digitalRead(10))
    {
      KeyboardReport->buffer[2] = 0x04;
    }
    else
    {
      KeyboardReport->buffer[2] = 0x00;
    }
    
    return true; // false를 반환해도 상관 없지만 호스트에 입력할 데이터가 존재한다면 명시적으로 true를 반환해 주는 것이 좋음.
  }

    // Mixer
    // if (HIDInterfaceInfo == &HID_Device_Mixer) {
    //     HID_Data_Mixer_t* MixerReport = (HID_Data_Mixer_t*)ReportData;
    //     memset(MixerReport, 0, sizeof(HID_Data_Mixer_t));
    //     *ReportSize = sizeof(HID_Data_Mixer_t);
    //     *ReportID = 2;

    //     MixerReport->buffer[0] = analogRead(A1);

    //     return true; // false를 반환해도 상관 없지만 호스트에 입력할 데이터가 존재한다면 명시적으로 true를 반환해 주는 것이 좋음.
    // }

    // Deck1
    // if (HIDInterfaceInfo == &HID_Device_Deck1) {
    //     HID_Data_Deck_t* DeckReport = (HID_Data_Deck_t*)ReportData;
    //     memset(DeckReport, 0, sizeof(HID_Data_Deck_t));
    //     *ReportSize = sizeof(HID_Data_Deck_t);
    //     *ReportID = 3;

    //     DeckReport->buffer[0] = analogRead(A2);

    //     return true; // false를 반환해도 상관 없지만 호스트에 입력할 데이터가 존재한다면 명시적으로 true를 반환해 주는 것이 좋음.
    // }

    // Deck2
    // if (HIDInterfaceInfo == &HID_Device_Deck2) {
    //     HID_Data_Deck_t* DeckReport = (HID_Data_Deck_t*)ReportData;
    //     memset(DeckReport, 0, sizeof(HID_Data_Deck_t));
    //     *ReportSize = sizeof(HID_Data_Deck_t);
    //     *ReportID = 4;

    //     DeckReport->buffer[0] = analogRead(A3);

    //     return true; // false를 반환해도 상관 없지만 호스트에 입력할 데이터가 존재한다면 명시적으로 true를 반환해 주는 것이 좋음.
    // }

    return false;
}

void CALLBACK_HID_Device_ProcessHIDReport(
    USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
    const uint8_t ReportID,
    const uint8_t ReportType,
    const void* ReportData,
    const uint16_t ReportSize)
{
    // 현재는 PC에서 받은 데이터 무시
}