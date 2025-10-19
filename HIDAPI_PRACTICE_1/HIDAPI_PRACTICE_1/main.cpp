#include <stdio.h>
#include <Windows.h>
#include "hidapi/hidapi.h"

int main() {
    if (hid_init())
    {
        printf("HID init failed.\n");
        return -1;
    }

    // VID, PID
    unsigned short vid = 0x16c0; // Vendor ID
    unsigned short pid = 0x05df; // Product ID

    //hid_device_info* info = hid_enumerate(vid, pid);
    //hid_device_info* curInfo = info;

    /*if (!curInfo)
    {
        printf("No matching HID devices found.\n");
    }

    while (curInfo)
    {
        printf("Device Path: %s\n", curInfo->path);
        curInfo = curInfo->next;
    }*/

    //hid_free_enumeration(info);

    // 장치 열기
    hid_device* device = hid_open(vid, pid, nullptr);
    if (!device)
    {
        printf("Device not found.\n");
        return -1;
    }

    // 예: Keyboard 리포트 읽기 (8바이트 가정)
    unsigned char report[9];
    int res = hid_read_timeout(device, report, sizeof(report), 10000);
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