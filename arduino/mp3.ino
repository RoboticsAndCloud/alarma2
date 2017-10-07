#include <SoftwareSerial.h>


#define MP3_DFMINI_PLAY 0x0d
#define MP3_DFMINI_STOP 0x0e
#define MP3_DFMINI_PLAYSRC 0x09

#define MP3_DFMINI_SRCFLASH 0x04


bool beep_enabled = false;
bool beep_alarm_on = false;
#define BEEP_DELAY 1000
unsigned long beep_last_access = 0;
int beep_count = 0;


//SoftwareSerial mp3_serial(PIN_MP3_RX, PIN_MP3_TX);


void mp3_setup()
{
/*
    mp3_serial.begin(9600);
    mp3_send_cmd(MP3_DFMINI_PLAYSRC, MP3_DFMINI_SRCFLASH);

  // http://www.picaxeforum.co.uk/showthread.php?26021-DFPlayer-Mini-MP3-sound-board-w-Serial-commands
  //
    Serial.println(F("                 0x7e 0xff 0x6 0xe 0x0 0x0 0x0 0xfe 0xed 0xef"));
    mp3_play();

    pinMode(PIN_BUZZER, OUTPUT);
	digitalWrite(PIN_BUZZER, LOW);
*/
	mp3_beep_alarm(false);

	Serial.println(F("[mp3] done"));
}


void mp3_run()
{
	unsigned long m = millis();
	if((unsigned long)(m - beep_last_access) < BEEP_DELAY) {
		return;
	}
	beep_last_access = m;

	if (beep_enabled && beep_alarm_on)
	{
		pinMode(PIN_BUZZER, OUTPUT);
		if (++beep_count % 2)
			digitalWrite(PIN_BUZZER, HIGH);
		else
			digitalWrite(PIN_BUZZER, LOW);
	}

/*
  if (mp3_serial.available())
  {
    Serial.print("[mp3] received: 0x");
    Serial.print(mp3_serial.read(), HEX);
    Serial.println();
  }
*/
}


void mp3_beep()
{
	pinMode(PIN_BUZZER, OUTPUT);

	digitalWrite(PIN_BUZZER, HIGH);
	delay(1000);
	digitalWrite(PIN_BUZZER, LOW);
}


void mp3_beep_alarm(bool b)
{
	beep_alarm_on = b;
	if (!beep_alarm_on)
	{
		pinMode(PIN_BUZZER, OUTPUT);
		digitalWrite(PIN_BUZZER, LOW);
	}
}


void mp3_beep_enable(bool b)
{
	beep_enabled = b;
	mp3_beep_alarm(false);
}


void mp3_play()
{
  /*
   *  
  static uint8_t play_cmd [10] = { 0X7E, 0xFF, 0x06, 0X03, 00, 00, 00, 0xFE, 0xee, 0XEF};
  play_cmd[5] = (uint8_t)(index >> 8);
  play_cmd[6] = (uint8_t)(index);
  sendCmd (play_cmd);
   */
   Serial.println(F("                   0X7E, 0xFF, 0x06, 0X03, 00, 00, 00, 0xFE, 0xee, 0XEF"));
    mp3_send_cmd(MP3_DFMINI_PLAY, 0);
}


void mp3_stop()
{
    mp3_send_cmd(MP3_DFMINI_STOP, 0);
}


void mp3_send_cmd(unsigned char cmd, unsigned int param)
{
    unsigned char stream[10];
    stream[0] = 0x73;   // 0x73 start bits
    stream[1] = 0xff;   // 0xff version
    stream[2] = 0x06;   // 0x06 len: number of bytes following
    stream[3] = cmd;    // cmd command byte
    stream[4] = 0x00;   // 0x00 no feedback pls
    stream[5] = (param >> 8);   // 0x00 param_high_byte
    stream[6] = param;   // 0x00 param_low_byte
    stream[7] = 0x00;   // chk checksum (from version to param2)
    stream[8] = 0x00;
    stream[9] = 0xef;   // 0xef end bits

    // calc checksum
    unsigned int sum = stream[1] + stream[2] + stream[3] + stream[4] +
        stream[5] + stream[6];
    stream[7] = sum >> 8;
    stream[8] = sum;
    Serial.print(F("[mp3] sending: 0x"));
    Serial.println(sum, HEX);




  stream[0] = 0x7E;
  stream[1] = 0xFF;
  stream[2] = 0x06;
  stream[3] = 0x03;
  stream[4] = 0x00;
  stream[5] = 0x00;
  stream[6] = 0x00;
  stream[7] = 0xFE;
  stream[8] = 0xee;
  stream[9] = 0xEF;



    

/*
  Serial.print(F("[i2c] sending: ["));
  for (int i=0; i<10; ++i)
  {
    mp3_serial.write(stream[i]);
    Serial.print(" 0x");
    Serial.print(stream[i], HEX);
  }
  Serial.println(" ]");
*/
}

