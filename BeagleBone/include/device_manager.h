#ifndef _DEVICE_MANAGER_H_
#define _DEVICE_MANAGER_H_

#include <stdint.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <uuid.h>

enum connection_status {
	ALIVE,
	DEAD
};

typedef struct {
	uuid_t id;
	struct sockaddr_in* address;
	enum connection_status curr_status;

} device_t;

void DeviceManager_Init( void );

uint32_t DeviceManager_Register( struct sockaddr_in* addr );

void DeviceManager_Shutdown( void );

#endif
