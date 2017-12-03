// CMPT 433: Project
// References: https://www.osepp.com/electronic-modules/breakout-boards/94-humidity-moisture-breakout
// According to OSEPP's sample code:
// "Reading temperature or humidity takes about 250 milliseconds! Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)"

// How to connect moisture sensor to ESP8266
// SH (sensor's first pin): Humidity/Temperature; connect to a digital pin
// SM (sensor's second pin): Moisture; connect to A0
// + (sensor's third pin): 3.3V/5V
// - (sensor's fourth pin): Gnd

#include <Adafruit_Sensor.h>
#include <DHT.h>                  // DHT Moisture sensor

#define DHTPIN D2
#define DHTTYPE DHT11     // DHT 11
#define MOISTURE_PIN A0   // select the input pin for the potentiometer

#define A2D_MAX_READING 4095

DHT dht(DHTPIN, DHTTYPE);

static int moistureValue = 0;
static float humidity = 0;
static float tempCelsius = 0;
static float heatIndexCelsius = 0;

void Moisture_setup()
{
   dht.begin();
}

int Moisture_getMoisture()
{
  // Moisture is on a scale of 0 to 100 (with 100 being more moist)
  // A0's input voltage range is from 0 to 1V
  int moisture = analogRead(MOISTURE_PIN);

  // Check if reading from the sensor failed
  if (isnan(moisture)) {
    Serial.println("Failed to read from moisture sensor!");
    return -1;
  }

  return moisture;
}

int Moisture_getHumidity()
{
  int humidity = dht.readHumidity();

  // Check if reading from the humidity failed
  if (isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }

  Serial.printf("Humidity: %d\n", humidity);
  return humidity;
}

int Moisture_getTemperature()
{
  // Read temperature as Celsius (the default)
  int tempCelsius = dht.readTemperature();

  // Check if any reads failed
  if (isnan(tempCelsius)) {
    Serial.println("Failed to read from DHT sensor!");
    return -1;
  }

  Serial.printf("Temperature: %d\n", tempCelsius);
}
