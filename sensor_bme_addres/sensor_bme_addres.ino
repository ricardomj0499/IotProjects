/*********
  Original Code by Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

/*********
This helps to find any to I2C devices connected to
the Arduino
By default Arduino uses
The ANALOG IN A4 pin for SDA 
And A5 for SCL.
You might connect more than one sensor to this pins 
to check wheter is is readed by the Arduino.FILE
of install libraries
*********/

#include <Wire.h>

int esp32_sda_pin=8;
int esp32_scl_pin=9;

void setup() {
  //Wire.begin();
  Wire.begin(esp32_sda_pin, esp32_scl_pin);
  Serial.begin(9600);
  Serial.println("\nI2C Scanner");
}
 
void loop() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}