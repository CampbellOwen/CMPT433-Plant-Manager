#ifndef _SEG_DISPLAY_H_
#define _SEG_DISPLAY_H_

#include <include/device_manager.h>

#include "GPIO.h"
#include "i2c.h"

typedef struct {
    int on;
    unsigned char top;
    unsigned char bottom;
} seg_char_t;

enum seg_chars {
    ZERO = 0,
    ONE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE
};

typedef struct {
    GPIO_Pin_t left_pin;
    GPIO_Pin_t right_pin;
    i2c_t i2c;
} seg_display_t;

int SegDisplay_Init( seg_display_t* display );

void SegDisplay_SetInt( int val );
void SegDisplay_SetChars( char vals[2] );
void SegDisplay_SetDevice(int index);
void SegDisplay_NextDevice();
void SegDisplay_Update(int index, moisture_row_t* moisture);

void SegDisplay_Cleanup( seg_display_t* display );

#endif
