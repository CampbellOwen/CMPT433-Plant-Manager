#include <include/device_manager.h>
#include <include/device_array.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define DEVICE_MANAGER_DEFAULT_SIZE 64

static device_array_t* device_arr;
static pthread_mutex_t lock;
static struct timespec heart_beat_time;
static int should_watch;

static void* watch_device( void* args )
{
	device_t* device = (device_t*)args;

	printf( "Starting watch for device %d\n", device->id );

	while( should_watch && device->state != TIMEOUT ) {
		nanosleep( &heart_beat_time, NULL );
		uint64_t curr_time = time( NULL );
		pthread_mutex_lock( &lock );
		{
			printf( "Current time: %llu; device %d last seen: %llu\n", curr_time, device->id, device->last_seen );
			if( ( curr_time - device->last_seen ) > ( heart_beat_time.tv_sec ) )
			{
				printf( "Device %d TIMEOUT\n", device->id );
				device->state = TIMEOUT;
			}
		}
		pthread_mutex_unlock( &lock );
	}

	printf( "Ending watch for device %d\n", device->id );

	return NULL;
}

void DeviceManager_Init( void )
{
	should_watch = 1;
	pthread_mutex_init( &lock, NULL );
	device_arr = DeviceArray_Init( DEVICE_MANAGER_DEFAULT_SIZE );
	srand( time( NULL ) );
	heart_beat_time.tv_sec = 5;
	heart_beat_time.tv_nsec = 0;
}

device_t* DeviceManager_Register( struct sockaddr_in* addr )
{
	uint32_t id = rand();

	device_t* new_device = malloc( sizeof( device_t ) );
	new_device->id = id;
	new_device->last_seen = (uint64_t)time( NULL );
	new_device->address = addr;
	new_device->state = ALIVE;

	printf( "Registering new device: id %d, port %d\n", id, ntohs( addr->sin_port ) );

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

	printf( "Reregistering old device: id %d, port %d\n", id, ntohs( addr->sin_port ) );

	pthread_create( &new_device->watch_thread, NULL, &watch_device, (void*)new_device );

	DeviceArray_Put( device_arr, new_device );

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

		printf( "Heartbeat from %d", id );

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
	DeviceArray_Destroy( device_arr );
	pthread_mutex_destroy( &lock );
}
