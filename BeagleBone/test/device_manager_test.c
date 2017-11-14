#include "device_manager_test.h"
#include <include/device_manager.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

void device_manager_test()
{
	struct timespec sleep_time = { 5, 0 };

	DeviceManager_Init();

	struct sockaddr_in test_socket_addr;
	test_socket_addr.sin_port = htons( 12 );

	uint32_t id = DeviceManager_Register( &test_socket_addr );

	for( int i = 0; i < 2; i++ ) {
		nanosleep( &sleep_time, NULL );
		DeviceManager_ReportHeartbeat( id );
	}

	sleep_time.tv_sec = 15;
	nanosleep( &sleep_time, NULL );

	device_t* test_device = DeviceManager_GetDevice( id );
	assert( test_device->state == TIMEOUT );

	DeviceManager_ReportHeartbeat( id );
	sleep_time.tv_sec = 1;
	nanosleep( &sleep_time, NULL );

	(void)id;
	DeviceManager_Shutdown();
}
