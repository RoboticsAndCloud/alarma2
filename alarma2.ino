#include <Wire.h>


void setup()
{
    Serial.begin(9600);
    Wire.begin();

    leds_setup();
    Serial.println(F("[leds] done"));

    keypad_setup();
    Serial.println(F("[keypad] done"));

    mp3_setup();
    Serial.println("F[mp3] done");

    sonic_setup();
    Serial.println("F[sonic] done");
}


void loop()
{
    keypad_run();

    sonic_run();
}
