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
    Serial.println(F("[mp3] done"));

    sonic_setup();
    Serial.println(F("[sonic] done"));

    bt_setup();
    Serial.println(F("[bt] done"));
}


void loop()
{
    keypad_run();

    sonic_run();

    bt_run();

    leds_run();

    mp3_run();

    if(Serial.available())
    {
    //if(Serial.read() == 'm')
      //sound_play_debug(2);
        cmd_add_char(Serial);
    }
}

