#include "common.h"


#define ALARM_MAX 1000


void sonic_setup()
{
    pinMode(PIN_SONIC_TRIGGER, OUTPUT);
    pinMode(PIN_SONIC_ECHO, INPUT);
}


int sonic_measure_distance()
{
    /**
        The following PIN_SONIC_TRIGGER/PIN_SONIC_ECHO cycle is used to
        determine the distance of the nearest object by bouncing soundwaves
        off of it.
    */
    digitalWrite(PIN_SONIC_TRIGGER, LOW); 
    delayMicroseconds(2); 

    digitalWrite(PIN_SONIC_TRIGGER, HIGH);
    delayMicroseconds(5); 

    digitalWrite(PIN_SONIC_TRIGGER, LOW);
    unsigned long duration = pulseIn(PIN_SONIC_ECHO, HIGH);
    if(duration == 0)
        return ALARM_MAX;

    // Calculate the distance (in cm) based on the speed of sound.
    unsigned long distance = duration/58.2;

    //Serial.println(distance);

    if(distance > ALARM_MAX) {
        return ALARM_MAX;
    }
    return distance;
}
