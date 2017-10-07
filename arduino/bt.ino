#include <SoftwareSerial.h>
#include "common.h"



#ifdef MY_USE_BT
SoftwareSerial bt_serial(PIN_BT_RX, PIN_BT_TX);


void bt_setup()
{
    bt_serial.begin(9600);

	Serial.println(F("[bt] done"));
}


void bt_run()
{
	if (bt_serial.available())
	{
		cmd_add_char(bt_serial);
	}
}
#endif

