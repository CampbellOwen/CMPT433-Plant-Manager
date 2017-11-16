// CMPT 433: Assignment 2 - Embedded Linux Programming
// Samantha Yu
// Date: October 10, 2017
// i2c.h

// Assumes ADC cape is already loaded:
//    root@beaglebone:/# echo BB-ADC > /sys/devices/platform/bone_capemgr/slots

#ifndef _I2C_H_
#define _I2C_H_

#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO61_DIR "/sys/class/gpio/gpio61/direction"
#define GPIO44_DIR "/sys/class/gpio/gpio44/direction"
#define GPIO61_VAL "/sys/class/gpio/gpio61/value"
#define GPIO44_VAL "/sys/class/gpio/gpio44/value"

#define CAPE_SLOTS "/sys/devices/platform/bone_capemgr/slots"

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x20

#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

pthread_t i2cThread;
int i2cFileDesc;
struct timespec stayOnDuration;

int currMoisture;

void I2C_start();
void I2C_stop();
void I2C_setMoisture(int moisture);

#endif /*_I2C_H_*/
