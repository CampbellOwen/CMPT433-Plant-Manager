// CMPT 433: Project
// Based off of Assignment 2's submission

// Assumes ADC cape is already loaded:
//    root@beaglebone:/# echo BB-ADC > /sys/devices/platform/bone_capemgr/slots

#include "../include/i2c.h"

#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include "../include/dht_moisture.h"
#include "../include/general.h"

static void I2C_writeFile(char* filename, char* value) {
	FILE* pfile = fopen(filename, "w");
	if (pfile == NULL) {
		perror("Unable to open file to write to.\n");
		exit(-1);
	}

	fprintf(pfile, "%s", value);
	fclose(pfile);
}

static int I2C_initI2Cbus(char* bus, int address) {
	int i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		exit(-1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}
	return i2cFileDesc;
}

static void I2C_writeI2Creg(int i2cFileDesc, unsigned char regAddr, unsigned char value) {
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		exit(-1);
	}
}

void I2C_setMoisture(int moisture) {
	pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_lock(&myMutex);
	{
		currMoisture = moisture;
	}
	pthread_mutex_unlock(&myMutex);
}

static void I2C_interpretDigit(int digit) {
	if (digit == 1) {
		I2C_writeI2Creg(i2cFileDesc, REG_OUTA, 0x80);
		I2C_writeI2Creg(i2cFileDesc, REG_OUTB, 0x2);
	}
	else if (digit == 2) {
		I2C_writeI2Creg(i2cFileDesc, REG_OUTA, 0x31);
		I2C_writeI2Creg(i2cFileDesc, REG_OUTB, 0xE);
	}
	else if (digit == 3) {
		I2C_writeI2Creg(i2cFileDesc, REG_OUTA, 0xB0);
		I2C_writeI2Creg(i2cFileDesc, REG_OUTB, 0xE);
	}
	else if (digit == 4) {
		I2C_writeI2Creg(i2cFileDesc, REG_OUTA, 0x90);
		I2C_writeI2Creg(i2cFileDesc, REG_OUTB, 0x8A);
	}
	else if (digit == 5) {
		I2C_writeI2Creg(i2cFileDesc, REG_OUTA, 0xB0);
		I2C_writeI2Creg(i2cFileDesc, REG_OUTB, 0x8C);
	}
	else if (digit == 6) {
		I2C_writeI2Creg(i2cFileDesc, REG_OUTA, 0xB1);
		I2C_writeI2Creg(i2cFileDesc, REG_OUTB, 0x8C);
	}
	else if (digit == 7) {
		I2C_writeI2Creg(i2cFileDesc, REG_OUTA, 0x80);
		I2C_writeI2Creg(i2cFileDesc, REG_OUTB, 0x6);
	}
	else if (digit == 8) {
		I2C_writeI2Creg(i2cFileDesc, REG_OUTA, 0xB1);
		I2C_writeI2Creg(i2cFileDesc, REG_OUTB, 0x8E);
	}
	else if (digit == 9) {
		I2C_writeI2Creg(i2cFileDesc, REG_OUTA, 0x90);
		I2C_writeI2Creg(i2cFileDesc, REG_OUTB, 0x8E);
	}
	else if (digit == 0) {
		I2C_writeI2Creg(i2cFileDesc, REG_OUTA, 0xA1);
		I2C_writeI2Creg(i2cFileDesc, REG_OUTB, 0x86);
	}
}

static void I2C_turnOffBothDigits() {
	I2C_writeFile(GPIO61_VAL, "0");
	I2C_writeFile(GPIO44_VAL, "0");
}

static void I2C_leftDigit() {
	if (currMoisture < 100) {
		I2C_interpretDigit((currMoisture / 10) % 10);
	}
	else {
		I2C_interpretDigit(9);
	}

 	I2C_writeFile(GPIO61_VAL, "1");
	nanosleep(&stayOnDuration, NULL);
	I2C_turnOffBothDigits();
}

static void I2C_rightDigit() {
	if (currMoisture < 100) {
		I2C_interpretDigit(currMoisture % 10);
	}
	else {
		I2C_interpretDigit(9);
	}

	I2C_writeFile(GPIO44_VAL, "1");
	nanosleep(&stayOnDuration, NULL);
	I2C_turnOffBothDigits();
}

static void I2C_setup() {
	// Configure both pins for output through GPIO
	I2C_writeFile(GPIO_EXPORT, "61");
	I2C_writeFile(GPIO_EXPORT, "44");

	I2C_writeFile(GPIO61_DIR, "out");
	I2C_writeFile(GPIO44_DIR, "out");

	// Enable /dev/i2c-1
	I2C_writeFile(CAPE_SLOTS, "BB-I2C1");

	i2cFileDesc = I2C_initI2Cbus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

	// Set direction of both 8-bit ports to output
	I2C_writeI2Creg(i2cFileDesc, REG_DIRA, 0x00);
	I2C_writeI2Creg(i2cFileDesc, REG_DIRB, 0x00);
}

static void* I2C_display() {
	I2C_setup();

  while(!General_isShuttingDown()) {
		I2C_leftDigit();
		I2C_rightDigit();
  }

  return NULL;
}

void I2C_start() {
	currMoisture = (int) Moisture_getValue();

	// Leave a digit on for 5000000ns = 5ms before switching to the other digit
	stayOnDuration.tv_nsec = 5000000;
  pthread_create(&i2cThread, NULL, &I2C_display, NULL);
}

void I2C_stop() {
	I2C_turnOffBothDigits();
}
