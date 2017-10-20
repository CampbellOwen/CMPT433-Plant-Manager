#include <include/LED.h>
#include <stdlib.h>
#include <time.h>

#define TRIGGER_FILE_PATH "/sys/class/leds/beaglebone:green:usr%d/trigger"
#define BRIGHTNESS_FILE_PATH "/sys/class/leds/beaglebone:green:usr%d/brightness"
#define LED_BUFFER_LENGTH 1024

int LED_Init( LED_t* led )
{
    char buffer[ LED_BUFFER_LENGTH ];
    sprintf( buffer, TRIGGER_FILE_PATH, led->number );
    buffer[ LED_BUFFER_LENGTH - 1 ] = '\0';
    
    FILE* triggerFile = fopen( buffer, "w" );
    if( triggerFile == NULL ) {
        fprintf( stderr, "Error: Unable to open trigger file for led %d\n", led->number );
        return 0;
    }
    fprintf( triggerFile, "none" );

    fclose( triggerFile );

    if( !LED_TurnOff( led ) ) {
        return 0;
    }

    return 1;
}

int LED_Flash( LED_t* led, int times, int time_on, int time_off )
{
    struct timespec time_on_struct = {
        0,
        time_on * 1000000
    };

    struct timespec time_off_struct = {
        0,
        time_off * 1000000
    };

    for( int i = 0; i < times; i++ ) {
        if( !LED_TurnOn( led ) ) {
            return 0;
        }
        nanosleep( &time_on_struct, ( struct timespec* ) NULL );

        if( !LED_TurnOff( led ) ) {
            return 0;
        }
        nanosleep( &time_off_struct, ( struct timespec* ) NULL );
    }

    return 1;
}

int LED_TurnOn( LED_t* led )
{
    return LED_WriteLED( led, 1 );
}

int LED_TurnOff( LED_t* led )
{
    return LED_WriteLED( led, 0 );
}

int LED_WriteLED( LED_t* led, int value )
{
    char buffer[ LED_BUFFER_LENGTH ];
    sprintf( buffer, BRIGHTNESS_FILE_PATH, led->number );
    buffer[ LED_BUFFER_LENGTH - 1 ] = '\0';
    
    FILE* brightnessFile = fopen( buffer, "w" );
    if( brightnessFile == NULL ) {
        fprintf( stderr, "Error: Unable to open brightness file for led %d\n", led->number );
        return 0;
    }
    
    fprintf( brightnessFile, "%d", value );
    fclose( brightnessFile );

    return 1;
}
