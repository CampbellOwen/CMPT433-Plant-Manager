# Plant Manager (Can't Be Beet)
Owen Campbell; Dolapo Toki; Samantha Yu

## BeagleBone
### How to Build BeagleBone Source Code
* Mount `cmpt433/public` folder to BeagleBone over NFS
  * See http://www.cs.sfu.ca/CourseCentral/433/bfraser/other/NFSGuide.pdf for more information
  * We'll assume that `cmpt433/public` on host gets mapped to `$(HOME)/cmpt433/public/` on the BeagleBone
* `$ cd BeagleBone`
* `$ make`
  * Creates the executable `plantManager` in `cmpt433/public`
  * Copies `sql/init.sql` to BeagleBone's `$(HOME)/cmpt433/public/myApps/init.sql`
  * Copies `scripts/createPlantDB` to BeagleBone's `$(HOME)/createPlantDB` and makes this script executable

### How to Run PlantManager Application on BeagleBone
* Run `./createPlantDB` to create the plant database in `$HOME/plants.db$`
* Run `./plantManager` to start the plant manager application

### How to View Database in the BeagleBone
* `# cd ~`
* `# sqlite3 plants.db`

### How to Run Web Dashboard
* Run `$ make website` from the BeagleBone folder
* Run `# ./start_flask.sh` on the target to launch the web server
* Connect to `http://target-ip:5000`

## ESP8266
### How to Build and Upload ESP8266 Source Code
* `$ cd esp`
* Use `PlatformIO` to build and send to the connected ESP8266
* To view print statements on the ESP8266, open the serial monitor

### How to Connect ESP8266 to Network
* If the ESP8266 does not have network credentials saved, it will create a `PLANT` WiFi Access Point
* Connect to this Access Point and enter the SSID and password of the network the BeagleBone is connected to
* ESP8266 will automatically connect to that network moving forward
* ESP8266 main.cpp needs to contain the IP Address of the BeagleBone

## Pump
* MOSFET gate pin should be connected to the D0 pin of the ESP8266

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
