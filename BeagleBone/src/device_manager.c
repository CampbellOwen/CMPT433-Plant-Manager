#include <include/device_manager.h>
#include <include/device_array.h>

#define DEVICE_MANAGER_DEFAULT_SIZE 64

static device_array_t* device_arr;

void DeviceManager_Init( void )
{
	device_arr = DeviceArray_Init( DEVICE_MANAGER_DEFAULT_SIZE );
}

uint32_t DeviceManager_Register( struct sockaddr_in* addr )
{
	uuid_t id;
	uuid_generate( id );
	device_t newDevice = {
		id,
		addr,
		ALIVE
	};
	
	DeviceArray_Put( device_arr, &newDevice );

	return id;
}

void DeviceManager_Shutdown()
{
	DeviceArray_Destroy( device_arr );
}
