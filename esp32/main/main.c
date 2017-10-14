#include <freertos/FreeRTOS.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <nvs_flash.h>
#include <driver/gpio.h>

#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/api.h>

#include "config.h"


static bool led_on = true;


esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


static void http_server_serve(struct netconn* conn)
{
	err_t err;
	struct netbuf* buf;

	err = netconn_recv(conn, &buf);
	if (err == ERR_OK)
	{
		led_on = !led_on;
		netconn_write(conn, "OK", 2, NETCONN_NOCOPY);
	}

	netconn_close(conn);

	netbuf_delete(buf);
}


static void http_server(void* pvParameters)
{
	struct netconn* conn;
	struct netconn* newconn;
	err_t err;

	conn = netconn_new(NETCONN_TCP);
	netconn_bind(conn, NULL, 80);
	netconn_listen(conn);

	do
	{
		err = netconn_accept(conn, &newconn);
		if (err == ERR_OK)
		{
			http_server_serve(newconn);
			netconn_delete(newconn);
		}
	}
	while (err == ERR_OK);

	netconn_close(conn);
	netconn_delete(conn);
}


void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
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

	// http server
	xTaskCreate(&http_server, "http_server", 2048, NULL, 5, NULL);

	// blink
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    while (true)
	{
        gpio_set_level(GPIO_NUM_2, led_on);
        vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}

