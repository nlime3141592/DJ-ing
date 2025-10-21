#include <stdio.h>
#include <Windows.h>

int main1()
{
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    INPUT_RECORD record;
    DWORD eventsRead;

    while (1)
    {
        ReadConsoleInput(hInput, &record, 1, &eventsRead);

        if (record.EventType == KEY_EVENT)
        {
            if (record.Event.KeyEvent.bKeyDown)
                printf("Key down: %c\n", record.Event.KeyEvent.uChar.AsciiChar);
        }
    }
}