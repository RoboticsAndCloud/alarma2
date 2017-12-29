#include "sdkconfig.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <esp_log.h>

#include "config.h"
#include "leds.h"
#include "i2c.h"


static QueueHandle_t m_queue;
static const char* M_TAG = "leds";


void leds_on()
{
	uint8_t val = 0x00;
	xQueueSend(m_queue, &val, portMAX_DELAY);
}


void leds_off()
{
	uint8_t val = 0xff;
	xQueueSend(m_queue, &val, portMAX_DELAY);
}


void leds_task(void* pvParameter)
{
	uint8_t val;
	for (;;)
	{
		xQueueReceive(m_queue, &val, portMAX_DELAY);
		i2c_master_write_slave(MY_LEDS_I2C_ADDR, &val, 1);
	}
}

void leds_init()
{
	m_queue = xQueueCreate(10, sizeof(uint8_t));

	xTaskCreate(&leds_task, "leds_task", 2048, NULL, 5, NULL);
}
