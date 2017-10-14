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
#include "bt_gatt_server.h"


int led_on = 1;

static const char* tag = "ESP32ALARMA2";


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


static void task_blinking_led(void* pvParameter)
{
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

	gpio_set_level(GPIO_NUM_2, 1);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	gpio_set_level(GPIO_NUM_2, 0);
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	while (true)
	{
		gpio_set_level(GPIO_NUM_2, led_on);
		vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}


void btstack_main(void)
{
    nvs_flash_init();

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	wifi_init();

	bt_gatt_server_init();

	xTaskCreate(&http_server, "http_server", 2048, NULL, 5, NULL);
	xTaskCreate(&task_blinking_led, "blinking_led", 2048, NULL, 5, NULL);
}

