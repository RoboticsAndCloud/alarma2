/**
 *
 * Wifi:
 * https://github.com/cmmakerclub/esp32-webserver/tree/master
 *
 *
 * Bluetooth:
 * https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/ble_adv
 *
 */
#include "sdkconfig.h"

#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/gpio.h>

#include <lwip/netdb.h>
#include <lwip/sockets.h>

#include <tcpip_adapter.h>

#include "config.h"
#include "http_server.h"
#include "i2c.h"
#include "hcsr04.h"
#include "leds.h"
#include "keypad.h"
#include "bt_rfcomm.h"


int led_on = 1;

static const char* M_TAG = "ESP32ALARMA2";

static char* m_password = "123";

static SemaphoreHandle_t m_bt_access;
static char m_bt_action = 'x';


esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


static void buzzer(bool on)
{
	gpio_pad_select_gpio(GPIO_NUM_15);
	gpio_set_direction(GPIO_NUM_15, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_15, on ? 1 : 0);
}


/**
 * FIXME: need a timeout here
 */
static bool check_password()
{
	ESP_LOGI(M_TAG, "do-activate");
	char key = KEYPAD_NO_INPUT;
	bool okay = true;

	// next X keys are for us
	for (int i=0; i<strlen(m_password); ++i)
	{
		while (key == KEYPAD_NO_INPUT)
		{
			vTaskDelay(10 / portTICK_PERIOD_MS);
			key = keypad_get_pressed();
		}

		ESP_LOGI(M_TAG, "do-activate-check: %c == %c", m_password[i], key);
		if (m_password[i] != key)
			okay = false;

		key = KEYPAD_NO_INPUT;
	}

	return okay;
}



bool m_activated = false;
float m_alarm_distance = 0.0;
bool m_in_alarm = false;


static void main_activate(float distance)
{
	m_activated = true;
	m_in_alarm = false;
	m_alarm_distance = distance - 10.0;
	leds_mode(MY_LEDS_MODE_ACTIVATED);
}


static void main_deactivate()
{
	m_activated = false;
	buzzer(false);
	leds_mode(MY_LEDS_MODE_OFF);
}


static void main_task(void* pvParameter)
{
	leds_mode(MY_LEDS_MODE_OFF);

	ESP_LOGI(M_TAG, "Ready");
	for (;;)
	{
		float distance = hcsr04_get_distance();
		char key = keypad_get_pressed();
//		ESP_LOGI(M_TAG, "distance..: %.1f", distance);
//		ESP_LOGI(M_TAG, "key.......: %c", key);

		if (m_activated && distance < m_alarm_distance)
		{
			if (!m_in_alarm)
			{
				buzzer(true);
				leds_mode(MY_LEDS_MODE_ALARM);
			}
		}

		switch (key)
		{
		case 'A': // activate
			ESP_LOGI(M_TAG, "A");
			if (!m_activated)
			{
				leds_mode(MY_LEDS_MODE_INPUT);
				if (check_password())
					main_activate(distance);
				else
					leds_mode(MY_LEDS_MODE_ERROR);
			}
			break;
		case 'B':
			ESP_LOGI(M_TAG, "B");
			if (!m_activated)
			{
				buzzer(true);
				vTaskDelay(250 / portTICK_PERIOD_MS);
				buzzer(false);
			}
			break;
		case 'C':
			ESP_LOGI(M_TAG, "C");
			if (!m_activated)
			{
				leds_mode(MY_LEDS_MODE_DEMO);
			}
			break;
		case 'D':
			ESP_LOGI(M_TAG, "D");
			if (m_activated && check_password())
				main_deactivate();
			break;
		}

		switch (m_bt_action)
		{
		case 'a':
			m_bt_action = 'x';
			main_activate(distance);
			break;
		case 'b':
			m_bt_action = 'x';
			if (!m_activated)
			{
				buzzer(true);
				vTaskDelay(250 / portTICK_PERIOD_MS);
				buzzer(false);
			}
			break;
		case 'd':
			m_bt_action = 'x';
			main_deactivate();
			break;
		case 'p':
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			esp_restart();
		}

		vTaskDelay(10 / portTICK_PERIOD_MS);

#if 0
		leds_mode(MY_LEDS_MODE_OFF);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		leds_mode(MY_LEDS_MODE_ON);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
	}
}


static const char* CFG_BOOT_CNT = "bootcnt";
static const char* CFG_PASSWORD = "password";

void read_settings()
{
	nvs_handle my_handle;
	int8_t bootcnt = 0;
	esp_err_t err;

	ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));

	err = nvs_get_i8(my_handle, CFG_BOOT_CNT, &bootcnt);
	if (err == ESP_OK)
		++bootcnt;

	size_t required_size;
	if (nvs_get_str(my_handle, CFG_PASSWORD, NULL, &required_size) == ESP_OK)
	{
		m_password = malloc(required_size);
		nvs_get_str(my_handle, CFG_PASSWORD, m_password, &required_size);
	}

	err = nvs_set_i8(my_handle, CFG_BOOT_CNT, bootcnt);
	if (err != ESP_OK)
	{
		ESP_LOGI(M_TAG, "[nvs] could not write");
		return;
	}

	err = nvs_commit(my_handle);
	if (err != ESP_OK)
	{
		ESP_LOGE(M_TAG, "[nvs] coult not commit");
		return;
	}

	ESP_LOGI(M_TAG, "[nvs] boot cnt: %d", bootcnt);
	ESP_LOGI(M_TAG, "[nvs] password: %s", m_password);
}


#define BTMAXBUF 40
static uint8_t m_bt_buffer[BTMAXBUF];
static uint8_t* bt_parser(uint8_t* data, uint16_t len, uint16_t* out_len)
{
	if (len == 0)
		return NULL;

	m_bt_buffer[0] = 'E';
	m_bt_buffer[1] = '\n';
	*out_len = 2;

	switch (data[0])
	{
	case 'a':
		if (len > strlen(m_password))
		{
			for (int i=0; i<strlen(m_password); ++i)
				if (data[i+1] != m_password[i])
					return m_bt_buffer;
			m_bt_buffer[0] = 'A';
			m_bt_action = 'a';
		}
		break;
	case 'b':
		m_bt_buffer[0] = 'B';
		m_bt_action = 'b';
		break;
	case 'c':
		m_bt_buffer[0] = 'C';
		m_bt_action = 'c';
		break;
	case 'd':
		if (len > strlen(m_password))
		{
			for (int i=0; i<strlen(m_password); ++i)
				if (data[i+1] != m_password[i])
					return m_bt_buffer;
			m_bt_buffer[0] = 'D';
			m_bt_action = 'd';
		}
		break;
	case 'p':
		if (m_activated && len > 2)
		{
			// terminate password with a NULL
			char* newpasswd = (char*)&data[1];
			for (int i=1; i<len; ++i)
			{
				if (newpasswd[i] < '0' || newpasswd[i] > '9')
				{
					newpasswd[i] = 0;
					break;
				}
			}

			if (strlen(newpasswd) > 0)
			{
				nvs_handle my_handle;
				ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &my_handle));
				nvs_set_str(my_handle, CFG_PASSWORD, newpasswd);

				ESP_LOGI(M_TAG, "new-password: %s", newpasswd);
				m_bt_buffer[0] = 'P';
				m_bt_action = 'p';
			}
		}
		break;
	default:
		break;
	}

	return m_bt_buffer;
}


void app_main(void)
{
	ESP_ERROR_CHECK(nvs_flash_init());
	read_settings();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    for (int i=0; i<3; ++i)
    {
		buzzer(true);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		buzzer(false);
		vTaskDelay(100 / portTICK_PERIOD_MS);
    }

	hcsr04_init();
	i2c_master_init(GPIO_NUM_16, GPIO_NUM_17);

#if 0
	for (int i=0; i<127; ++i)
		if (i2c_master_scan(i) == ESP_OK)
			ESP_LOGI(M_TAG, "i2c-device @0x%x", i);
#endif

	keypad_init();
	leds_init();

	m_bt_access = xSemaphoreCreateMutex();
	bt_rfcomm_init(M_TAG, &bt_parser);

	xTaskCreate(&main_task, "main_task", 2048, NULL, 5, NULL);
}

