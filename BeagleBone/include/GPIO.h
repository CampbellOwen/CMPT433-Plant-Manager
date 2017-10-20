#ifndef _GPIO_H_
#define _GPIO_H_

#include <stdio.h>

typedef enum {
    IN,
    OUT
} GPIO_Direction_t;

typedef enum {
    OPEN,
    CLOSED
} GPIO_PinStatus_t;

typedef struct {
    int pinNumber;
    GPIO_Direction_t direction;
    GPIO_PinStatus_t status;
} GPIO_Pin_t;

int GPIO_OpenPin( GPIO_Pin_t* pin );
int GPIO_ClosePin( GPIO_Pin_t* pin );

int GPIO_ReadPin( GPIO_Pin_t* pin );
int GPIO_WritePin( GPIO_Pin_t* pin, int value );

int GPIO_GetDirection( GPIO_Pin_t* pin );
int GPIO_SetDirection( GPIO_Pin_t* pin, GPIO_Direction_t direction );

int GPIO_InitPin( GPIO_Pin_t* pin );

#endif