# esp32

Right now, we are toggling an LED only.

## Features

* BT - SPP (serial communication)
* WiFi - Returns OK

## sdkconfig

Required component configuration.

```
CONFIG_BT_ENABLED=y
CONFIG_CLASSIC_BT_ENABLED=y
CONFIG_WIFI_ENABLED=n
CONFIG_BT_SPP_ENABLED=y
````

## config.h

```c
#ifndef _CONFIG_H_
#define _CONFIG_H_

#define CONFIG_SSID		"ssid"
#define CONFIG_PASSWORD	"pass"
#define CONFIG_ADDRESS 	"192.168.1.52"
#define CONFIG_GATEWAY	"192.168.1.1"
#define CONFIG_NETMASK	"255.255.255.0"

extern int led_on;

#endif
```

## CMakeFile

Use this to import the project in QtCreator, sort of...

## Bluetooth

Default password: 123

Return value is capital letter on success or 'E' on error.

`a????` - activate. Requires password.
`b` - buzzer. Only when deactivated.
`c` - NOP.
`d????` - deactivate. Requires password.
`p?????` - new password. Only when activated.

