#define PASSWD_LEN 4
int passwd[PASSWD_LEN] = { '1', '3', '8', '5' };
int passwd_pos = -1;
int passwd_try[PASSWD_LEN];


void game_run()
{
	int key = keypad_run();
	if (key != -1)
	{
		Serial.print("key: ");
		Serial.print((char)key);
		Serial.print(" ");
		Serial.println(passwd_pos);

		if (passwd_pos == -1)
		{
			if (key == 'A')
			{
				passwd_pos = 0;

				leds_set(0, true);
				leds_set(1, true);
				leds_set(2, true);
				leds_set(3, true);
			}
		}
		else if (passwd_pos < PASSWD_LEN)
		{
			leds_set(passwd_pos, false);

			passwd_try[passwd_pos++] = key;

			// all keys pressed
			if (passwd_pos == PASSWD_LEN)
			{
				delay(1 * 100);
				leds_off();
				delay(1 * 100);

				passwd_pos = -1;
				for (int i=0; i<PASSWD_LEN; ++i)
				{
					if (passwd_try[i] != passwd[i])
					{
						leds_set(0, true);
						leds_set(3, true);
						return;
					}
				}
				leds_set(1, true);
				leds_set(2, true);
			}
		}
	}
}
