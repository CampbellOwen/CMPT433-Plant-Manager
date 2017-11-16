#ifndef _DHT_MOISTURE_H_
#define _DHT_MOISTURE_H_

#include <pthread.h>

#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_PIN "/sys/class/gpio/gpio"

#define DHT_DATA "33"
#define DHT_DATA_PIN GPIO_PIN DHT_DATA
#define DHT_DATA_DIRECTION DHT_DATA_PIN "/direction"
#define DHT_DATA_VALUE DHT_DATA_PIN "/value"

#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

void DHTmoisture_init();
void DHTmoisture_cleanup();

int Moisture_getValue();

pthread_t moistureThread;

// Moisture is on a scale of 0 to 100 (with 100 being more moist)
int currMoisture;

#endif
