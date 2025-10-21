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

void printk(DJKeyboardInput* input)
{
    printf("Shift   [%02x %02x]\nDigital [%02x %02x %02x %02x %02x %02x]\nMixer   [%04d]\nDeck1   [%04d %04d %04d %04d]\nDeck2   [%04d %04d %04d %04d]\n",
        input->lShift, input->rShift,
        input->digital0, input->digital1, input->digital2, input->digital3, input->digital4, input->digital5,
        input->mixer0,
        input->deck10, input->deck11, input->deck12, input->deck13,
        input->deck20, input->deck21, input->deck22, input->deck23);
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

            HIDKeyboardPacket* packet = (HIDKeyboardPacket*)(buffer + 1);

            switch (packet->channel)
            {
            case 0: // Digital Input
                update_modifier(&input, packet->modifier);
                input.digital0 = packet->values[0];
                input.digital1 = packet->values[1];
                input.digital2 = packet->values[2];
                input.digital3 = packet->values[3];
                input.digital4 = packet->values[4];
                input.digital5 = packet->values[5];
                break;
            case 1: // Analog Input (Mixer)
                update_modifier(&input, packet->modifier);
                input.mixer0 = packet->values[0];
                break;
            case 2: // Analog Input (Deck 1)
                update_modifier(&input, packet->modifier);
                input.deck10 = packet->values[0];
                input.deck11 = packet->values[1];
                input.deck12 = packet->values[2];
                input.deck13 = packet->values[3];
                break;
            case 3: // Analog Input (Deck 2)
                update_modifier(&input, packet->modifier);
                input.deck20 = packet->values[0];
                input.deck21 = packet->values[1];
                input.deck22 = packet->values[2];
                input.deck23 = packet->values[3];
                break;
            }

            printk(&input);
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