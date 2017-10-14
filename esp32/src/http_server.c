#include <esp_log.h>

#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/api.h>

#include "config.h"


static void http_server_serve(struct netconn* conn)
{
	err_t err;
	struct netbuf* buf;

	err = netconn_recv(conn, &buf);
	if (err == ERR_OK)
	{
		led_on = !led_on;
		netconn_write(conn, "OK", 2, NETCONN_NOCOPY);
//		ESP_LOGI(tag, "[http] cmd received.");
	}

	netconn_close(conn);

	netbuf_delete(buf);
}


void http_server(void* pvParameters)
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

