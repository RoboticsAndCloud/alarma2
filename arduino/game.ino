extern int g_sonic_distance;



#define PASSWD_LEN 4
int passwd[PASSWD_LEN] = { '1', '3', '8', '5' };
int passwd_pos = -1;
int passwd_try[PASSWD_LEN + 1];


#define GAME_KEY_ACTIVE 'A'
#define GAME_KEY_DEACTIVE 'D'
#define GAME_KEY_BEEP 'B'


bool game_activated = false;
bool game_show_alarm = false;


int game_min_distance = -1;

int game_key_pressed = -1;


void game_run()
{
	if (game_activated)
	{
		if (g_sonic_distance < game_min_distance)
		{
			if (game_show_alarm)
			{
				led_alarm(true);
				mp3_beep_alarm(true);

				game_show_alarm = false;
				Serial.print(F("[game] ALARM @"));
				Serial.println(g_sonic_distance);
			}
		}
	}















	int key = keypad_run();
	if (key == -1)
		return;

	Serial.print("key: ");
	Serial.print((char)key);
	Serial.print(" ");
	Serial.println(passwd_pos);

	if (passwd_pos == -1)
	{
		if (key == GAME_KEY_ACTIVE || key == GAME_KEY_DEACTIVE)
		{
			passwd_pos = 0;
			game_key_pressed = key;

			leds_set(0, true);
			leds_set(1, true);
			leds_set(2, true);
			leds_set(3, true);
		}
		else if (key == GAME_KEY_BEEP)
		{
			mp3_beep_enable(true);
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

			switch(game_key_pressed)
			{
			case GAME_KEY_ACTIVE:
				game_activate();
				break;
			case GAME_KEY_DEACTIVE:
				game_deactivate();
				break;
			}

			game_key_pressed = -1;
		}
	}
}


void game_activate()
{
	leds_off();
	leds_set(1, true);
	leds_set(2, true);

	game_activated = true;
	game_show_alarm = true;

	game_min_distance = g_sonic_distance - 10;
	Serial.print(F("[game] ACTIVATED @"));
	Serial.println(game_min_distance);

	led_alarm(false);
	mp3_beep_alarm(false);

	mp3_beep();
}


void game_deactivate()
{
	leds_off();

	game_activated = false;
	game_show_alarm = false;

	led_alarm(false);
	mp3_beep_alarm(false);

	mp3_beep();

	Serial.println(F("[game] DEACTIVATED"));
}
