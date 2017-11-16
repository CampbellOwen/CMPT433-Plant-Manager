#include "../include/dht_moisture.h"
#include "../include/general.h"
#include "../include/i2c.h"

int main(int argCoutn, char* args[]) {
  DHTmoisture_init();
  I2C_start();

  while (!General_isShuttingDown()) {
    printf("Moisture: %d\n", Moisture_getValue());
    sleep_msec(500);
  }
}
