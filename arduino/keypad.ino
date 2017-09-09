#include "common.h"


int key2char[16][2] = {
  {0x77, '1'}, {0x7b, '2'}, {0x7d, '3'}, {0x7e, 'A'},
  {0xb7, '4'}, {0xbb, '5'}, {0xbd, '6'}, {0xbe, 'B'},
  {0xd7, '7'}, {0xdb, '8'}, {0xdd, '9'}, {0xde, 'C'},
  {0xe7, '*'}, {0xeb, '0'}, {0xed, '#'}, {0xee, 'D'},
};


void keypad_setup()
{
}



void keypad_send(int state)
{
    Wire.beginTransmission(I2C_KEYPAD_ADDR);
    Wire.write(state);
    Wire.endTransmission();
}


int keypad_read()
{
    Wire.requestFrom(I2C_KEYPAD_ADDR, 1);
    if (Wire.available())
        return Wire.read();
    return 0xff;
}

void keypad_write(int val)
{
  Wire.beginTransmission(I2C_KEYPAD_ADDR);
  Wire.write(val);
  Wire.endTransmission();
}


int keypad_run()
{
	int key = keypad_poll_keys();
	// wait for release
	if (key != -1)
	{
		for (int i=0; i<50; ++i)
		{
			if (keypad_poll_keys() == -1)
				return key;
		}
	}
	return -1;
}


int keypad_poll_keys()
{
	int inbytes;
	int key;
 
  //Serial.println("Scanning...");
 
	keypad_write(0xf0);
	inbytes = keypad_read();
	if (inbytes != 0xf0)
	{
		key = inbytes;
		//i2c_print(address, inbytes >> 4);

		keypad_write(0x0f);
		inbytes = keypad_read();
        //if (inbytes != 0x0f)
          //i2c_print(address, inbytes);


		key |= inbytes;
		//i2c_print(address, key);

		for (int i=0; i<16; ++i)
		{
			if (key2char[i][0] == key)
			{
				return key2char[i][1];
			}
		}
	}

  return -1;
}
