#include <stdio.h>
#include <include/device_manager.h>
#include <include/udp_server.h>

#ifndef PORT
#define PORT 12345
#endif

int main( void )
{

	DeviceManager_Init();

	UDP_Server_Init( PORT );

	UDP_Server_Wait();

	DeviceManager_Shutdown();

	return 0;
}
