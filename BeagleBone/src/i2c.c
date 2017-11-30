#include <include/i2c.h>
#include <stdlib.h>
#include <stdio.h>
#include <stropts.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>

#define I2C_BUS_PATH "/dev/i2c-%d"
#define I2C_BUFFER_LENGTH 1024
#define I2C_SLOTS_PATH "/sys/devices/platform/bone_capemgr/slots"

static int I2C_ActivateBus( int bus )
{
    FILE* slotsFile = fopen( I2C_SLOTS_PATH, "w" );
    if( slotsFile == NULL ) {
        fprintf( stderr, "Error opening slots file\n" );
        return 0;
    }

    fprintf( slotsFile, "BB-I2C%d", bus );
    fclose( slotsFile );

    return 1;
}

int I2C_InitBus( i2c_t* i2c, int address )
{
    char buffer[ I2C_BUFFER_LENGTH ];
    sprintf( buffer, I2C_BUS_PATH, i2c->bus );
    buffer[ I2C_BUFFER_LENGTH - 1 ] = '\0';

    if( !I2C_ActivateBus( i2c->bus ) ) {
        return 0;
    }

    i2c->fileDesc = open( buffer, O_RDWR );
    if( i2c->fileDesc < 0 ) {
        fprintf( stderr, "Error opening i2c file desc\n" );
        return 0;
    }

    int result = ioctl( i2c->fileDesc, I2C_SLAVE, address );
    if( result < 0 ) {
        fprintf( stderr, "Error: Unable to set I2C device to slave address\n" );
        return 0;
    }
    return 1;
}

void I2C_WriteRegister( i2c_t* i2c, unsigned char regAddr, unsigned char value )
{
    unsigned char buffer[2];
    buffer[0] = regAddr;
    buffer[1] = value;

    int res = write( i2c->fileDesc, buffer, 2 );
    if ( res != 2 ) {
        fprintf( stderr, "Error: Unable to write to I2C register\n" );
    }    
}

unsigned char I2C_ReadRegister( i2c_t* i2c, unsigned char regAddr )
{
    int res = write( i2c->fileDesc, &regAddr, sizeof( regAddr ) );
    if ( res != sizeof( regAddr ) ) {
        fprintf( stderr, "Error: Unable to read from I2C register - write step\n" );
        return 0;
    }

    char value = 0;
    res = read( i2c->fileDesc, &value, sizeof( value ) );
    if( res != sizeof( value ) ) {
        fprintf( stderr, "Error: Unable to read from I2C register - read step\n" );
        return 0;
    }

    return value;
}

void I2C_Cleanup( i2c_t* i2c )
{
    close( i2c->fileDesc );
}
