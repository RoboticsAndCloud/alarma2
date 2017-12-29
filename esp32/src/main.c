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

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/gpio.h>

#include <lwip/netdb.h>

#include "config.h"
#include "http_server.h"
#include "bt.h"
#include "i2c.h"
#include "hcsr04.h"
#include "leds.h"
#include "keypad.h"


int led_on = 1;

static const char* M_TAG = "ESP32ALARMA2";

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


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


static void main_task(void* pvParameter)
{
	uint8_t val = 0xff;

	for (;;)
	{
		float distance = hcsr04_get_distance();
		char key = keypad_get_pressed();
		ESP_LOGI(M_TAG, "distance..: %.1f", distance);
		ESP_LOGI(M_TAG, "key.......: %c", key);

		leds_off();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		leds_on();
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}


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


static void buzzer_off(/*void* pvParameter*/)
{
	gpio_set_direction(GPIO_NUM_15, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_15, 0);
}


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


void app_main(void)
{
	ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	buzzer_off();
	hcsr04_init();
	i2c_master_init(GPIO_NUM_16, GPIO_NUM_17);

#if 0
	for (int i=0; i<127; ++i)
		if (i2c_master_scan(i) == ESP_OK)
			ESP_LOGI(M_TAG, "i2c-device @0x%x", i);
#endif

	keypad_init();
	leds_init();

//	wifi_init();
//	bt_gatt_server_init();
//


//	xTaskCreate(&http_server, "http_server", 2048, NULL, 5, NULL);
//	xTaskCreate(&task_blinking_led, "blinking_led", 2048, NULL, 5, NULL);
//	xTaskCreate(&task_i2c_test, "task_i2c_test", 2048, NULL, 5, NULL);
	xTaskCreate(&main_task, "main_task", 2048, NULL, 5, NULL);
}

