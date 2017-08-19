#include <Wire.h>
#include <avr/wdt.h>
#include "common.h"

void setup()
{
    Serial.begin(9600);

/*
    // very ... slow
    Wire.begin();
    Wire.setClock(31L * 1000L); // 31 .. 400
    TWBR = 40;
    TWSR |= bit (TWPS0);
    TWSR |= bit (TWPS1);

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

#ifdef MY_USE_WDT
    wdt_enable(WDTO_8S);
    Serial.println(F("[wdt] on"));
#endif
*/
}


extern bool i2c_write_byte(bool, bool, unsigned char);
int ack;
void loop()
{
    Serial.println("loop");

/*
    pinMode(A4, OUTPUT);
    digitalWrite(A4, HIGH);
    pinMode(A5, OUTPUT);
    digitalWrite(A5, HIGH);

    for (int i=0; i<128; ++i)
    {
        ack = i2c_write_byte(true, true, (i<<1) & 0xfe);
        if (ack == 0)
        {
            Serial.print("found: ");
            Serial.println(i, HEX);
        }
    }
    Serial.println("done");
    delay(1000);
    return;

    ack = i2c_write_byte(true, false, (I2C_LEDS_ADDR<<1) & 0xfe);
    Serial.println(ack, HEX);
    ack = i2c_write_byte(false, true, 0xff);
    Serial.println(ack, HEX);
    delay(1000);

    ack = i2c_write_byte(true, false, (I2C_LEDS_ADDR<<1) & 0xfe);
    Serial.println(ack, HEX);
    ack = i2c_write_byte(false, true, 0x00);
    Serial.println(ack, HEX);
    delay(1000);

    */

    slow_i2c_send(I2C_LEDS_ADDR, 0xff);
    delay(2000);
    slow_i2c_send(I2C_LEDS_ADDR, 0x00);
    delay(2000);

    return;

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

#ifdef MY_USE_WDT
    wdt_reset();
#endif
}

