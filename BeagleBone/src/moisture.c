// CMPT 433: Project
// Make sure to ADC cape is already loaded: echo BB-ADC > /sys/devices/platform/bone_capemgr/slots

// How to connect moisture sensor to BeagleBone
// SH (sensor's first pin): Humidity/Temperature; connect to a digital pin
// SM (sensor's second pin): Moisture; connect to AIN1 (P9_40)
// + (sensor's third pin): 3.3V/5V
// - (sensor's fourth pin): Gnd

#include <stdlib.h>
#include <string.h>

#include <include/A2D.h>
#include <include/moisture.h>

static int should_run = 0;
static struct timespec delay_time = { 0, 500000000 }; // Sleep for 500ms

void Moisture_read() {
  currMoisture = ((double) A2D_Read(&ain1) / MOISTURE_MAX_READING) * 100;
}

static void* Moisture_collect() {
  if (should_run) {
    Moisture_read();
    // I2C_setMoisture(currMoisture);
    nanosleep( &delay_time, ( struct timespec* ) NULL );
  }

  return NULL;
}

int Moisture_getValue() {
  return currMoisture;
}

void Moisture_init() {
  should_run = 1;
  currMoisture = 0;
  ain1.pin = AIN1_PIN;
  A2D_Init(&ain1);

  pthread_create(&moistureThread, NULL, &Moisture_collect, NULL);
}

void Moisture_cleanup() {
  should_run = 0;
}
