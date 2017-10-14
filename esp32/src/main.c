/**
 *
 * Wifi:
 * https://github.com/cmmakerclub/esp32-webserver/tree/master/main
 *
 *
 * Bluetooth:
 * https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/ble_adv/main/app_bt.c
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

#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/api.h>

#include <bt.h>

#include "config.h"


static bool led_on = true;

static const char* tag = "ESP32ALARMA2";


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
		ESP_LOGI(tag, "[http] cmd received.");
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


/*
 * @brief: BT controller callback function, used to notify the upper layer that
 *         controller is ready to receive command
 */
static void controller_rcv_pkt_ready(void)
{
    printf("controller rcv pkt ready\n");
}

/*
 * @brief: BT controller callback function, to transfer data packet to upper
 *         controller is ready to receive command
 */
static int host_rcv_pkt(uint8_t *data, uint16_t len)
{
    printf("host rcv pkt: ");
    for (uint16_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
    return 0;
}

static esp_vhci_host_callback_t vhci_host_cb = {
    controller_rcv_pkt_ready,
    host_rcv_pkt
};

/*
 * @brief: send HCI commands to perform BLE advertising;
 */
void bleAdvtTask(void *pvParameters)
{
	int cmd_cnt = 0;
	bool send_avail = false;
	esp_vhci_host_register_callback(&vhci_host_cb);
	printf("BLE advt task start\n");
	while (1)
	{
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        send_avail = esp_vhci_host_check_send_available();
        if (send_avail)
		{
			/*
            switch (cmd_cnt) {
            case 0: hci_cmd_send_reset(); ++cmd_cnt; break;
            case 1: hci_cmd_send_ble_set_adv_param(); ++cmd_cnt; break;
            case 2: hci_cmd_send_ble_set_adv_data(); ++cmd_cnt; break;
            case 3: hci_cmd_send_ble_adv_start(); ++cmd_cnt; break;
            }
			*/
			printf("BLE Advertise, flag_send_avail: %d, cmd_sent: %d\n", send_avail, cmd_cnt);
        }
	}
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


static void bt_init()
{
	esp_err_t ret;

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    
    ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if (ret) {
        ESP_LOGI(tag, "Bluetooth controller release classic bt memory failed");
        return;
    }

    if (esp_bt_controller_init(&bt_cfg) != ESP_OK) {
        ESP_LOGI(tag, "Bluetooth controller initialize failed");
        return;
    }

    if (esp_bt_controller_enable(ESP_BT_MODE_BLE) != ESP_OK) {
        ESP_LOGI(tag, "Bluetooth controller enable failed");
        return;
    }

    /*
     * If call mem release here, also work. Input ESP_BT_MODE_CLASSIC_BT, the function will
     * release the memory of classic bt mode.
     * esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
     *
     */

    /*
     * If call mem release here, also work. Input ESP_BT_MODE_BTDM, the function will calculate
     * that the BLE mode is already used, so it will release of only classic bt mode.
     * esp_bt_controller_mem_release(ESP_BT_MODE_BTDM);
     */

    xTaskCreatePinnedToCore(&bleAdvtTask, "bleAdvtTask", 2048, NULL, 5, NULL, 0);

	ESP_LOGI(tag, "BLE ready.");
}



static void task_blinking_led(void* pvParameter)
{
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);

	gpio_set_level(GPIO_NUM_2, true);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	gpio_set_level(GPIO_NUM_2, false);
	vTaskDelay(1000 / portTICK_PERIOD_MS);

	while (true)
	{
		gpio_set_level(GPIO_NUM_2, led_on);
		vTaskDelay(300 / portTICK_PERIOD_MS);
    }
}


void app_main(void)
{
    nvs_flash_init();

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	wifi_init();
	xTaskCreate(&http_server, "http_server", 2048, NULL, 5, NULL);

	bt_init();

	xTaskCreate(&task_blinking_led, "blinking_led", 2048, NULL, 5, NULL);
}

