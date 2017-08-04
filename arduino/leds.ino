
int leds_state = 0xff;   // all off


#define LEDS_DELAY 100
unsigned long leds_last_access = 0;


void leds_setup()
{
}


void leds_run()
{
  unsigned long m = millis();
  if((unsigned long)(m - leds_last_access) < LEDS_DELAY) {
    return;
  }
  leds_last_access = m;

  leds_state = (leds_state + 1) % 2;

  
  Wire.beginTransmission(I2C_LEDS_ADDR);
  Wire.write(leds_state ? 0xff : 0x00);
  Wire.endTransmission();
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

