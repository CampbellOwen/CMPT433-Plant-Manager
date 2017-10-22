#include <include/device_array.h>
#include <include/device_manager.h> 
#include <assert.h> 
#include "device_array_test.h"
#include <stdlib.h>
#include <netinet/in.h>

#define ARR_LENGTH 64

void device_array_test()
{
	device_array_t* arr = DeviceArray_Init( ARR_LENGTH );
	assert( arr != NULL );
	assert( arr->len == 0 );
	assert( arr->total_size == ARR_LENGTH );

	struct sockaddr_in test_addr;
	(void)test_addr;
	device_t new_device = {
		1,
		&test_addr,
		ALIVE
	};

	int index = DeviceArray_Put( arr, &new_device );
	assert( arr->len == index+1 );
	assert( index == 0 );

	device_t* test_device = DeviceArray_Get( arr, 0 );
	assert( test_device == &new_device );
		
	int curr_size = arr->total_size;
	for( int i = 0; i <= curr_size; i++ ) {
		DeviceArray_PutI( arr, &new_device, i );
	}
	assert( arr->total_size > ARR_LENGTH );
}


