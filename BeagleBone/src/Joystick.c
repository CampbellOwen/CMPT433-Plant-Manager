#include <include/Joystick.h>

#include <include/GPIO.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_PORTS 4
#define EXPORT_FILE_PATH "/sys/class/gpio/export"
#define VALUE_FILE_PATH "/sys/class/gpio/gpio%d/value"
#define DIRECTION_FILE_PATH "/sys/class/gpio%d/direction"

static GPIO_Pin_t ports[] = {
    { 26, OUT, CLOSED },
    { 46, OUT, CLOSED },
    { 65, OUT, CLOSED },
    { 47, OUT, CLOSED },
};

void Joystick_Init( void )
{
        for( int i = 0; i < NUM_PORTS; i++ ) {
            GPIO_InitPin( &ports[ i ] );
            GPIO_SetDirection( &ports[ i ], IN );
        }
}

int Joystick_DirectionPressed( JoystickState state )
{
    // int port = ports[state].gpio;
    // printf("port: %d\n", port);

    // char buffer[ 256 ];
    // fgets( buffer, 256, ports[ state ].valueFile );
    // buffer[ 255 ] = '\0';

    // int val = atoi( buffer );
    
    // return !val;

    return !GPIO_ReadPin( &ports[ state ] );
}
