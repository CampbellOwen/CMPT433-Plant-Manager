#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <stdio.h>

typedef enum {
    JOYSTICK_UP = 0,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_RIGHT
} JoystickState;

void Joystick_Init(void);
int Joystick_DirectionPressed( JoystickState state );

#endif