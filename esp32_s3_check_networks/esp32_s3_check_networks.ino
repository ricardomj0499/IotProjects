#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
}


void loop() {
  check_for_networks();
  delay(10000);
}

void check_for_networks() {
  Serial.println("🔍 Escaneando redes WiFi...");
  int n = WiFi.scanNetworks();
  Serial.println("Hecho.");
  if (n == 0) {
    Serial.println("⚠️ No se encontraron redes WiFi.");
  } else {
    for (int i = 0; i < n; ++i) {
      Serial.printf("%d: %s (%d dBm)  Canal: %d  %s\n",
                    i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.RSSI(i),
                    WiFi.channel(i),
                    (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Abierta" : "Cerrada")
                    ;
                    
    }
  }
}