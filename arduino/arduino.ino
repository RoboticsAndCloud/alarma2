#include <Wire.h>
#include <avr/wdt.h>
#include "common.h"

void setup()
{
	Serial.begin(9600);
    Wire.begin();

    leds_setup();
    Serial.println(F("[leds] done"));

    keypad_setup();
    Serial.println(F("[keypad] done"));

	mp3_setup();

    sonic_setup();
    Serial.println(F("[sonic] done"));

	bt_setup();

#ifdef MY_USE_WDT
    wdt_enable(WDTO_8S);
    Serial.println(F("[wdt] on"));
#endif
}




void loop()
{
	mp3_beep();
	return;

	game_run();

    sonic_run();

	bt_run();

    leds_run();

//    mp3_run();

	if(Serial.available())
	{
    //if(Serial.read() == 'm')
      //sound_play_debug(2);
		cmd_add_char(Serial);
	}
/*
#ifdef MY_USE_WDT
    wdt_reset();
#endif
*/
}

