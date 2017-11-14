#include <include/udp_server.h>
#include <include/device_manager.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

#define UDP_SERVER_BUFFER_LENGTH 1024
#define UDP_SERVER_MAX_PACKET 1500

#define CATEGORY_CONFIG 'C'
#define CATEGORY_STATUS 'S'
#define CATEGORY_ACTION 'A'

#define CONFIG_REGISTER 'r'

#define STATUS_HEARTBEAT 'h'
#define STATUS_STATUS 's'

#define ACTION_ACTIVATE 'a'

static int serverfd;
static struct sockaddr_in serverAddr;
static pthread_t tid;
static int poll = 0;

static void UDP_Server_SendMessage( struct sockaddr_in* clientAddr, unsigned int client_len, char* message ) {
    int message_len = strlen( message );
    if( strlen( message ) < UDP_SERVER_MAX_PACKET ) {
        sendto( 
            serverfd, 
            message, 
            message_len,
            0,
            ( struct sockaddr* )clientAddr,
            client_len );
    }
    else {
        int pos = 0;
        while( pos < message_len ) {
            int len = UDP_SERVER_MAX_PACKET;
            if( pos + len >= message_len ) {
                len = message_len - pos;
            }
            sendto( 
                serverfd, 
                message + pos, 
                len,
                0,
                ( struct sockaddr* )clientAddr,
                client_len );
            pos += len;
        }   
    }
}

static void SendRegistration( struct sockaddr_in* clientAddr, uint32_t id )
{
	char buffer[ UDP_SERVER_MAX_PACKET ];
	uint32_t n_id = htonl( id );
	sprintf( buffer, "Cr" );
	memcpy( buffer + 2, &n_id, sizeof( uint32_t ) );
	buffer[ 2 + sizeof( uint32_t ) ] = '\0';

	UDP_Server_SendMessage( clientAddr, strlen( buffer ), buffer );
}

static void HandleRegistration( struct sockaddr_in* clientAddr, unsigned int client_len, char* buffer )
{
	device_t* new_device = DeviceManager_Register( clientAddr );
	SendRegistration( clientAddr, new_device->id );
}

static void HandleHeartbeat( struct sockaddr_in* clientAddr, unsigned int client_len, char* buffer )
{
	if( client_len < ( 2 + sizeof( uint32_t ) ) ) {
		return;
	}

	uint32_t id = 
				  buffer[2] << 24 |
				  buffer[3] << 16 |
				  buffer[4] << 8 |
				  buffer[5];
	uint32_t converted_id = ntohl( id );

	printf( "Heartbeat for id: %u\n", converted_id );

	DeviceManager_ReportHeartbeat( clientAddr, converted_id );

	UDP_Server_SendMessage( clientAddr, client_len, buffer );
}

static void HandleStatus( struct sockaddr_in* clientAddr, unsigned int client_len, char* buffer )
{
	// TODO: No status to report yet
}

static void HandleActivate( struct sockaddr_in* clientAddr, unsigned int client_len, char* buffer )
{
	// TODO nothing to activate yet
}

static void UDP_Server_HandleMessage( struct sockaddr_in* clientAddr, unsigned int client_len, char* buffer )
{
	if( client_len < 2 ) {
		return;
	}

	switch( buffer[ 0 ] ) {
		case CATEGORY_CONFIG:
			switch( buffer[ 1 ] ) {
				case CONFIG_REGISTER:
					HandleRegistration( clientAddr, client_len, buffer );
					break;
			}
			break;
		case CATEGORY_STATUS:
			switch( buffer[ 1 ] ) {
				case STATUS_HEARTBEAT:
					HandleHeartbeat( clientAddr, client_len, buffer );
					break;
				case STATUS_STATUS:
					HandleStatus( clientAddr, client_len, buffer );
			}

			break;
		case CATEGORY_ACTION:
			switch( buffer[ 1 ] ) {
				case ACTION_ACTIVATE:
					HandleActivate( clientAddr, client_len, buffer );
					break;
			}	
			break;

		default:
			return;
	}
}

static void* UDP_Server_Thread( void* args )
{
    struct sockaddr_in clientAddr;
    unsigned int client_len;
    client_len = sizeof( clientAddr );
    char buffer[ UDP_SERVER_BUFFER_LENGTH ];
    // int msg_size;

    while( poll ) {
        bzero( buffer, UDP_SERVER_BUFFER_LENGTH );
        recvfrom( 
            serverfd,
            buffer,
            UDP_SERVER_BUFFER_LENGTH,
            0,
            ( struct sockaddr* )&clientAddr,
            &client_len );
        UDP_Server_HandleMessage( &clientAddr, client_len, buffer );

    }
    return NULL;
}

int UDP_Server_Init( int port )
{
    serverfd = socket( AF_INET, SOCK_DGRAM, 0 );
    if( serverfd < 0 ) {
        fprintf( stderr, "Error creating socket\n" );
        return 0;
    }

    bzero( ( char* )&serverAddr, sizeof( serverAddr ) );
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl( INADDR_ANY );
    serverAddr.sin_port = htons( port );

    int res = bind( 
        serverfd, 
        ( struct sockaddr* )&serverAddr, 
        sizeof( serverAddr ) );
    if( res < 0 ) {
        fprintf( stderr, "Error binding socket to port %d\n", port );
        return 0;
    }
    poll = 1;
    pthread_create( &tid, NULL, &UDP_Server_Thread, NULL );

    return 1;
}

void UDP_Server_Wait( void )
{
    pthread_join( tid, NULL );
    close( serverfd );
}
