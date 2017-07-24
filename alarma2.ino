void setup()
{
    Serial.begin(9600);

    leds_setup();
    Serial.println(F("[leds] done"));

    keypad_setup();
    Serial.println(F("[keypad] done"));
}


void loop()
{
    keypad_run();
}
