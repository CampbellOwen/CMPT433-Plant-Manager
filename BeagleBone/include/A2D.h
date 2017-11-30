#ifndef _A2D_H_
#define _A2D_H_

typedef struct {
    int pin;
    int A2D_bits;
    double v_ref; 
} A2D_t;

//Activate A2D in the hardware
int A2D_Activate( void );

//Initialize an A2D_t struct, this should be the user's starting point
int A2D_Init( A2D_t* pin );

int A2D_Read( A2D_t* pin );

double A2D_ReadingToVoltage( A2D_t* pin, int reading );

#endif