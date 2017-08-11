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


void keypad_run()
{
  byte error, address;
  int nDevices;
  int inbytes;
  int key;
 
  //Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      /*
      Serial.print(F("[device] I2C device found at address 0x"));
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
      */

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
            Serial.print("key: ");
            Serial.println((char)key2char[i][1]);
          }
        }

        delay(1000);
        Serial.println();
      }
        
      
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print(F("[i2c] Unknown error at address 0x"));
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
}
