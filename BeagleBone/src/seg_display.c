#include <include/seg_display.h>
#include <include/device_manager.h>

#include <pthread.h>
#include <stdlib.h>
#include <time.h>

#define SEG_LEFT_PIN 61
#define SEG_RIGHT_PIN 44
#define SEG_BUS 1
#define SEG_ADDR 0x20
#define SEG_UPPER 0x15
#define SEG_LOWER 0x14

static pthread_t tid;
static pthread_mutex_t lock;
static seg_char_t left_char;
static seg_char_t right_char;
static int should_run = 0;
static struct timespec delay_time = { 0, 5000000 };
// static struct timespec delay_time = { 1, 0 };
static int cap_devices = 10;
static int num_devices = 0;
static int devices_index = 0;
static moisture_row_t* moisture_devices;

// Access using enum in header
static seg_char_t chars[ 10 ] = {
    { 1, 0x86, 0xA1 }, // 0
    { 1, 0x12, 0x80 }, // 1
    { 1, 0x0E, 0x31 }, // 2
    { 1, 0x0E, 0xB0 }, // 3
    { 1, 0x8A, 0x90 }, // 4
    { 1, 0x8C, 0xB0 }, // 5
    { 1, 0x8C, 0xB1 }, // 6
    { 1, 0x06, 0x80 }, // 7
    { 1, 0x8E, 0xB1 }, // 8
    { 1, 0x8E, 0xB0 }  // 9
};

static void* SegDisplay_Thread( void* args )
{
    seg_display_t* display = ( seg_display_t* )args;
    seg_char_t left_bak;
    seg_char_t right_bak;

    while( should_run ) {
        pthread_mutex_lock( &lock );
        {
            left_bak = left_char;
            right_bak = right_char;
        }
        pthread_mutex_unlock( &lock );

        GPIO_WritePin( &( display->left_pin ), 0 );
        GPIO_WritePin( &( display->right_pin ), 0 );

        I2C_WriteRegister( &( display->i2c ), SEG_UPPER, left_bak.top );
        I2C_WriteRegister( &( display->i2c ), SEG_LOWER, left_bak.bottom );
        GPIO_WritePin( &( display->left_pin ), left_bak.on );

        nanosleep( &delay_time, ( struct timespec* ) NULL );
        GPIO_WritePin( &( display->left_pin ), 0 );

        I2C_WriteRegister( &( display->i2c ), SEG_UPPER, right_bak.top );
        I2C_WriteRegister( &( display->i2c ), SEG_LOWER, right_bak.bottom );
        GPIO_WritePin( &( display->right_pin ), right_bak.on );

        nanosleep( &delay_time, ( struct timespec* ) NULL );
        GPIO_WritePin( &( display->right_pin ), 0 );
    }

    return NULL;
}

int SegDisplay_Init( seg_display_t* display )
{
    display->left_pin.pinNumber = SEG_LEFT_PIN;
    display->right_pin.pinNumber = SEG_RIGHT_PIN;

    GPIO_InitPin( &( display->left_pin ) );
    GPIO_InitPin( &( display->right_pin ) );

    GPIO_WritePin( &( display->left_pin ), 0 );
    GPIO_WritePin( &( display->right_pin ), 0 );

    display->i2c.bus = SEG_BUS;

    I2C_InitBus( &( display->i2c ), SEG_ADDR );
    I2C_WriteRegister( &( display->i2c ), 0x00, 0x00 );
    I2C_WriteRegister( &( display->i2c ), 0x01, 0x00 );

    left_char.on = 0;
    right_char.on = 0;

    pthread_mutex_init( &lock, NULL );

    should_run = 1;
    moisture_devices = malloc(cap_devices * sizeof(int));
    pthread_create( &tid, NULL, &SegDisplay_Thread, ( void* )( display ) );

    return 1;
}

static enum seg_chars SegDisplay_CharToEnum( char c )
{
    switch( c ) {
        case '0':
            return ZERO;
            break;
        case '1':
            return ONE;
            break;
        case '2':
            return TWO;
            break;
        case '3':
            return THREE;
            break;
        case '4':
            return FOUR;
            break;
        case '5':
            return FIVE;
            break;
        case '6':
            return SIX;
            break;
        case '7':
            return SEVEN;
            break;
        case '8':
            return EIGHT;
            break;
        case '9':
            return NINE;
            break;
        default:
            return ZERO;
    }
}

void SegDisplay_SetInt( int val )
{
    if( val > 99 ) val = 99;
    if( val < 0 ) val = 0;
    int left = val / 10;
    int right = val % 10;
    char vals[2] = { left + '0', right + '0' };

    SegDisplay_SetChars( vals );
}

void SegDisplay_SetChars( char vals[2] )
{
    pthread_mutex_lock( &lock );
    {
        if( vals[ 0 ] == '\0' ) {
            left_char.on = 0;
            return;
        }

        if( vals[ 1 ] == '\0' ) {
            right_char.on = 0;
            return;
        }

        left_char = chars[ SegDisplay_CharToEnum( vals[ 0 ] ) ];
        right_char = chars[ SegDisplay_CharToEnum( vals[ 1 ] ) ];
    }
    pthread_mutex_unlock( &lock );
}

void SegDisplay_SetDevice(int index) {
  if (index >= num_devices) {
    return;
  }
  SegDisplay_SetInt(index);
}

void SegDisplay_NextDevice() {
  devices_index++;

  if (devices_index >= num_devices) {
    devices_index = 0;
  }

  SegDisplay_SetDevice(devices_index);
}

void SegDisplay_Update(int index, moisture_row_t* moisture) {
  if (index > cap_devices) {
    cap_devices = 2 * cap_devices;
    moisture_devices = realloc(moisture_devices, cap_devices*sizeof(moisture_row_t));
  }

  moisture_devices[index] = *moisture;
}

void SegDisplay_Cleanup( seg_display_t* display )
{
    should_run = 0;
    pthread_join( tid, NULL );

    I2C_Cleanup( &( display->i2c) );
    pthread_mutex_destroy( &lock );
}
