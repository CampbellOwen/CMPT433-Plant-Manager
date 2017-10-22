#include <include/device_array.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

device_array_t* DeviceArray_Init( int initialSize )
{
	device_array_t* newArr = malloc( sizeof( device_array_t ) );
	if( newArr == NULL ) {
		return NULL;
	}

	newArr->len = 0;
	newArr->total_size = initialSize;

	newArr->data = malloc( sizeof( device_t* ) * initialSize );
	if( newArr->data == NULL ) {
		free( newArr );
		return NULL;
	}

	return newArr;
}

void DeviceArray_Destroy( device_array_t* arr )
{
	for( int i = 0; i < arr->len; i++ ) {
		free( arr->data[i] );
	}
	free( arr->data );
	arr->data = NULL;

	free( arr );
	arr = NULL;
}

void resize_amount( device_array_t* arr, int amount ) 
{
	uint32_t new_size = arr->total_size + amount;
	arr->data = realloc( arr->data, sizeof( device_t* ) * new_size );
	if( errno != ENOMEM ) {
		arr->total_size = new_size;
	}
}

void resize_arr( device_array_t* arr )
{
	resize_amount( arr, arr->total_size );
}

int DeviceArray_Put( device_array_t* arr, device_t* p )
{
	if( arr->len >= arr->total_size ) {
		resize_arr( arr );
		
		// resize didn't work?
		if( arr->len >= arr->total_size ) {
			return -1;
		}
	}

	int index = arr->len;
	arr->data[ index ] = p;
	arr->len++;

	return index;
}

int DeviceArray_PutI( device_array_t* arr, device_t* p, int index )
{
	if( index < 0 ) {
		return -1;
	}
	if( index < arr->total_size ) {
		arr->data[ index ] = p;

		return index;
	}

	int new_size = (int)ceil( pow( 2, log2( index+1 ) ) );
	
	resize_amount( arr, new_size - arr->total_size );
	
	// resize didn't work?
	if( index >= arr->total_size ) {
		return -1;
	}

	arr->data[ index ] = p;

	return index;
}

device_t* DeviceArray_Get( device_array_t* arr, int index )
{
	if( index < 0 || index >= arr->len ) {
		return NULL;
	}

	return arr->data[ index ];
}
