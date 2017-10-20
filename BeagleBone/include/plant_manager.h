#ifndef _PLANT_MANAGER_H_
#define _PLANT_MANAGER_H_

#include <stdint.h>
#include <netinet/in.h>
#include <sys/socket.h>

enum connection_status {
	ALIVE,
	DEAD
};

typedef struct {
	uint64_t id;
	struct sockaddr_in* address;
	enum connection_status curr_status;

} plant_t;

void PlantManager_Init( void );

void PlantManager_Register( struct sockaddr_in* addr, uint64_t id );

#endif
