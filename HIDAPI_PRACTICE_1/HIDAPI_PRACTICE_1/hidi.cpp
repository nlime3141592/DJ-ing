#include <iostream>
#include <Windows.h>
#include "hidapi/hidapi.h"

int print_flag = 0;

typedef struct
{
    uint8_t modifier;
    uint8_t channel;
    uint8_t values[6];
} HIDKeyboardPacket;

typedef struct
{
    uint8_t lShift;
    uint8_t rShift;

    uint8_t digital0;
    uint8_t digital1;
    uint8_t digital2;
    uint8_t digital3;
    uint8_t digital4;
    uint8_t digital5;

    uint8_t mixer0;

    uint8_t deck10;
    uint8_t deck11;
    uint8_t deck12;
    uint8_t deck13;

    uint8_t deck20;
    uint8_t deck21;
    uint8_t deck22;
    uint8_t deck23;
} DJKeyboardInput;

void printe()
{
    if (print_flag == 1)
        return;

    printf("GetLastError() == %d\n", GetLastError());
    print_flag = 1;
}

void printp(int reportID, unsigned char* buffer, int length)
{
    printf("Digital value-%03d:", reportID);
    for (int i = 0; i < length; ++i)
    {
        printf(" %3d", (int)buffer[i]);
    }
    printf("\n");

    print_flag = 0;
}

void printz()
{
    if (print_flag == 2)
        return;

    printf("Empty Packet.\n");
    print_flag = 2;
}

void printk(uint8_t* buffer)
{
    printf("Shift   [%02x %02x]\nDigital [%02x %02x %02x %02x %02x %02x]\nMixer   [%04d %04d %04d %04d %04d %04d %04d %04d]\nDeck1   [%04d %04d %04d %04d %04d %04d %04d %04d]\nDeck2   [%04d %04d %04d %04d %04d %04d %04d %04d]\n",
        buffer[0], buffer[1],
        buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7],
        buffer[8], buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15],
        buffer[16], buffer[17], buffer[18], buffer[19], buffer[20], buffer[21], buffer[22], buffer[23],
        buffer[24], buffer[25], buffer[26], buffer[27], buffer[28], buffer[29], buffer[30], buffer[31]);
}

void update_modifier(DJKeyboardInput* input, uint8_t modifier)
{
    input->lShift = ((modifier & 0xC0) != 0);
    input->rShift = ((modifier & 0x0C) != 0);
}

int main() {
    if (hid_init())
        return -1;

    // 아두이노 Leonardo의 VID/PID (확인 필요)
    unsigned short vid = 0x2341;
    //unsigned short vid = 0x1357;
    unsigned short pid = 0x8036;
    //unsigned short pid = 0x2468;

    hid_device* handle = hid_open(vid, pid, NULL);
    if (!handle) {
        std::cerr << "Device not found.\n";
        return -1;
    }

    unsigned char buffer[64];
    
    DJKeyboardInput input;

    while (true)
    {
        int res = hid_read(handle, buffer, sizeof(buffer));

        int reportId = buffer[0];

        if (res < 0)
        {
            printe();
        }
        else if (res > 0)
        {
            unsigned char reportId = buffer[0];

            if (reportId != 2)
                continue;
            
            printk((uint8_t*)(buffer + 1));
        }
        else
        {
            printz();
        }
    }

    hid_close(handle);
    hid_exit();
    return 0;
}