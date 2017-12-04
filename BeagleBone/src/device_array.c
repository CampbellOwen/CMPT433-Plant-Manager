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
	pthread_mutex_init( &newArr->arr_lock, NULL );

	return newArr;
}

void DeviceArray_Destroy( device_array_t* arr )
{
	pthread_mutex_destroy( &arr->arr_lock );

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
	printf( "Resizing device array to %d\n", new_size );
}

void resize_arr( device_array_t* arr )
{
	resize_amount( arr, arr->total_size );
}

int DeviceArray_Put( device_array_t* arr, device_t* p )
{
	int index = -1;

	pthread_mutex_lock( &arr->arr_lock );
	{
		if( arr->len >= arr->total_size ) {
			resize_arr( arr );
			
			// resize didn't work?
			if( arr->len >= arr->total_size ) {
				return -1;
			}
		}

		index = arr->len;
		arr->data[ index ] = p;
		arr->len++;
	}
	pthread_mutex_unlock( &arr->arr_lock );

	return index;
}

int DeviceArray_PutI( device_array_t* arr, device_t* p, int index )
{
	pthread_mutex_lock( &arr->arr_lock );
	{
		if( index < 0 ) {
			index = -1;
		}
		else if( index < arr->total_size ) {
			arr->data[ index ] = p;
		}

		else {
			double log2 = log( index + 1 ) / log( 2 );
			int new_size = (int) pow( 2, ceil( log2 ) );
			
			resize_amount( arr, new_size - arr->total_size );
			
			// resize didn't work?
			if( index >= arr->total_size ) {
				index = -1;
			}
			else {
				arr->data[ index ] = p;
			}
		}
	}
	pthread_mutex_unlock( &arr->arr_lock );

	return index;
}

device_t* DeviceArray_Get( device_array_t* arr, int index )
{
	device_t* ret = NULL;
	pthread_mutex_lock( &arr->arr_lock );
	{
		if( index >= 0 && index < arr->len ) {
			ret = arr->data[ index ];
		}
	}
	pthread_mutex_unlock( &arr->arr_lock );

	return ret;
}

device_t* DeviceArray_GetId( device_array_t* arr, uint32_t id )
{
	device_t* ret = NULL;
	pthread_mutex_lock( &arr->arr_lock );
	{
		for( int i = 0; i < arr->len; i++ ) {
			if( arr->data[ i ]->id == id ) {
				ret = arr->data[ i ];
				break;
			}
		}
	}
	pthread_mutex_unlock( &arr->arr_lock );

	return ret;
}

device_t* DeviceArray_GetAlive( device_array_t* arr, int* len_out )
{
	device_t* alive_devices = NULL;
	*len_out = 0;
	pthread_mutex_lock( &arr->arr_lock );
	{
		alive_devices = malloc( sizeof( device_t ) * arr->len );
		if( alive_devices != NULL ) {
			for( int i = 0; i < arr->len; i++ ) {
				if( arr->data[ i ]->state == ALIVE ) {
					alive_devices[ *len_out ] = *arr->data[ i ];
					( *len_out )++;
				}
			}
		}
	}
	pthread_mutex_unlock( &arr->arr_lock );

	return alive_devices;
}

device_t* DeviceArray_GetAll( device_array_t* arr, int* len_out )
{
	device_t* alive_devices = NULL;
	*len_out = 0;
	pthread_mutex_lock( &arr->arr_lock );
	{
		alive_devices = malloc( sizeof( device_t ) * arr->len );
		if( alive_devices != NULL ) {
			for( int i = 0; i < arr->len; i++ ) {
                     alive_devices[ *len_out ] = *arr->data[ i ];
                     ( *len_out )++;
			}
		}
	}
	pthread_mutex_unlock( &arr->arr_lock );

	return alive_devices;
}
