#ifndef _I2C_H_
#define _I2C_H_

typedef struct {
    int bus;
    int fileDesc;
} i2c_t;

int I2C_InitBus( i2c_t* i2c, int address );
void I2C_WriteRegister( i2c_t* i2c, unsigned char regAddr, unsigned char value );
unsigned char I2C_ReadRegister( i2c_t* i2c, unsigned char regAddr );
void I2C_Cleanup( i2c_t* i2c );

#endif