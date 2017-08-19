/**
 * Copied from houston pic16 code.
 */

#define SDA A4
#define SDL A5

void slow_i2c_delay_50us()
{
//    delayMicroseconds(50);
    delay(10);
}

void slow_i2c_high_sda()
{
    pinMode(SDA, INPUT_PULLUP);
//    digitalWrite(SDA, HIGH);
    slow_i2c_delay_50us();
}
void slow_i2c_low_sda()
{
    pinMode(SDA, OUTPUT);
    digitalWrite(SDA, LOW);
    slow_i2c_delay_50us();
}
void slow_i2c_high_scl()
{
    pinMode(SCL, INPUT_PULLUP);
//    digitalWrite(SCL, HIGH);
    slow_i2c_delay_50us();
}
void slow_i2c_low_scl()
{
    pinMode(SCL, OUTPUT);
    digitalWrite(SCL, LOW);
    slow_i2c_delay_50us();
}

void slow_i2c_start()
{
    slow_i2c_low_scl();
    slow_i2c_high_sda();
    slow_i2c_high_scl();
    slow_i2c_low_sda();
    slow_i2c_low_scl();
}

void slow_i2c_stop()
{
    slow_i2c_low_scl();
    slow_i2c_low_sda();
    slow_i2c_high_scl();
    slow_i2c_high_sda();
    slow_i2c_low_scl();
}

void slow_i2c_nack()
{
    slow_i2c_high_sda();
    slow_i2c_clock_pulse();
}

void slow_i2c_clock_pulse()
{
    slow_i2c_high_scl();
    slow_i2c_low_scl();
}

void slow_i2c_send_byte(int data)
{
    Serial.print("[");
    for (int i=0; i<8; ++i)
    {
        Serial.print((data & 0x80) != 0, HEX);

        if (data & 0x80)
        {
            slow_i2c_high_sda();
            slow_i2c_clock_pulse();
        }
        else
        {
            slow_i2c_low_sda();
            slow_i2c_clock_pulse();
            slow_i2c_high_sda();
        }
        data <<= 1;
    }

    Serial.println("]");
}

void slow_i2c_send(int addr, int data)
{
    int ack = -1;

    Serial.println();

    slow_i2c_start();

    slow_i2c_send_byte((addr << 1) & 0xfe);
    // skip ack
    slow_i2c_nack();
//    ack = slow_i2c_clock();
//    Serial.print("ack: ");
//    Serial.println(ack);

    slow_i2c_send_byte(data);
    // skip ack
    slow_i2c_nack();
//    ack =slow_i2c_clock();
//    Serial.print("ack: ");
//    Serial.println(ack);

    slow_i2c_stop();
}
