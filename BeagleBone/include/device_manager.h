#ifndef _DEVICE_MANAGER_H_
#define _DEVICE_MANAGER_H_

#include <stdint.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

enum connection_status {
	ALIVE,
	TIMEOUT
};

typedef struct {
	uint32_t id;
	uint64_t last_seen;
	struct sockaddr_in* address;
	enum connection_status state;
	pthread_t watch_thread;

} device_t;

int DeviceManager_Init( void );

device_t* DeviceManager_Register( struct sockaddr_in* addr );

device_t* DeviceManager_Reregister( struct sockaddr_in* addr, uint32_t id );

int DeviceManager_ReportHeartbeat( struct sockaddr_in* addr, uint32_t id );

device_t* DeviceManager_GetDevice( uint32_t id );

void DeviceManager_Shutdown( void );

void DeviceManager_SaveMoistureData( device_t* device, uint32_t value );

#endif
