// CMPT 433: Project
// References: https://www.osepp.com/electronic-modules/breakout-boards/94-humidity-moisture-breakout
// According to OSEPP's sample code, "Reading temperature or humidity takes about 250 milliseconds! Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)"

#include "DHT.h"
#define DHTPIN D2
#define DHTTYPE DHT11     // DHT 11
#define MOISTURE_PIN A0   // select the input pin for the potentiometer

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
   Serial.begin(115200);
   dht.begin();
}

void getSensorData()
{
  int moistureValue = analogRead(MOISTURE_PIN);
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

void readInSerial()
{
  if (Serial.available() > 0)
  {
    String readString = Serial.readStringUntil('\n');
    
    if (readString == "get sensor data")
    {
      getSensorData();
    }
  }
}
 
void loop()
{
  delay(1000);
  readInSerial();
}
