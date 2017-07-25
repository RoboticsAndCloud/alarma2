
int leds_state = 0xff;   // all off


void leds_setup()
{
}


void leds_set(int led, bool on)
{
    int prev_state = leds_state;

    if (on)
        leds_state &= ~(led << 1);
    else
        leds_state |= (led << 1);

    if (leds_state != prev_state)
    {
        Wire.beginTransmission(I2C_LEDS_ADDR);
        Wire.write(leds_state);
        Wire.endTransmission();
    }
}

