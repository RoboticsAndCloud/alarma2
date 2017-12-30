#ifndef ALARMA2_LEDS_H
#define ALARMA2_LEDS_H


void leds_init();


#define MY_LEDS_MODE_OFF		0
#define MY_LEDS_MODE_ON			1
#define MY_LEDS_MODE_ERROR		2
#define MY_LEDS_MODE_ACTIVATED	3
#define MY_LEDS_MODE_INPUT		4
#define MY_LEDS_MODE_ALARM		5

void leds_mode(uint8_t mode);


void leds_off();


#endif
