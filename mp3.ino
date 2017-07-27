#include <SoftwareSerial.h>


#define MP3_DFMINI_PLAY 0x0d
#define MP3_DFMINI_STOP 0x0e
#define MP3_DFMINI_PLAYSRC 0x09

#define MP3_DFMINI_SRCFLASH 0x04


SoftwareSerial mp3_serial(PIN_MP3_RX, PIN_MP3_TX);


void mp3_setup()
{
    mp3_serial.begin(9600);
    mp3_send_cmd(MP3_DFMINI_PLAYSRC, MP3_DFMINI_SRCFLASH);

    pinMode(PIN_BUZZER, OUTPUT);
}


void mp3_beep()
{
    digitalWrite(PIN_BUZZER, HIGH);
    delay(100);
    digitalWrite(PIN_BUZZER, LOW);
}


void mp3_play()
{
    mp3_send_cmd(MP3_DFMINI_PLAY, 0);
}


void mp3_stop()
{
    mp3_send_cmd(MP3_DFMINI_STOP, 0);
}


void mp3_send_cmd(unsigned char cmd, unsigned int param)
{
    unsigned char stream[9];
    stream[0] = 0x73;   // 0x73 start bits
    stream[1] = 0xff;   // 0xff version
    stream[2] = 0x06;   // 0x06 len: number of bytes following
    stream[3] = cmd;    // cmd command byte
    stream[4] = 0x00;   // 0x00 no feedback pls
    stream[5] = (param >> 8);   // 0x00 param_high_byte
    stream[6] = param;   // 0x00 param_low_byte
    stream[7] = 0x00;   // chk checksum (from version to param2)
    stream[8] = 0xef;   // 0xef end bits

    // calc checksum
    unsigned char sum = stream[1] + stream[2] + stream[3] + stream[4] +
        stream[5] + stream[6];
    Serial.println(sum);
}

