#ifndef _DEVICE_ARRAY_H_
#define _DEVICE_ARRAY_H_

#include <include/device_manager.h>
#include <stdint.h>
#include <pthread.h>

typedef struct {
	uint32_t len;
	uint32_t total_size;
	device_t** data;
	pthread_mutex_t arr_lock;
	
} device_array_t;

device_array_t* DeviceArray_Init( int initialSize );

void DeviceArray_Destroy( device_array_t* arr );

int DeviceArray_Put( device_array_t* arr, device_t* p );
int DeviceArray_PutI( device_array_t* arr, device_t* p, int index );

device_t* DeviceArray_Get( device_array_t* arr, int index );

device_t* DeviceArray_GetId( device_array_t* arr, uint32_t id );

//User must free the array returned, but none of the contents
device_t* DeviceArray_GetAlive( device_array_t* arr, int* len_out );

#endif
