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
#include <elapsedMillis.h>

#include "SparkFunBME280.h"
#include <SparkFunLSM9DS1.h>
#include "Adafruit_MCP9808.h"

#define B_RATE     9600 // Serial baud rate
#define BUFF_SIZE  480  // TODO tailor to packet size
#define PACK_LIM   8    
#define CS_0       10   // CS0 pin for SPI

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
elapsedMillis poll_elapsed, since_init;

uint16_t poll_rate = 100; // in milliseconds
uint8_t buffer[BUFF_SIZE];
uint8_t *cursor = buffer;
uint8_t packet_count = 0;


void setup() {
  Serial.begin(B_RATE);

  while(!Serial) {
  }
  Serial.print("Serial init success\n");

  init();

}

void loop() {
  if(packet_count == PACK_LIM) {
    write_buffer();
  }
  
  if (poll_elapsed > poll_rate ) {
    poll_sensors();
    poll_elapsed = 0;
  }

}

void init() {
  //////////////////////////////////////
  // Setup SD Card
  Serial.print("Initialization: ");

  if (!SD.begin(CS_0)) {
    Serial.print("SD card initilization fail\n");
    return ;
  }

  log_file = SD.open("tester.bin", FILE_WRITE);

  if(!log_file) {
    Serial.println("File failed to open");
    for(;;){}
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
  delay(10);

  if(!bme280.begin()) {
    Serial.println("BME280 failed to initiate");

    for(;;){}
  }

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

void poll_sensors() {
  buffer_float(since_init);
  poll_bme280();
  poll_imu();
  poll_mcp();

  packet_count++;
}

void poll_bme280() {
  float temp_c   = bme280.readTempC();
  float pressure = bme280.readFloatPressure();
  float alt_m    = bme280.readFloatAltitudeMeters();
  float humidity = bme280.readFloatHumidity();

  buffer_float(temp_c);
  buffer_float(pressure);
  buffer_float(alt_m);
  buffer_float(humidity);
}

void poll_imu() {
  imu.readGyro();

  buffer_float(imu.calcGyro(imu.gx));
  buffer_float(imu.calcGyro(imu.gy));
  buffer_float(imu.calcGyro(imu.gz));

  imu.readAccel();

  buffer_float(imu.calcAccel(imu.ax));
  buffer_float(imu.calcAccel(imu.ay));
  buffer_float(imu.calcAccel(imu.az));


  imu.readMag();

  buffer_float(imu.calcMag(imu.mx));
  buffer_float(imu.calcMag(imu.my));
  buffer_float(imu.calcMag(imu.mz));

}

void poll_mcp() {
  mcp9808.shutdown_wake(0);

  buffer_float(mcp9808.readTempC());

  mcp9808.shutdown_wake(1);
}

void buffer_float(float in){
  memcpy(cursor, &in, sizeof(float));
  cursor = cursor + 4;
}

void write_buffer(){
  if(log_file) {
    log_file.write(buffer, BUFF_SIZE);
  } else {
    Serial.println("Error opening log_file");
  }

  cursor = buffer;
  packet_count = 0;
  log_file.flush();
}