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

void DeviceManager_Init( void );

uint32_t DeviceManager_Register( struct sockaddr_in* addr );

int DeviceManager_ReportHeartbeat( uint32_t id );

void DeviceManager_Shutdown( void );

#endif
