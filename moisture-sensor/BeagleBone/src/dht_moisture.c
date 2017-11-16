// CMPT 433: Project
// Make sure to ADC cape is already loaded: echo BB-ADC > /sys/devices/platform/bone_capemgr/slots

// How to connect moisture sensor to BeagleBone
// SH (sensor's first pin): Humidity/Temperature; connect to a digital pin
// SM (sensor's second pin): Moisture; connect to AIN1 (P9_40)
// + (sensor's third pin): 3.3V/5V
// - (sensor's fourth pin): Gnd

#include <stdlib.h>
#include <string.h>

#include "../include/dht_moisture.h"
#include "../include/general.h"
#include "../include/i2c.h"

// Maximum amount of time to spin in a loop before bailing out and considering the read a timeout
#define DHT_MAXCOUNT 32000

// Number of bit pulses to expect from the DHT
// 41 because the first pulse is a constant 50 microsecond pulse, with 40 pulses to represent the data afterwards
#define DHT_PULSES 41

// TODO(samantha): Have to read whether it's a 1 or 0 per pulse
void DHT_read() {}

void DHT_init() {
  WriteToFile((char *) GPIO_EXPORT, (char*) DHT_DATA);
  WriteToFile((char *) DHT_DATA_DIRECTION, "in");
}

int getVoltage1Reading() {
  FILE *f = fopen(A2D_FILE_VOLTAGE1, "r");
  if (!f) {
    printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
    printf("try: echo BB-ADC > /sys/devices/platform/bone_capemgr/slots\n");
    exit(-1);
  }

  // Get reading
  int a2dReading = 0;
  int itemsRead = fscanf(f, "%d", &a2dReading);

  if (itemsRead <= 0) {
    printf("ERROR: Unable to read values from voltage input file.\n");
    exit(-1);
  }

  // Close file
  fclose(f);
  return a2dReading;
}

void Moisture_read() {
  currMoisture = ((double) getVoltage1Reading() / A2D_MAX_READING) * 100;
}

static void* Moisture_collect() {
  while (!General_isShuttingDown()) {
    Moisture_read();
    I2C_setMoisture(currMoisture);
    sleep_msec(500);
  }

  return NULL;
}

int Moisture_getValue() {
  return currMoisture;
}

void Moisture_init() {
  currMoisture = 0;
}

void DHTmoisture_init() {
  DHT_init();
  Moisture_init();

  pthread_create(&moistureThread, NULL, &Moisture_collect, NULL);
}
