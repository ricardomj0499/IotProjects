/******************************************************
 *  Universal I2C Scanner
 *  Compatible with Arduino UNO/Nano and ESP32-S3
 *
 *  Original code by Rui Santos
 *  Modified & extended version with board selection
 *
 *  Details:
 *   - Detects all I2C devices connected to the SDA/SCL bus.
 *   - Prints found addresses in HEX.
 *
 *  Select your board below by defining one of:
 *    #define USE_ARDUINO
 *    #define USE_ESP32
 ******************************************************/

#include <Wire.h>

// ======== BOARD SELECTION ========
// Uncomment only one of these:
// #define USE_ARDUINO
#define USE_ESP32
// =================================

// --- I2C pin configuration ---
#if defined(USE_ARDUINO)
// Arduino UNO defaults
const int SDA_PIN = A4;
const int SCL_PIN = A5;
#elif defined(USE_ESP32)
// ESP32-S3 example pins
const int SDA_PIN = 8;
const int SCL_PIN = 9;
#else
#error "Please define either USE_ARDUINO or USE_ESP32 at the top of the code."
#endif

void setup()
{
  Serial.begin(9600);
  Serial.println("\n===== I2C Scanner =====");

  // Initialize I2C bus depending on board type
#if defined(USE_ARDUINO)
  Wire.begin(); // Arduino uses default SDA/SCL pins
#elif defined(USE_ESP32)
  Wire.begin(SDA_PIN, SCL_PIN);
#endif

  Serial.print("Using SDA pin: ");
  Serial.println(SDA_PIN);
  Serial.print("Using SCL pin: ");
  Serial.println(SCL_PIN);
  Serial.println();
}

void loop()
{
  byte error, address;
  int nDevices = 0;

  Serial.println("Scanning for I2C devices...");

  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("Device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0)
  {
    Serial.println("❌ No I2C devices found.\n");
  }
  else
  {
    Serial.println("✅ Scan complete!\n");
  }

  delay(5000); // Wait 5 seconds before rescanning
}
