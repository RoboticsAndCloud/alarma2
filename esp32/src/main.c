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
#include "bt.h"
#include "i2c.h"
#include "hcsr04.h"
#include "leds.h"
#include "keypad.h"
#include "bt_rfcomm.h"


int led_on = 1;

static const char* M_TAG = "ESP32ALARMA2";

static const char* M_PASSWORD = "123";

static SemaphoreHandle_t m_bt_access;


esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


#if 0
static void wifi_init()
{
    tcpip_adapter_init();

	tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA); // no DHCP

	tcpip_adapter_ip_info_t ipInfo;
	inet_pton(AF_INET, CONFIG_ADDRESS, &ipInfo.ip);
	inet_pton(AF_INET, CONFIG_GATEWAY, &ipInfo.gw);
	inet_pton(AF_INET, CONFIG_NETMASK, &ipInfo.netmask);
	tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = CONFIG_SSID,
            .password = CONFIG_PASSWORD,
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );

	ESP_LOGI(M_TAG, "Wifi ready.");
}
#endif


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

	// next X keys are for us
	for (int i=0; i<strlen(M_PASSWORD); ++i)
	{
		while (key == KEYPAD_NO_INPUT)
		{
			vTaskDelay(10 / portTICK_PERIOD_MS);
			key = keypad_get_pressed();
		}

		ESP_LOGI(M_TAG, "do-activate-check: %c == %c", M_PASSWORD[i], key);
		if (M_PASSWORD[i] != key)
			return false;

		key = KEYPAD_NO_INPUT;
	}

	return true;
}


static void main_task(void* pvParameter)
{
	leds_mode(MY_LEDS_MODE_OFF);

	bool activated = false;
	float alarm_distance = 0.0;

	ESP_LOGI(M_TAG, "Ready");
	for (;;)
	{
		float distance = hcsr04_get_distance();
		char key = keypad_get_pressed();
//		ESP_LOGI(M_TAG, "distance..: %.1f", distance);
//		ESP_LOGI(M_TAG, "key.......: %c", key);

		if (activated && distance < alarm_distance)
		{
			buzzer(true);
			leds_mode(MY_LEDS_MODE_ALARM);
		}

		switch (key)
		{
		case 'A': // activate
			ESP_LOGI(M_TAG, "A");
			if (!activated)
			{
				leds_mode(MY_LEDS_MODE_INPUT);
				if (check_password())
				{
					activated = true;
					alarm_distance = distance - 10.0;
					leds_mode(MY_LEDS_MODE_ACTIVATED);
				}
				else
					leds_mode(MY_LEDS_MODE_ERROR);
			}
			break;
		case 'B':
			ESP_LOGI(M_TAG, "B");
			if (!activated)
			{
				buzzer(true);
				vTaskDelay(250 / portTICK_PERIOD_MS);
				buzzer(false);
			}
			break;
		case 'C':
			ESP_LOGI(M_TAG, "C");
			if (!activated)
			{
				leds_mode(MY_LEDS_MODE_DEMO);
			}
			break;
		case 'D':
			ESP_LOGI(M_TAG, "D");
			if (activated)
			{
				if (check_password())
				{
					activated = false;
					buzzer(false);
					leds_mode(MY_LEDS_MODE_OFF);
				}
			}
			break;
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


#if 0
static void task_blinking_led(void* pvParameter)
{
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

	gpio_set_level(GPIO_NUM_2, 1);
	vTaskDelay(500 / portTICK_PERIOD_MS);
	gpio_set_level(GPIO_NUM_2, 0);
	vTaskDelay(500 / portTICK_PERIOD_MS);
	gpio_set_level(GPIO_NUM_2, 1);
	vTaskDelay(500 / portTICK_PERIOD_MS);
	gpio_set_level(GPIO_NUM_2, 0);
	vTaskDelay(500 / portTICK_PERIOD_MS);

	led_on = 0;
	while (true)
	{
		gpio_set_level(GPIO_NUM_2, led_on);
		vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}
#endif


void read_settings()
{
	nvs_handle my_handle;
	const char* CFG_BOOT_CNT = "bootcnt";
	int8_t bootcnt = 0;
	esp_err_t err;

	err = nvs_open("storage", NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
	{
		ESP_LOGI(M_TAG, "[nvs] could not open storage");
		return;
	}

	err = nvs_get_i8(my_handle, CFG_BOOT_CNT, &bootcnt);
	if (err == ESP_OK)
		++bootcnt;

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
		m_bt_buffer[0] = 'A';
		break;
	case 'b':
		m_bt_buffer[0] = 'B';
		break;
	case 'c':
		m_bt_buffer[0] = 'C';
		break;
	case 'd':
		m_bt_buffer[0] = 'D';
		break;
	default:
		break;
	}

	return m_bt_buffer;
}


void app_main(void)
{
	ESP_ERROR_CHECK(nvs_flash_init());
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

//	wifi_init();
//	bt_gatt_server_init();
//


//	xTaskCreate(&http_server, "http_server", 2048, NULL, 5, NULL);
//	xTaskCreate(&task_blinking_led, "blinking_led", 2048, NULL, 5, NULL);
//	xTaskCreate(&task_i2c_test, "task_i2c_test", 2048, NULL, 5, NULL);
	xTaskCreate(&main_task, "main_task", 2048, NULL, 5, NULL);
}

