#include <include/GPIO.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPORT_FILE_PATH "/sys/class/gpio/export"
#define UNEXPORT_FILE_PATH "/sys/class/gpio/unexport"
#define VALUE_FILE_PATH "/sys/class/gpio/gpio%d/value"
#define DIRECTION_FILE_PATH "/sys/class/gpio/gpio%d/direction"

#define GPIO_BUFFER_SIZE 1024

int GPIO_OpenPin( GPIO_Pin_t* pin )
{
    FILE* exportFile = fopen( EXPORT_FILE_PATH, "w" );
    if( exportFile == NULL ) {
        fprintf( stderr, "Error: Opening export file failed\n" );
        return 0;
    }

    fprintf( exportFile, "%d", pin->pinNumber );
    fclose( exportFile );
    
    pin->status = OPEN;

    return 1;
}

int GPIO_ClosePin( GPIO_Pin_t* pin )
{
    FILE* unexportFile = fopen( UNEXPORT_FILE_PATH, "w" );
    if( unexportFile == NULL ) {
        fprintf( stderr, "Error: Opening unexport file failed\n" );
        return 0;
    }

    fprintf( unexportFile, "%d", pin->pinNumber );
    fclose( unexportFile );
    
    pin->status = CLOSED;

    return 1;
}

int GPIO_ReadPin( GPIO_Pin_t* pin )
{
    char buffer[ GPIO_BUFFER_SIZE ];
    sprintf( buffer, VALUE_FILE_PATH, pin->pinNumber );
    buffer[ GPIO_BUFFER_SIZE - 1 ] = '\0';
    
    if( pin->status == CLOSED ) {
        GPIO_OpenPin( pin );
    }

    if( pin->direction == OUT ) {
        GPIO_SetDirection( pin, IN );
    }
    
    FILE* valueFile = fopen( buffer, "r" );
    if( valueFile == NULL ) {
        fprintf( stderr, "Error: Unable to open value file for pin %d\n", pin->pinNumber );
        return 0;
    }

    fgets( buffer, GPIO_BUFFER_SIZE, ( valueFile ) );
    buffer[ GPIO_BUFFER_SIZE - 1 ] = '\0';

    return atoi( buffer );
}

int GPIO_WritePin( GPIO_Pin_t* pin, int value )
{
    char buffer[ GPIO_BUFFER_SIZE ];
    sprintf( buffer, VALUE_FILE_PATH, pin->pinNumber );
    buffer[ GPIO_BUFFER_SIZE - 1 ] = '\0';

    if( pin->status == CLOSED ) {
        GPIO_OpenPin( pin );
    }

    if( pin->direction == IN ) {
        GPIO_SetDirection( pin, OUT );
    }

    FILE* valueFile = fopen( buffer, "w" );
    if( valueFile == NULL ) {
        fprintf( stderr, "Error: Unable to open value file for pin %d\n", pin->pinNumber );
        return 0;
    }

    fprintf( valueFile, "%d", value );
    fclose( valueFile );
    printf(" Wrote value\n");
    return 1;
}

int GPIO_GetDirection( GPIO_Pin_t* pin )
{
    char buffer[ GPIO_BUFFER_SIZE ];
    sprintf( buffer, DIRECTION_FILE_PATH, pin->pinNumber );
    buffer[ GPIO_BUFFER_SIZE - 1 ] = '\0';

    FILE* directionFile = fopen( buffer, "r" );
    if( directionFile == NULL ) {
        fprintf( stderr, "Error: Unable to open direction file { %s } for pin %d\n", buffer, pin->pinNumber );
        return 0;
    }

    fgets( buffer, GPIO_BUFFER_SIZE, directionFile );
    buffer[ GPIO_BUFFER_SIZE - 1 ] = '\0';

    if( !strcmp( "out", buffer ) ) {
        pin->direction = OUT;
    }
    else {
        pin->direction = IN;
    }

    fclose( directionFile );

    return 1;
}

int GPIO_SetDirection( GPIO_Pin_t* pin, GPIO_Direction_t direction )
{
    char buffer[ GPIO_BUFFER_SIZE ];
    sprintf( buffer, DIRECTION_FILE_PATH, pin->pinNumber );
    buffer[ GPIO_BUFFER_SIZE - 1 ] = '\0';

    FILE* directionFile = fopen( buffer, "w" );
    if( directionFile == NULL ) {
        fprintf( stderr, "Error: Unable to open direction file { %s } for pin %d\n", buffer, pin->pinNumber );
        return 0;
    }

    switch( direction ) {
        case IN:
            fprintf( directionFile, "in" );
            pin->direction = IN;
            break;

        case OUT:
            fprintf( directionFile, "out" ); 
            pin->direction = OUT;
            break;
    }
    fclose( directionFile );

    return 1;
}

int GPIO_InitPin( GPIO_Pin_t* pin )
{
    if( !GPIO_OpenPin( pin ) ) {
        return 0;
    }

    if( !GPIO_GetDirection( pin ) ) {
        return 0;
    }

    return 1;
}
