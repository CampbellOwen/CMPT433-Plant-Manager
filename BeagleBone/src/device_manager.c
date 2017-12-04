#include <include/device_manager.h>
#include <include/device_array.h>
#include <include/udp_server.h>
#include <include/pid.h>
#include <include/define.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <include/sqlite3.h>

#define DEVICE_MANAGER_DEFAULT_SIZE 64
#define SQL_STATEMENT_BUFFER_SIZE 1024

#define STATUS_MOISTURE 'm'
#define STATUS_HUMIDITY 'u'
#define STATUS_TEMPERATURE 't'

#define DB_NAME "/root/plants.db"
#define INSERT_MOISTURE "INSERT INTO moisture (id, time, value) VALUES ( %u, %llu, %u );"
#define INSERT_HUMIDITY "INSERT INTO humidity (id, time, value) VALUES ( %u, %llu, %u );"
#define INSERT_TEMPERATURE "INSERT INTO temperature (id, time, value) VALUES ( %u, %llu, %u );"
#define SELECT_LAST_MOISTURE "SELECT * FROM moisture WHERE TIME=(SELECT MAX(TIME) FROM moisture WHERE id=%u);"

#define UPDATE_STATUS "UPDATE devices SET status='%s' WHERE id=%u;"
#define INSERT_DEVICE "INSERT OR IGNORE INTO devices(id, status) VALUES(%d, '%s');"

static pthread_t poll_thread;

static device_array_t* device_arr;
static pthread_mutex_t lock;
static struct timespec heart_beat_time;
static struct timespec poll_time;
static int should_watch;
static sqlite3* db;

static void* poll_devices( void* args )
{
     while( should_watch ) {

         int num_devices = 0;
         device_t* devices = DeviceArray_GetAlive( device_arr, &num_devices );

         for( int i = 0; i < num_devices; i++ ) {
             printf( DEVICE_STATUS "Requesting sensor data from device %u\n", devices[ i ].id );
             UDP_Server_RequestSensor( &devices[ i ], STATUS_MOISTURE );
             UDP_Server_RequestSensor( &devices[ i ], STATUS_HUMIDITY );
             UDP_Server_RequestSensor( &devices[ i ], STATUS_TEMPERATURE );
             PID_Update(&devices[i]);
         }

         free( devices );

         nanosleep( &poll_time, NULL );
     }

    return NULL;
}

static void save_device_status( device_t* device, char* status )
{
    char sql_statement[ SQL_STATEMENT_BUFFER_SIZE ];

    sprintf( sql_statement, UPDATE_STATUS, status, device->id );

    char* err_msg = NULL;

    int ret = sqlite3_exec( db, sql_statement, NULL, NULL, &err_msg );
    if( ret != SQLITE_OK ) {
          fprintf( stderr, ERROR "Error updating device status in SQL: %s\n", err_msg );
          sqlite3_free( err_msg );
    }
    else {
        printf( INFO "Values succesfully updated in db\n" );
    }
}

static void* watch_device( void* args )
{
	device_t* device = (device_t*)args;

	printf( DEVICE_STATUS "Starting watch for %d\n", device->id );

	while( should_watch && device->state != TIMEOUT ) {
		nanosleep( &heart_beat_time, NULL );
		uint64_t curr_time = time( NULL );
		pthread_mutex_lock( &lock );
		{
			printf( DEVICE_STATUS "Checking on device %d at time %llu\n", device->id, curr_time );
			if( ( curr_time - device->last_seen ) > ( heart_beat_time.tv_sec ) )
			{
				printf( DEVICE_STATUS "Device %d timeout\n", device->id );
				device->state = TIMEOUT;
                    save_device_status( device, "Offline" );
			}
               else {
                   printf( DEVICE_STATUS "Device %d okay\n", device->id );
                    save_device_status( device, "Online" );
               }
		}
		pthread_mutex_unlock( &lock );
	}

	printf( DEVICE_STATUS "Ending watch for device %d\n", device->id );

	return NULL;
}

static void DeviceManager_InitPID( void ) {
  int num_devices = 0;
  device_t* devices = DeviceArray_GetAlive( device_arr, &num_devices );

  for( int i = 0; i < num_devices; i++ ) {
    // Save initial values into database
    printf("Device: %d's initial values\n", i);
    PID_SavePIDdata(&devices[i], 0, 0, 0);
  }
}

int DeviceManager_Init( void )
{
  int ret = sqlite3_open( DB_NAME, &db );

  if( ret ) {
    fprintf( stderr, ERROR "Can't open database: %s\n", sqlite3_errmsg( db ) );
    return 0;
  }

  should_watch = 1;
  pthread_mutex_init( &lock, NULL );
  device_arr = DeviceArray_Init( DEVICE_MANAGER_DEFAULT_SIZE );
  srand( time( NULL ) );

  heart_beat_time.tv_sec = 5;
  heart_beat_time.tv_nsec = 0;

  poll_time.tv_sec = 5;
	poll_time.tv_nsec = 0;

  DeviceManager_InitPID();
  pthread_create( &poll_thread, NULL, poll_devices, NULL );

  return 1;
}

device_t* DeviceManager_Register( struct sockaddr_in* addr )
{
	uint32_t id = rand();

	device_t* new_device = malloc( sizeof( device_t ) );
	new_device->id = id;
	new_device->last_seen = (uint64_t)time( NULL );
	new_device->address = addr;
	new_device->state = ALIVE;

	printf( DEVICE_STATUS "Registering device to id %d\n", id );

    char sql_statement[ SQL_STATEMENT_BUFFER_SIZE ];

    sprintf( sql_statement, INSERT_DEVICE, new_device->id, "Online" );

    char* err_msg = NULL;

    int ret = sqlite3_exec( db, sql_statement, NULL, NULL, &err_msg );
    if( ret != SQLITE_OK ) {
          fprintf( stderr, ERROR "Error inserting device in SQL: %s\n", err_msg );
          sqlite3_free( err_msg );
    }
    else {
        printf( INFO "Device succesfully inserted in db\n" );
    }

	pthread_create( &new_device->watch_thread, NULL, &watch_device, (void*)new_device );


	DeviceArray_Put( device_arr, new_device );

	return new_device;
}

device_t* DeviceManager_Reregister( struct sockaddr_in* addr, uint32_t id )
{
	device_t* new_device = malloc( sizeof( device_t ) );
	new_device->id = id;
	new_device->last_seen = (uint64_t)time( NULL );
	new_device->address = addr;
	new_device->state = ALIVE;

	printf( DEVICE_STATUS "Re-registering old device - id: %d\n", id );

     printf( DEBUG "Port: %d\n", ntohs( new_device->address->sin_port ) );

    char sql_statement[ SQL_STATEMENT_BUFFER_SIZE ];
    sprintf( sql_statement, INSERT_DEVICE, new_device->id, "Online" );

    printf( INFO "SQL STATEMENT: %s\n", sql_statement );

    char* err_msg = NULL;

    int ret = sqlite3_exec( db, sql_statement, NULL, NULL, &err_msg );
    if( ret != SQLITE_OK ) {
          fprintf( stderr, ERROR "Error inserting device in SQL: %s\n", err_msg );
          sqlite3_free( err_msg );
    }
    else {
        printf( INFO "Device succesfully inserted in db\n" );
    }

	pthread_create( &new_device->watch_thread, NULL, &watch_device, (void*)new_device );

	DeviceArray_Put( device_arr, new_device );

//     int len;
//     moisture_row_t* vals = DeviceManager_GetMoistureAfterTime( new_device, time( NULL ) - 1000000, &len );
//
//     printf( INFO "Received rows from db\n" );
//     for( int i = 0; i < len; i++ ) {
//         printf( INFO "Moisture value: %d\n", vals[ i ].value );
//     }
//
//     free( vals );
//
     //DeviceManager_ActivatePump( new_device, 1000 );

	return new_device;
}

device_t* DeviceManager_GetDevice( uint32_t id )
{
	return DeviceArray_GetId( device_arr, id );
}

int DeviceManager_ReportHeartbeat( struct sockaddr_in* clientAddr, uint32_t id )
{
	pthread_mutex_lock( &lock );
	{
		device_t* device = DeviceArray_GetId( device_arr, id );
		if( device == NULL ) {
			device = DeviceManager_Reregister( clientAddr, id );
		}

		printf( DEVICE_STATUS "Heartbeat from %d", id );

		device->last_seen = (uint64_t)time( NULL );
		if( device->state == TIMEOUT ) {
			printf( " - previously timed out\n" );
			pthread_join( device->watch_thread, NULL );
			pthread_create( &device->watch_thread, NULL, &watch_device, (void*)device );
		}
		else {
			printf( "\n" );
		}

		device->state = ALIVE;
	}
	pthread_mutex_unlock( &lock );

	return 1;
}

void DeviceManager_Shutdown()
{
	should_watch = 0;
	int num_living = 0;
	device_t* living_devices = DeviceArray_GetAlive( device_arr, &num_living );
	for( int i = 0; i < num_living; i++ ) {
		pthread_join( living_devices[ i ].watch_thread, NULL );
	}
     pthread_join( poll_thread, NULL );
	DeviceArray_Destroy( device_arr );
	pthread_mutex_destroy( &lock );

     sqlite3_close( db );
}

void DeviceManager_SaveSensorData( device_t* device, uint32_t value, char sensorType )
{
  if (value >= 1000000) {
    return;
  }

  long long curr_time = ( long long )time( NULL );
  uint32_t id = device->id;
  char sql_statement[ SQL_STATEMENT_BUFFER_SIZE ];

  switch(sensorType) {
   case STATUS_MOISTURE:
      printf( INFO "Saving moisture data from id: %u, value: %u\n", id, value );
      sprintf( sql_statement, INSERT_MOISTURE, id, curr_time, value );
      break;
   case STATUS_HUMIDITY:
      printf( INFO "Saving humidity data from id: %u, value: %u\n", id, value );
      sprintf( sql_statement, INSERT_HUMIDITY, id, curr_time, value );
      break;
   case STATUS_TEMPERATURE:
      printf( INFO "Saving temperature data from id: %u, value: %u\n", id, value );
      sprintf( sql_statement, INSERT_TEMPERATURE, id, curr_time, value );
      break;
  }

  char* err_msg = NULL;

  int ret = sqlite3_exec( db, sql_statement, NULL, NULL, &err_msg );
  if( ret != SQLITE_OK ) {
      fprintf( stderr, ERROR "Error writing to SQL: %s\n", err_msg );
      sqlite3_free( err_msg );
  }
  else {
    printf( INFO "Values succesfully stored in db\n" );
  }
}

static int sql_read_moisture_callback( void* ret_args, int num_rows, char** rows, char** columns )
{
     moisture_callback_args_t* args = (moisture_callback_args_t*) ret_args;

     moisture_row_t* row = malloc( sizeof( moisture_row_t ) );

     row->id = atoi( rows[ 0 ] );
     row->timestamp = atol( rows[ 1 ] );
     row->value = atoi( rows[ 2 ] );

     //printf( INFO "(id, time, value) : (%d, %lld, %d)\n", row->id, row->timestamp, row->value );

     if( args->length >= args->max ) {
          printf( ERROR "Too many rows returned\n" );
          free( row );
          return 1;
     }
     else {
          args->rows[ args->length++ ] = *row;
     }
     return 0;
}

moisture_row_t* DeviceManager_GetLastMoisture(device_t* device)
{
  char sql[ SQL_STATEMENT_BUFFER_SIZE ];
  sprintf( sql, SELECT_LAST_MOISTURE, device->id );
  char* err_msg = NULL;

  moisture_callback_args_t args;
  args.max = 1;
  args.length = 0;
  args.rows = malloc( args.max * sizeof( moisture_row_t ) );

  int ret = sqlite3_exec( db, sql, sql_read_moisture_callback, &args, &err_msg );
  if( ret != SQLITE_OK )
  {
        fprintf( stderr, ERROR "Error reading from moisture table: %s\n", err_msg );
        sqlite3_free( err_msg );
        return NULL;
  }
  else {
      printf( INFO "Last moisture was succesfully read from db\n" );
  }

  return args.rows;
}

moisture_row_t* DeviceManager_GetMoistureAfterTime( device_t* device, long long timestamp, int* arr_len )
{
    char sql[ SQL_STATEMENT_BUFFER_SIZE ];

    sprintf( sql, "SELECT * FROM moisture WHERE id = %u AND time > %lld;", device->id, timestamp );

    char* err_msg = NULL;


    moisture_callback_args_t args;
    args.max = 1024;
    args.length = 0;
    args.rows = malloc( args.max * sizeof( moisture_row_t ) );

    int ret = sqlite3_exec( db, sql, sql_read_moisture_callback, &args, &err_msg );
    if( ret != SQLITE_OK ) {
          fprintf( stderr, ERROR "Error reading from SQL: %s\n", err_msg );
          sqlite3_free( err_msg );
          return NULL;
    }
    else {
        printf( INFO "%d values succesfully read from db\n", args.length );
    }

    *arr_len = args.length;

    return args.rows;
}

void DeviceManager_ActivatePump( device_t* device, uint32_t duration )
{
    if (duration > 1000) {
      duration = 1000;
    }

    if (duration > 0) {
      uint32_t id = device->id;
      printf( INFO "Activating %u's pump for %u milliseconds\n", id, duration );

      UDP_Server_RequestPump( device->address, sizeof( *device->address ), duration );
    }
}

device_t* DeviceManager_GetAll( int* len )
{
     return DeviceArray_GetAll( device_arr, len );
}
