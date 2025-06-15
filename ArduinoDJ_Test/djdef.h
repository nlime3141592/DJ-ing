#ifndef _DJDEF_H
#define _DJDEF_H

#include <Arduino.h>

#define PIN_TEMPO_1 A0
#define PIN_TEMPO_2 A1
#define PIN_VF_1 A2
#define PIN_VF_2 A3
#define PIN_XF A4
#define PIN_EQ A5
#define PIN_BUTTON_ROTARY 2
#define PIN_SOFTWARE_CLK 4
#define PIN_BUS_S0 5
#define PIN_BUS_S1 6
#define PIN_BUS_S2 7
#define PIN_74166_SIN 8
#define PIN_BUTTON_CTRL 11
#define PIN_BUTTON_DECK_1 12
#define PIN_BUTTON_DECK_2 13



#define IO_SETUP_TEMPO_1 pinMode(PIN_TEMPO_1, INPUT)
#define IO_SETUP_TEMPO_2 pinMode(PIN_TEMPO_2, INPUT)
#define IO_SETUP_VF_1 pinMode(PIN_VF_1, INPUT)
#define IO_SETUP_VF_2 pinMode(PIN_VF_2, INPUT)
#define IO_SETUP_XF pinMode(PIN_XF, INPUT)
#define IO_SETUP_EQ pinMode(PIN_EQ, INPUT)
#define IO_SETUP_BUTTON_ROTARY pinMode(PIN_BUTTON_ROTARY, INPUT)
#define IO_SETUP_SOFTWARE_CLK pinMode(PIN_SOFTWARE_CLK, OUTPUT)
#define IO_SETUP_74166_SIN pinMode(PIN_74166_SIN, OUTPUT)
#define IO_SETUP_BUS_S0 pinMode(PIN_BUS_S0, OUTPUT)
#define IO_SETUP_BUS_S1 pinMode(PIN_BUS_S0, OUTPUT)
#define IO_SETUP_BUS_S2 pinMode(PIN_BUS_S0, OUTPUT)
#define IO_SETUP_VF_1 pinMode(PIN_VF_1, INPUT)
#define IO_SETUP_VF_2 pinMode(PIN_VF_2, INPUT)
#define IO_SETUP_BUTTON_CTRL pinMode(PIN_BUTTON_CTRL, INPUT)
#define IO_SETUP_BUTTON_DECK_1 pinMode(PIN_BUTTON_DECK_1, INPUT)
#define IO_SETUP_BUTTON_DECK_2 pinMode(PIN_BUTTON_DECK_2, INPUT)



// Baud Rate와 통신하는 데이터 크기를 고려해서 적절한 Baud Rate 및 Period 설정 필요
#define BAUD_RATE 115200
#define COMMUNICATION_PERIOD_MICROSECONDS 5000



#endif