#include <stdio.h>
#include <Windows.h>
#include "hidapi/hidapi.h"

int main2() {
    if (hid_init())
    {
        printf("HID init failed.\n");
        return -1;
    }

    // VID, PID
    unsigned short vid = 0x16c0; // Vendor ID
    unsigned short pid = 0x05df; // Product ID

    hid_device_info* info = hid_enumerate(vid, pid);
    hid_device_info* curInfo = info;

    if (!curInfo)
    {
        printf("No matching HID devices found.\n");
    }

    while (curInfo)
    {
        printf("Device Path: %s\n", curInfo->path);
        curInfo = curInfo->next;
    }

    hid_free_enumeration(info);

    // 장치 열기
    hid_device* device = hid_open(vid, pid, nullptr);
    if (!device)
    {
        printf("Device not found.\n");
        return -1;
    }

    wchar_t str[128];
    int len = -1;
    len = hid_get_manufacturer_string(device, str, 128);
    wprintf(L"Opened Device Manufacturer Name == %ls\n", str);
    len = hid_get_product_string(device, str, 128);
    wprintf(L"Opened Device Product Name == %ls\n", str);

    int res = -1;

    // 송신 데이터 준비
    //unsigned char output[65];
    //output[0] = 0x00; // Report ID
    //res = hid_write(device, output, sizeof(output));

    //if (res < 0)
    //{
    //    wprintf(L"Write Failed. (%ls)\n", hid_error(device));
    //}
    //else
    //{
    //    wprintf(L"Write Succeed. (%d bytes)\n", res);
    //}

    // 예: Keyboard 리포트 읽기 (8바이트 가정)
    unsigned char report[9];
    res = hid_read_timeout(device, report, sizeof(report), 10000);
    printf("res == %d\n", res);

    if (res < 0)
    {
        printf("Error: %d\n", GetLastError());
    }

    if (res > 0)
    {
        printf("Read %d bytes: ", res);
        for (int i = 0; i < res; ++i)
        {
            printf("%x ", (int)report[i]);
        }
        printf("\n");
    }

    // 예: Mixer 리포트 쓰기 (Vendor-defined)
    //unsigned char outReport[16] = { 0x01, 0x02, 0x03 };
    //hid_write(device, outReport, sizeof(outReport));

    hid_close(device);
    hid_exit();

    return 0;
}