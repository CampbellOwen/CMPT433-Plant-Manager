#include <stdio.h>
#include <include/device_manager.h>
#include <include/udp_server.h>

int main( void )
{

	DeviceManager_Init();

	UDP_Server_Init( 12345 );

	UDP_Server_Wait();

	DeviceManager_Shutdown();

	return 0;
}
