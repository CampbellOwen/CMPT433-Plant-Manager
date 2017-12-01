#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include <string.h>
#include <EEPROM.h>

#define HOST_IP "192.168.7.2"
#define HOST_PORT 12345

#define BUFFER_SIZE 512

#define HEARTBEAT_TIME 5000

#define CATEGORY_CONFIG 'C'
#define CATEGORY_STATUS 'S'
#define CATEGORY_ACTION 'A'

#define CONFIG_REGISTER 'r'

#define STATUS_HEARTBEAT 'h'
#define STATUS_MOISTURE 'm'

#define ACTION_ACTIVATE 'a'
#define ACTION_PUMP 'p'

IPAddress hostip( 192, 168, 86, 45 );

WiFiUDP udp;
byte buffer[ BUFFER_SIZE ];
char reply[] = "hello there";
bool init_sent = false;
uint32_t id = 0;
bool have_id = false;
unsigned long curr_time;
unsigned long prev_time;

uint32_t try_read_id()
{
    EEPROM.begin( 512 );
    char is_saved = EEPROM.read( 10 );

    Serial.printf( "Saved bit: %x\n", is_saved );

    if( is_saved == 1 ) {
       have_id = true;
       uint32_t saved_id = 0;
        for( int i = 0; i < 4; i ++ ) {
            char segment = EEPROM.read( i+11 );
              saved_id = ( saved_id | ( segment << (i*8) ) );
        }
        Serial.printf( "Recovered id: %x\n", saved_id );
        EEPROM.end();
        return saved_id;
    }
    else {
        EEPROM.end();
        return 0;
    }
}

void save_id( uint32_t id )
{
    EEPROM.begin( 512 );

    Serial.printf( "Saving id: %x\n", id );
    Serial.println( "Writing to save-bit");
    EEPROM.write( 10, 1 );
    delay( 100 );
    char save_bit = EEPROM.read( 0 );
    Serial.printf( "Save-bit value: %x\n", save_bit );
    for( int i = 0; i < 4; i ++ ) {
          char segment = ( id & ( 0xFF << (i*8) ) ) >> (i*8);
          Serial.printf("\tSegment: %x\n", segment );
          EEPROM.write( i+11, segment );
          delay( 100 );
    }

    EEPROM.commit();
    EEPROM.end();
}

void setup()
{
//    EEPROM.begin( 512 );
    Serial.begin( 115200 );

    delay( 10000 );
//    Serial.println( "CLEARING" );
//    EEPROM.begin( 512 );
//    Serial.println( "Writing to save-bit");
//    EEPROM.write( 10, 0 );
//    delay( 100 );
//    EEPROM.commit();
//    EEPROM.end();
//    delay( 50000 );

    prev_time = millis();
    Serial.println( "Starting Wifi Manager" );
    WiFiManager wifiManager;
    wifiManager.autoConnect( "PLANT" );
    Serial.println( "WifiManager connected" );

    Serial.printf( "Starting UDP Server on port %d\n", HOST_PORT );
    udp.begin( 8088 );

    Serial.println( "Sending init packet to host" );

    id = try_read_id();

    if( !have_id ) {
        char message[] = { 'C', 'r' };

        udp.beginPacket( hostip, HOST_PORT);
        udp.write(message);
        udp.endPacket();
    }
}

uint32_t get32bitNumber( byte* buffer, int index )
{
    Serial.printf( "Pre swap: %x %x %x %x\n", buffer[index], buffer[index+1], buffer[index+2], buffer[index+3] );
     uint32_t id = 0;
     id =    buffer[index+3] << 24 |
             buffer[index+2] << 16 |
             buffer[index+1] << 8 |
             buffer[index];
	return ntohl( id );
    Serial.printf( "Post swap: %x\n", id);

}

void handle_message( byte* buffer, int len )
{
    if( len < 2 ) return;
    switch( buffer[ 0 ] ) {
        case CATEGORY_CONFIG:
            if( len < 6 ) break;
            switch( buffer[ 1 ] ) {
               case CONFIG_REGISTER:
                   id = get32bitNumber( buffer, 2 );
                   Serial.printf( "Received id: %u\n", id );
                   save_id( id );
                   have_id = true;
                   break;
            }
            break;
        case CATEGORY_STATUS:
            break;
        case CATEGORY_ACTION:

            break;
    }

    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write(reply);
    udp.endPacket();
}

void udp_loop()
{
  int packetSize = udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
    int len = udp.read(buffer, 255);
    if (len > 0)
    {
      buffer[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", buffer);
    if( len >= 6 ) {
          Serial.printf( "Hex number: %x %x %x %x\n", buffer[ 2 ], buffer[ 3 ], buffer[ 4 ], buffer[ 5 ] );
    }

    handle_message( buffer, len );
    // send back a reply, to the IP address and port we got the packet from
  }
}

void send_heartbeat()
{

     char message[6];
     message[ 0 ] = 'S';
     message[ 1 ] = 'h';
     uint32_t n_id = htonl( id );
     memcpy( &message[ 2 ], &n_id, sizeof( uint32_t ) );

     Serial.printf( "Sending heartbeat message: ");
     Serial.printf( "\t%x\n", message[ 0 ] );
     Serial.printf( "\t%x\n", message[ 1 ] );
     Serial.printf( "\t%x\n", message[ 2 ] );
     Serial.printf( "\t%x\n", message[ 3 ] );
     Serial.printf( "\t%x\n", message[ 4 ] );
     Serial.printf( "\t%x\n", message[ 5 ] );

    udp.beginPacket( hostip, HOST_PORT);
    udp.write(message);
    udp.endPacket();
}

void loop()
{
    udp_loop();

    curr_time = millis();
    if( have_id && curr_time - prev_time >= HEARTBEAT_TIME ) {
        Serial.println( "Sending heartbeat" );
        send_heartbeat();
        prev_time = curr_time;
    }
}
