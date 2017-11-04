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


int led_on = 1;

static const char* tag = "ESP32ALARMA2";

extern void hcsr04_init();


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

	ESP_LOGI(tag, "Wifi ready.");
}



static void task_i2c_test()
{
	uint8_t val = 0xff;

	for (;;)
	{
		val = 0xff;
		i2c_master_write_slave(MY_LEDS_I2C_ADDR, &val, 1);

//		led_on = 0;
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		val = 0x00;
		i2c_master_write_slave(MY_LEDS_I2C_ADDR, &val, 1);

//		led_on = 1;
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		// read keypad
		i2c_master_read_slave(MY_KEYPAD_I2C_ADDR, &val, 1);
		ESP_LOGI(tag, "keypad: 0x%x", val);
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
		ESP_LOGI(tag, "[nvs] could not open storage");
		return;
	}

	err = nvs_get_i8(my_handle, CFG_BOOT_CNT, &bootcnt);
	if (err == ESP_OK)
		++bootcnt;

	err = nvs_set_i8(my_handle, CFG_BOOT_CNT, bootcnt);
	if (err != ESP_OK)
	{
		ESP_LOGI(tag, "[nvs] could not write");
		return;
	}

	err = nvs_commit(my_handle);
	if (err != ESP_OK)
	{
		ESP_LOGE(tag, "[nvs] coult not commit");
		return;
	}

	ESP_LOGI(tag, "[nvs] boot cnt: %d", bootcnt);
}


void app_main(void)
{
	esp_err_t err = nvs_flash_init();
	if (err == ESP_OK)
	{
		read_settings();
	}
	else
		ESP_LOGI(tag, "nvs returned with an error.");

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	/*
	hcsr04_init();

	for(;;);
	*/
//	wifi_init();

//	bt_gatt_server_init();
//
	buzzer_off();

	i2c_master_init();

//	xTaskCreate(&http_server, "http_server", 2048, NULL, 5, NULL);
//	xTaskCreate(&task_blinking_led, "blinking_led", 2048, NULL, 5, NULL);
	xTaskCreate(&task_i2c_test, "task_i2c_test", 2048, NULL, 5, NULL);
}

