#include "sdkconfig.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <esp_log.h>

#include "config.h"
#include "leds.h"
#include "i2c.h"


static SemaphoreHandle_t m_access_mutex;
static uint8_t m_mode = 0;
static const char* M_TAG = "leds";


void leds_mode(uint8_t mode)
{
	xSemaphoreTake(m_access_mutex, portMAX_DELAY);
	m_mode = mode;
	xSemaphoreGive(m_access_mutex);
}


static inline void i2c_send(uint8_t val)
{
	i2c_master_write_slave(MY_LEDS_I2C_ADDR, &val, 1);
}


static uint8_t get_mode()
{
	uint8_t mode;
	xSemaphoreTake(m_access_mutex, portMAX_DELAY);
	mode = m_mode;
	xSemaphoreGive(m_access_mutex);
	return mode;
}


void leds_task(void* pvParameter)
{
	uint8_t prev_mode = -1;
	uint8_t mode = -1;
	for (;;)
	{
		prev_mode = mode;
		xSemaphoreTake(m_access_mutex, portMAX_DELAY);
		mode = m_mode;
		xSemaphoreGive(m_access_mutex);

		switch (m_mode)
		{
		case MY_LEDS_MODE_OFF:
			i2c_send(0xff);
			break;
		case MY_LEDS_MODE_ON:
			i2c_send(0x00);
			break;
		case MY_LEDS_MODE_ERROR:
			while (mode == get_mode())
			{
				i2c_send(0xff);
				vTaskDelay(200 / portTICK_PERIOD_MS);
				i2c_send(0x00);
				vTaskDelay(200 / portTICK_PERIOD_MS);
			}
			break;
		}
	}
}

void leds_init()
{
	m_access_mutex = xSemaphoreCreateMutex();

	xTaskCreate(&leds_task, "leds_task", 2048, NULL, 5, NULL);
}
