#include <include/A2D.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define UNIVERSAL_CAPE_SLOTS_FILE_PATH "/sys/devices/platform/bone_capemgr/slots"
#define IN_VOLTAGE_FILE_PATH_FORMAT "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw"
#define A2D_BUFFER_SIZE 1024

int A2D_Activate( void )
{
    FILE* capeSlotsFile = fopen( UNIVERSAL_CAPE_SLOTS_FILE_PATH, "w" );
    if( capeSlotsFile == NULL ) {
        fprintf( stderr, "Error: Opening slots file failed\n" );
        return 0;
    }

    if( fprintf( capeSlotsFile, "BB-ADC" ) == 0 ) {
        fclose( capeSlotsFile );
        fprintf( stderr, "Error: Writing to slots file failed\n" );
        return 0;
    }

    fclose( capeSlotsFile );

    // Wait 1 second to give the A2D time to turn on
    struct timespec delay_time = { 1, 0 };
    nanosleep( &delay_time, ( struct timespec* ) NULL );

    return 1;
}

int A2D_Init( A2D_t* pin )
{
    pin->A2D_bits = 12;
    pin->v_ref = 1.8f;

    return A2D_Activate();
}

int A2D_Read( A2D_t* pin )
{
    char buffer[ A2D_BUFFER_SIZE ];
    sprintf( buffer, IN_VOLTAGE_FILE_PATH_FORMAT, pin->pin );
    buffer[ A2D_BUFFER_SIZE - 1 ] = '\0';

    FILE* voltage_in_file = fopen( buffer, "r" );
    if( voltage_in_file == NULL ) {
        fprintf( stderr, "Error: Opening in_voltage_raw file failed\n" );
        return 0;
    }

    fgets( buffer, A2D_BUFFER_SIZE, voltage_in_file );
    buffer[ A2D_BUFFER_SIZE - 1 ] = '\0';

    fclose( voltage_in_file );

    return atoi( buffer );
}

double A2D_ReadingToVoltage( A2D_t* pin, int reading )
{
    double max_val = pow( 2.0, ( double )( pin->A2D_bits ) ) - 1;
    double percentage = ( reading * 1.0 ) / max_val;

    return percentage * pin->v_ref;
}
