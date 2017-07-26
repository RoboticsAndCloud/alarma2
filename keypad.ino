#include "common.h"


const int ROWS_COUNT = 4;
const int COLS_COUNT = 4;

#define KEYPAD_INIT 0xf0
#define KEYPAD_COL(x) (1 << x)
#define KEYPAD_ROW(x) ((1 << x) << 4)


void keypad_setup()
{
    keypad_send(KEYPAD_INIT);
}



void keypad_send(int state)
{
    Wire.beginTransmission(I2C_KEYPAD_ADDR);
    Wire.write(state);
    Wire.endTransmission();
}


int keypad_read()
{
    Wire.requestFrom(I2C_KEYPAD_ADDR, 1);
    if (Wire.available())
        return Wire.read();
    return 0xff;
}


void keypad_run()
{
    int key_pressed = -1;
    for (int c=0; c<COLS_COUNT; ++c)
    {
        keypad_send(KEYPAD_COL(c));
        int state = keypad_read();
        for (int r=0; r<ROWS_COUNT; ++r)
        {
            if (state & KEYPAD_ROW(c))
            {
                Serial.print("push: c=");
                Serial.print(COLS_COUNT + c);
                Serial.print(" r=");
                Serial.println(ROWS_COUNT + r);
                key_pressed = c * COLS_COUNT + r;
            }
        }
        keypad_send(KEYPAD_INIT);
    }

    if (key_pressed != -1)
    {
        Serial.print("[keypad] ");
        Serial.println(key_pressed);
    }
}
