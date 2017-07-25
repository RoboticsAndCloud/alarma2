#include "common.h"


const int ROWS_FROM = PIN_KEYPAD_START;
const int ROWS_COUNT = 4;
const int COLS_FROM = 6;
const int COLS_COUNT = 4;


void keypad_setup()
{
    for (int i=0; i<ROWS_COUNT; ++i)
        pinMode(ROWS_FROM + i, INPUT_PULLUP);
    for (int i=0; i<COLS_COUNT; ++i)
        pinMode(COLS_FROM + i, OUTPUT);
}


void keypad_run()
{
    int key_pressed = -1;
    for (int c=0; c<COLS_COUNT; ++c)
    {
        digitalWrite(COLS_FROM + c, HIGH);
        for (int r=0; r<ROWS_COUNT; ++r)
        {
            if (digitalRead(ROWS_FROM + r) == HIGH)
            {
                Serial.print("push: c=");
                Serial.print(COLS_COUNT + c);
                Serial.print(" r=");
                Serial.println(ROWS_COUNT + r);
                key_pressed = c * COLS_COUNT + r;
            }
        }
        digitalWrite(COLS_FROM + c, LOW);
    }

    if (key_pressed != -1)
    {
        Serial.print("[keypad] ");
        Serial.println(key_pressed);
    }
}
