#ifndef _UDP_Server_H_
#define _UDP_Server_H_

#include <include/device_manager.h>

int UDP_Server_Init( int port );

void UDP_Server_RequestSensor( device_t device, char sensorType );

void UDP_Server_Wait( void );

#endif
