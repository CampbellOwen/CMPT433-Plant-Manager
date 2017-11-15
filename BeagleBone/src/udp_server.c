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
#include <sqlite3.h>

#define UDP_SERVER_BUFFER_LENGTH 1024
#define UDP_SERVER_MAX_PACKET 1500

#define CATEGORY_CONFIG 'C'
#define CATEGORY_STATUS 'S'
#define CATEGORY_ACTION 'A'

#define CONFIG_REGISTER 'r'

#define STATUS_HEARTBEAT 'h'
#define STATUS_MOISTURE 'm'

#define ACTION_ACTIVATE 'a'

#define DB_NAME "plants.db"
#define INSERT_MOISTURE "INSERT INTO moisture (id, time, value) VALUES ( %u, %u, %lld );"

static int serverfd;
static struct sockaddr_in serverAddr;
static pthread_t tid;
static int poll = 0;
static sqlite3* db;

static uint32_t get_uint32_t( char* buffer, int index )
{
     uint32_t id =    buffer[index] << 24 |
				  buffer[index+1] << 16 |
				  buffer[index+2] << 8 |
				  buffer[index+3];
	return ntohl( id );
}

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
{ device_t* new_device = DeviceManager_Register( clientAddr );
	SendRegistration( clientAddr, new_device->id );
}

static void HandleHeartbeat( struct sockaddr_in* clientAddr, unsigned int client_len, char* buffer )
{
	if( client_len < ( 2 + sizeof( uint32_t ) ) ) {
		return;
	}
     
     uint32_t id = get_uint32_t( buffer, 2 );

	printf( "Heartbeat for id: %u\n", id );

	DeviceManager_ReportHeartbeat( clientAddr, id );

	UDP_Server_SendMessage( clientAddr, client_len, buffer );
}

static void HandleMoisture( struct sockaddr_in* clientAddr, unsigned int client_len, char* buffer )
{
    if( client_len < ( 2 + sizeof( uint32_t ) ) ) {
        return;
    }

     uint32_t id = get_uint32_t( buffer, 2 );
     uint32_t value = get_uint32_t( buffer, 2 + sizeof( uint32_t ) );
     long long curr_time = ( long long )time( NULL );

     printf( "Received moisture data from id: %u, value: %u\n", id, value );

    char sql_statement[ UDP_SERVER_BUFFER_LENGTH ];

    sprintf( sql_statement, INSERT_MOISTURE, id, value, curr_time );

    char* err_msg = NULL;

    int ret = sqlite3_exec( db, sql_statement, NULL, NULL, &err_msg );
    if( ret != SQLITE_OK ) {
          fprintf( stderr, "Error writing to SQL: %s\n", err_msg );
          sqlite3_free( err_msg );
    }
    else {
        printf( "Values succesfully stored in db\n" );
    }

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
				case STATUS_MOISTURE:
					HandleMoisture( clientAddr, client_len, buffer );
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
    int ret = sqlite3_open( DB_NAME, &db );

    if( ret ) {
      fprintf( stderr, "Can't open database: %s\n", sqlite3_errmsg( db ) );
      return( 0 );
    } 
    
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
    sqlite3_close( db );
}
