#ifndef _LED_H_
#define _LED_H_

#include <stdio.h>

typedef enum {
    ON,
    OFF
} LED_Status_t;

typedef struct {
    int number;
    LED_Status_t status;
} LED_t;

int LED_Init( LED_t* led );
int LED_Flash( LED_t* led, int times, int time_on, int time_off );
int LED_TurnOn( LED_t* led );
int LED_TurnOff( LED_t* led );
int LED_WriteLED( LED_t* led, int value );

#endif