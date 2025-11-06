/*******************************************************
 *  BME280 I2C Sensor Test
 *  Compatible with Arduino UNO/Nano and ESP32 / ESP32-S3
 *
 *  Measures:
 *    - Temperature (°C)
 *    - Pressure (hPa)
 *    - Altitude (m)
 *    - Humidity (%)
 *
 *  Supports both:
 *    - Arduino AVR boards (fixed I2C pins)
 *    - ESP32 boards (customizable I2C pins)
 *
 *  Library dependencies:
 *    - Adafruit_Sensor
 *    - Adafruit_BME280
 *
 *******************************************************/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// --------------------------------------
// Constants and Configuration
// --------------------------------------
#define SEALEVELPRESSURE_HPA (1012)  // Adjust for your location

// ======== BOARD SELECTION ========
// Uncomment only ESP32 is going t be used:
#define USE_ESP32

// Define I2C pins for ESP32
#if defined(USE_ESP32)
  const int SDA_PIN = 8;
  const int SCL_PIN = 9;
#endif

// I2C sensor address (check with scanner if unsure)
uint8_t sensor_address = 0x76;

// --------------------------------------
// Objects and Variables
// --------------------------------------
Adafruit_BME280 bme;  // Create BME280 object (I2C)
unsigned long delayTime = 5000; // Delay between readings (ms)

// --------------------------------------
// Setup
// --------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println(F("\n=== BME280 Sensor Test ==="));

  // Initialize I2C
#if defined(USE_ESP32)
  Serial.println(F("Initializing I2C on ESP32..."));
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.print(F("SDA pin: ")); Serial.println(SDA_PIN);
  Serial.print(F("SCL pin: ")); Serial.println(SCL_PIN);
#else
  Serial.println(F("Initializing I2C on Arduino..."));
  Wire.begin();  // Arduino uses fixed SDA=A4, SCL=A5
#endif

  // Initialize BME280 sensor
  Serial.println(F("Initializing BME280..."));
  if (!bme.begin(sensor_address)) {
    Serial.println(F("❌ Could not find a valid BME280 sensor!"));
    Serial.println(F("Check wiring or try address 0x77."));
    while (1); // Stop here if sensor not found
  }

  // Configure sensor sampling mode
  bme.setSampling(
    Adafruit_BME280::MODE_FORCED,     // Forced mode = single measurement per trigger
    Adafruit_BME280::SAMPLING_X1,     // Temperature oversampling
    Adafruit_BME280::SAMPLING_X1,     // Pressure oversampling
    Adafruit_BME280::SAMPLING_X1,     // Humidity oversampling
    Adafruit_BME280::FILTER_OFF       // No filtering
  );

  Serial.println(F("✅ BME280 initialized successfully!\n"));
  delay(500);
}

// --------------------------------------
// Loop
// --------------------------------------
void loop() {
  getSensorValues();
  delay(delayTime);
}

// --------------------------------------
// Read and print sensor values
// --------------------------------------
void getSensorValues() {
  // Force one measurement (sensor is in sleep mode otherwise)
  bme.takeForcedMeasurement();
  delay(100);  // Wait a bit for measurement to complete

  // Read data
  float temperature = bme.readTemperature();
  float pressure    = bme.readPressure() / 100.0F;  // Convert Pa → hPa
  float altitude    = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humidity    = bme.readHumidity();

  // Print results
  Serial.println(F("---- Sensor Readings ----"));
  Serial.print(F("Temperature: ")); Serial.print(temperature); Serial.println(F(" °C"));
  Serial.print(F("Pressure:    ")); Serial.print(pressure);    Serial.println(F(" hPa"));
  Serial.print(F("Altitude:    ")); Serial.print(altitude);    Serial.println(F(" m"));
  Serial.print(F("Humidity:    ")); Serial.print(humidity);    Serial.println(F(" %"));
  Serial.println();
}
