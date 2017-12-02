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

void Moisture_setup()
{
   dht.begin();
}

void Moisture_getSensorData()
{
  // Moisture is on a scale of 0 to 100 (with 100 being more moist)
  int moistureValue = (analogRead(MOISTURE_PIN) / A2D_MAX_READING) * 100;
  Serial.println(moistureValue);

  float humidity = dht.readHumidity();

  // Read temperature as Celsius (the default)
  float tempCelsius = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float tempFahr = dht.readTemperature(true);

  // Check if any reads failed
  if (isnan(humidity) || isnan(tempCelsius) || isnan(tempFahr)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float heatIndexFahr = dht.computeHeatIndex(tempFahr, humidity);
  // Compute heat index in Celsius (isFahreheit = false)
  float heatIndexCelsius = dht.computeHeatIndex(tempCelsius, humidity, false);

  Serial.println(humidity);
  Serial.println(tempCelsius);
  Serial.println(tempFahr);
  Serial.println(heatIndexCelsius);
  Serial.println(heatIndexFahr);
}

void Moisture_readSerial()
{
  if (Serial.available() > 0)
  {
    String readString = Serial.readStringUntil('\n');

    if (readString == "get sensor data")
    {
      Moisture_getSensorData();
    }
  }
}
