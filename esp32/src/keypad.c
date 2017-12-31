#include "sdkconfig.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#include <esp_log.h>

#include "i2c.h"
#include "config.h"
#include "keypad.h"


static const char* M_TAG = "alarma2/keypad";
static SemaphoreHandle_t m_access_mutex;
static char m_key_pressed = KEYPAD_NO_INPUT;


static char key2char[16][2] = {
  {0x77, '1'}, {0x7b, '2'}, {0x7d, '3'}, {0x7e, 'A'},
  {0xb7, '4'}, {0xbb, '5'}, {0xbd, '6'}, {0xbe, 'B'},
  {0xd7, '7'}, {0xdb, '8'}, {0xdd, '9'}, {0xde, 'C'},
  {0xe7, '*'}, {0xeb, '0'}, {0xed, '#'}, {0xee, 'D'},
};



static uint8_t keypad_read()
{
	uint8_t val;
	i2c_master_read_slave(MY_KEYPAD_I2C_ADDR, &val, 1);
	return val;
}


static void keypad_write(uint8_t val)
{
	i2c_master_write_slave(MY_KEYPAD_I2C_ADDR, &val, 1);
}


static char keypad_poll_keys()
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



static void keypad_task(void* pvParameter)
{
	char key = -1;
	for (;;)
	{
		key = keypad_poll_keys();
		// pressed
		if (key != (char)-1)
		{
			// wait for release
			while (keypad_poll_keys() != (char)-1)
				vTaskDelay(100 / portTICK_PERIOD_MS);

			if (key != m_key_pressed)
			{
				xSemaphoreTake(m_access_mutex, portMAX_DELAY);
				   m_key_pressed = key;
				   xSemaphoreGive(m_access_mutex);

				ESP_LOGI(M_TAG, "new-key: %c", m_key_pressed);
			}
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}


char keypad_get_pressed()
{
	xSemaphoreTake(m_access_mutex, portMAX_DELAY);
	char val = m_key_pressed;
	m_key_pressed = KEYPAD_NO_INPUT;
	xSemaphoreGive(m_access_mutex);
	return val;
}


void keypad_init()
{
	m_access_mutex = xSemaphoreCreateMutex();
    xTaskCreate(keypad_task, "keypad_task", 2048, NULL, 10, NULL);
}
