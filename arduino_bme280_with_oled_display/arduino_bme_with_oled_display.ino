
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1012)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_BME280 bme;
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

struct SensorData
{
  float temperature;
  float humidity;
  float pressure;
  float altitude;
};

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  if (!bme.begin(0x76))
  { // o 0x77, según tu módulo
    Serial.println("❌ No se detecta el BME280!");
    while (1)
      ;
  }
  // Configure sensor sampling mode
  bme.setSampling(
      Adafruit_BME280::MODE_FORCED, // Forced mode = single measurement per trigger
      Adafruit_BME280::SAMPLING_X1, // Temperature oversampling
      Adafruit_BME280::SAMPLING_X1, // Pressure oversampling
      Adafruit_BME280::SAMPLING_X1, // Humidity oversampling
      Adafruit_BME280::FILTER_OFF   // No filtering
  );
}

void loop()
{
  SensorData data = getValues();
  display_sensor_values(data);
  delay(5000);
}

SensorData getValues()
{
  bme.takeForcedMeasurement();
  delay(100);
  SensorData data;
  data.temperature = bme.readTemperature();
  data.humidity = bme.readHumidity();
  data.pressure = bme.readPressure() / 100.0F; // Convert Pa → hPa
  data.altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  return data;
}

void display_sensor_values(SensorData data)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  display.clearDisplay();

  // Temperature (big)
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(data.temperature, 1); // one decimal
  display.setTextSize(1);
  display.cp437(true);
  display.write(167); // degree symbol
  display.print("C");

  // Humidity (big)
  display.setTextSize(1);
  display.setCursor(0, 32);
  display.print("Hum: ");
  display.setTextSize(2);
  display.setCursor(0, 42);
  display.print(data.humidity, 1);
  display.setTextSize(1);
  display.print(" %");

  // Pressure (small)
  display.setTextSize(1);
  display.setCursor(72, 0);
  display.print("P:");
  display.setCursor(90, 0);
  display.print(data.pressure, 0); // no decimals
  display.print("hPa");

  // Altitude (small)
  display.setCursor(72, 10);
  display.print("Alt:");
  display.setCursor(95, 10);
  display.print(data.altitude, 0);
  display.print("m");

  display.display();
}