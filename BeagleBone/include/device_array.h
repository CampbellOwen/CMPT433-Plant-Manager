#ifndef _DEVICE_ARRAY_H_
#define _DEVICE_ARRAY_H_

#include <include/device_manager.h>
#include <stdint.h>

typedef struct {
	uint32_t len;
	uint32_t total_size;
	device_t** data;
	
} device_array_t;

device_array_t* DeviceArray_Init( int initialSize );

void DeviceArray_Destroy( device_array_t* arr );

int DeviceArray_Put( device_array_t* arr, device_t* p );
int DeviceArray_PutI( device_array_t* arr, device_t* p, int index );

device_t* DeviceArray_Get( device_array_t* arr, int index );

#endif
