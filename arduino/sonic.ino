#include "common.h"


#define ALARM_MAX 1000

#define SONIC_DELAY 200
unsigned long sonic_last_access = 0;


int g_sonic_distance = -1;


void sonic_setup()
{
	pinMode(PIN_SONIC_TRIGGER, OUTPUT);
	pinMode(PIN_SONIC_ECHO, INPUT);
}


int sonic_measure_distance_raw()
{
    /**
        The following PIN_SONIC_TRIGGER/PIN_SONIC_ECHO cycle is used to
        determine the distance of the nearest object by bouncing soundwaves
        off of it.
    */
    digitalWrite(PIN_SONIC_TRIGGER, LOW); 
    delayMicroseconds(2); 

    digitalWrite(PIN_SONIC_TRIGGER, HIGH);
    delayMicroseconds(10); 
    digitalWrite(PIN_SONIC_TRIGGER, LOW);

    unsigned long duration = pulseIn(PIN_SONIC_ECHO, HIGH);
    if(duration == 0)
        return -1;

    // Calculate the distance (in cm) based on the speed of sound.
    unsigned long distance = duration/58.2;

//    Serial.println(distance);

    if(distance > ALARM_MAX) {
        return -1;
    }
    return distance;
}


int sonic_measure_distance()
{
	bool do_stop = false;

	int sum = 0;
	int raw;
	int nr_values = 0;
	for (int i=0; i<10; ++i)
	{
		raw = sonic_measure_distance_raw();
		if (raw != -1)
		{
			++nr_values;
			sum += raw;
			if (raw > 0 && raw < 30)
			{
				Serial.print(".....");
				Serial.println(raw);
			}
		}
	}
	Serial.print("______________");
	Serial.println(nr_values);

	if (do_stop)
		for(;;);

	return sum / nr_values;
}


void sonic_run()
{
	unsigned long m = millis();
	if((unsigned long)(m - sonic_last_access) < SONIC_DELAY) {
		return;
	}
	sonic_last_access = m;

	int tmp = sonic_measure_distance();
	if (tmp != 0 && tmp != ALARM_MAX)
	{
		g_sonic_distance = tmp;
	}

#if 1
	Serial.print(F("[sonic] "));
	Serial.print(g_sonic_distance);
	Serial.println(F("cm"));
#endif
}

