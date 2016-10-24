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
#include "Adafruit_MCP9808.h"

#define B_RATE     9600 // Serial baud rate
#define CS_0       10 // CS0 pin for SPI

// Magnetic field declination, RIT Nov, 21, 2016
// TODO confirm format of declination
#define DECL       -11.44 // 11.44 degrees West

// LSM9DS1 I2C
#define LSM9DS1_M	 0x1E 
#define LSM9DS1_AG 0x6B 



File log_file;
BME280 bme280;
LSM9DS1 imu;
Adafruit_MCP9808 mcp9808 = Adafruit_MCP9808();


void setup() {
  Serial.begin(B_RATE);

  while(!Serial) {
  }
  Serial.print("Serial init success\n");

  init();

}

void loop() {
  //////////////////////////////////////
  // set up sensors and file
  log_file = SD.open("log.txt", FILE_WRITE);
  // wake up mcp9808
  mcp9808.shutdown_wake(0);
  //////////////////////////////////////



  //////////////////////////////////////
  // clean up
  mcp9808.shutdown_wake(0);
  log_file.close();
  //////////////////////////////////////
}

void init() {
  //////////////////////////////////////
  // Setup SD Card
  Serial.print("Initialization: ");

  if (!SD.begin(CS_0)) {
    Serial.print("SD card initilization fail\n");
    return ;
  }

  //////////////////////////////////////
  // Setup BME280
  bme280.settings.commInterface   = I2C_MODE;
  bme280.settings.I2CAddress      = 0x77;
  bme280.settings.runMode         = 3;
  bme280.settings.tStandby        = 0;
  bme280.settings.filter          = 0;
  bme280.settings.tempOverSample  = 1;
  bme280.settings.humidOverSample = 1;

  //////////////////////////////////////
  // Setup LSM9DS1
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;

  if(!imu.begin()) {
    Serial.println("LSM9DS1 failed to initiate");

    for( ;; ) {
    }
  }

  //////////////////////////////////////
  // Setup MCP9808
  if (!mcp9808.begin()) {
    Serial.println("MCP9808 failed to initiate");

    for ( ;; ) {
    }
  }

  Serial.println("Initilization success");
}
