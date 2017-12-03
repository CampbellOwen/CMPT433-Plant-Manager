#include <stdio.h>
#include <include/device_manager.h>
#include <include/udp_server.h>
#include <include/moisture.h>
#include <include/pid.h>
#include <include/i2c.h>
#include <include/seg_display.h>
#include <include/GPIO.h>

#ifndef PORT
#define PORT 12345
#endif

#define I2C_BUS 0
#define I2C_DEVICE_ADDRESS 0x20

void SetupSegDisplay() {
	i2c_t i2c;
	i2c.bus = I2C_BUS;
	I2C_InitBus(&i2c, I2C_DEVICE_ADDRESS);

	seg_display_t segDisplay;
	SegDisplay_Init(&segDisplay);
}

int main( void )
{
	if( !PID_Init() ) return 1;
	if( !DeviceManager_Init() ) return 1;
	if( !UDP_Server_Init( PORT ) ) return 1;

	SetupSegDisplay();

	UDP_Server_Wait();
	DeviceManager_Shutdown();
	PID_Shutdown();

	return 0;
}
