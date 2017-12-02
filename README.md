## DHT Moisture Sensor
### How to Connect Moisture Sensor to BeagleBone
* SH (sensor's first pin): Humidity/Temperature
  * Connect to a digital pin: GPIO_39 - P8_4
* SM (sensor's second pin): Moisture
  * Connect to an analog pin: AIN1 - P9_40
* \+ (sensor's third pin): 3.3V/5V
* \- (sensor's fourth pin): Gnd
* Don't forget to add pull-up resistors
  * BeagleBone analog inputs take in only 1.8V; sensor operates at 3V-5V max

### How to Connect Moisture Sensor to ESP8266
* SH (sensor's first pin): Humidity/Temperature
  * Connect to a digital pin: D2
* SM (sensor's second pin): Moisture
  * Connect to an analog pin: A0
* \+ (sensor's third pin): 3.3V/5V
* \- (sensor's fourth pin): Gnd
