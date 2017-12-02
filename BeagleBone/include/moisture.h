#ifndef _MOISTURE_H_
#define _MOISTURE_H_

#include <pthread.h>
#include <include/A2D.h>

#define AIN1_PIN 1
#define MOISTURE_VOLTAGE_REF_V 1.8
#define MOISTURE_MAX_READING 4095

void Moisture_init();
int Moisture_getValue();

A2D_t ain1;
pthread_t moistureThread;

// Moisture is on a scale of 0 to 100 (with 100 being more moist)
int currMoisture;

#endif
