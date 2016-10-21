////////////////////////
/// RIT SPEX HAB
/// Main Flight Software
/// October 20th, 2016
/// 
/// Austin Bodzas
////////////////////////

// #include <Wire.h>
#include <stdint.h>

#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#include "SparkFunBME280.h"
#include <SparkFunLSM9DS1.h>

#define B_RATE 9600 // Serial baud rate
#define CS_0 10 // CS0 pin for SPI


File log_file;
BME280 bme280;

void setup() {
  Serial.begin(B_RATE);

  while(!Serial) {
  }

  //////////////////////////////////////
  // Setup SD Card
  Serial.print("Initialization: ");

  if (!SD.begin(CS_0)) {
    Serial.print("FAIL\n");
    return ;
  }

  Serial.print("SUCCESS\n");

  //////////////////////////////////////
  // Setup BME280
  bme280.settings.comminterface   = I2C_MODE;
  bme280.settings.I2CAddress      = 0x77;
  bme280.settings.runmode         = 3;
  bme280.settings.tStandby        = 0;
  bme280.settings.filter          = 0;
  bme280.settings.tempOverSample  = 1;
  bme280.settings.humidOverSample = 1;

}

void loop() {
  log_file = SD.open("log.txt", FILE_WRITE);

  log_file.close();
}
