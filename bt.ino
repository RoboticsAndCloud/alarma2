#include <SoftwareSerial.h>
#include "common.h"


SoftwareSerial bt_serial(PIN_BT_TX, PIN_BT_RX);


void bt_setup()
{
    bt_serial.begin(9600);
}


void bt_run()
{
    if (bt_serial.available())
    {
        cmd_add_char(bt_serial);
    }
}

