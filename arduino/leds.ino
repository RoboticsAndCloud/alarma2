
int leds_state = 0xff;   // all off


#define LEDS_DELAY 100
unsigned long leds_last_access = 0;


bool led_alarm_is_on = false;
int led_alarm_blinking = 0;


void leds_setup()
{
	leds_off();
}


void leds_run()
{
	unsigned long m = millis();
	if((unsigned long)(m - leds_last_access) < LEDS_DELAY) {
		return;
	}

	leds_last_access = m;

	if (led_alarm_is_on)
	{
		if (led_alarm_blinking++ % 2)
		{
			leds_set(4, true);
			leds_set(7, true);
		}
		else
		{
			leds_set(4, false);
			leds_set(7, false);
		}
	}
}


void leds_off()
{
	leds_state = 0xff;

	Wire.beginTransmission(I2C_LEDS_ADDR);
	Wire.write(leds_state);
	Wire.endTransmission();
}


void leds_set(int led, bool on)
{
    int prev_state = leds_state;

    if (on)
        leds_state &= ~(1 << led);
    else
        leds_state |= (1 << led);

	if (prev_state != leds_state)
	{
		Wire.beginTransmission(I2C_LEDS_ADDR);
		Wire.write(leds_state);
		Wire.endTransmission();
	}
}


void led_alarm()
{
    Wire.beginTransmission(I2C_LEDS_ADDR);
    Wire.write(0x00);
    Wire.endTransmission();
}


void led_alarm(bool b)
{
	led_alarm_is_on = b;
}
