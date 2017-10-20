#include <include/udp_server.h>

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


static void UDP_Server_HandleMessage( struct sockaddr_in* clientAddr, unsigned int client_len, char* buffer )
{
    int len = 0;
    char* tokens[ UDP_SERVER_BUFFER_LENGTH ];
    char delims[] = { '\t', '\n', ' ' }; 
    for( char* token = strtok( buffer, delims ); token != NULL; token = strtok( NULL, " " ) ) {
        if( token[ strlen( token ) - 1 ] == '\n' ) {
            token[ strlen( token ) - 1] = '\0';
        }
        tokens[ len++ ] = token;     
		(void)tokens;
    }

    if( len == 0 ) {
        UDP_Server_SendMessage( clientAddr, client_len, "Invalid Command\n");                 
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
        fprintf( stderr, "Erro binding socket to port %d\n", port );
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
