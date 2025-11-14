#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <HTTPClient.h>

// CONSTANTS
// bme280
#define BME280_ADDRESS 0x76

// diplay oled 128x64
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)

// Custom Data Tye
struct SensorData
{
    float temperature;
    float humidity;
    float pressure;
};

// I2C PINS AND SENSOR ADDRESSES
constexpr int SDA_PIN = 8;
constexpr int SCL_PIN = 9;

// WiFi credentials
const char *ssid = "test2g";
const char *password = "dohaeris";

// Your PC/server running VictoriaMetrics
const char *serverName = "http://192.168.100.163:8428/write";

// Objects
Adafruit_BME280 bme;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
SensorData data;

void setup()
{
    Serial.begin(9600);
    delay(1000);

    WiFi.disconnect(true); // desconecta
    WiFi.mode(WIFI_STA);   // modo cliente (station)
    delay(500);
    WiFi.setTxPower(WIFI_POWER_8_5dBm); // menor potencia = menos interferencia
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

    Wire.begin(SDA_PIN, SCL_PIN);

    bme.begin();
    // Initialize BME280 sensor
    if (!bme.begin(BME280_ADDRESS))
    {
        while (1)
            ; // Stop here if sensor not found
    }
    // Configure sensor sampling mode
    bme.setSampling(
        Adafruit_BME280::MODE_FORCED, // Forced mode = single measurement per trigger
        Adafruit_BME280::SAMPLING_X1, // Temperature oversampling
        Adafruit_BME280::SAMPLING_X1, // Pressure oversampling
        Adafruit_BME280::SAMPLING_X1, // Humidity oversampling
        Adafruit_BME280::FILTER_OFF   // No filtering
    );
    delay(500);

    display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
}

SensorData get_bme280_values()
{
    bme.takeForcedMeasurement();
    delay(100);
    SensorData data;
    data.temperature = bme.readTemperature();
    data.humidity = bme.readHumidity();
    data.pressure = bme.readPressure() / 100.0F; // Convert Pa → hPa
    return data;
}

void display_sensor_values(const SensorData &data)
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

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

    display.display();
}

void send_data_to_victoria_metrics(const SensorData &data)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.reconnect();
        delay(2000);
        if (WiFi.status() != WL_CONNECTED)
            return;
    }
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "text/plain");

    // ---- Influx line protocol format ----
    // measurement,tag=value field=value
    String payload = "";
    payload += "WeatherScore,sensor=bme280_1 temperature=" + String(data.temperature, 2) + "\n";
    payload += "WeatherScore,sensor=bme280_1 humidity=" + String(data.humidity, 2) + "\n";
    payload += "WeatherScore,sensor=bme280_1 pressure=" + String(data.pressure, 2);

    // ---- Send the data ----
    int httpResponseCode = http.POST(payload);
    http.end();
}

void loop()
{
    SensorData data = get_bme280_values();
    display_sensor_values(data);
    send_data_to_victoria_metrics(data);
    delay(5000);
}
