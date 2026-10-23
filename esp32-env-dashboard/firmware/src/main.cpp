#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <DHT.h>

// ===================== CONFIGURAZIONE NODO =====================
const char* NODE_NAME = "living-room";   // nome identificativo del nodo
const char* SSID      = "TUO_WIFI";
const char* PASSWORD  = "TUO_PASSWORD";

// Pin sensori
#define BME280_ADDR  0x76
#define DHT_PIN      4
#define MQ135_PIN    34
#define LDR_PIN      35

// Abilita/disabilita sensori su questo nodo
#define HAS_BME280  true   // BME280 (I2C: temp + umidità + pressione)
#define HAS_DHT     false  // DHT22/DHT11 (temp + umidità)
#define HAS_MQ135   true   // MQ-135 (qualità aria)
#define HAS_LDR     true   // LDR (luminosità)

#define SENSOR_INTERVAL 5000  // ms tra una lettura e l'altra

// ===================== GLOBALI =====================
WebServer server(80);

#if HAS_BME280
Adafruit_BME280 bme;
bool bmeOk = false;
#endif
#if HAS_DHT
DHT dht(DHT_PIN, DHT_TYPE);
#endif

float temperature = NAN, humidity = NAN, pressure = NAN;
float airQuality  = NAN;
int   lightLevel  = 0;
uint32_t lastRead = 0;

// ===================== LETTURA SENSORI =====================
void readSensors() {
#if HAS_BME280
  if (bmeOk) {
    bme.takeForcedMeasurement();
    temperature = bme.readTemperature();
    pressure    = bme.readPressure() / 100.0F;
    humidity    = bme.readHumidity();
  }
#endif

#if HAS_DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(t)) temperature = t;
  if (!isnan(h)) humidity    = h;
#endif

#if HAS_MQ135
  airQuality = (float)analogRead(MQ135_PIN);
#endif

#if HAS_LDR
  int raw = analogRead(LDR_PIN);
  lightLevel = constrain(map(raw, 0, 4095, 100, 0), 0, 100);
#endif
}

// ===================== HTTP =====================
void handleRoot() {
  server.send(200, "text/plain", "ESP32 Environmental Node: " + String(NODE_NAME));
}

void handleAPI() {
  JsonDocument doc;
  doc["node"]        = NODE_NAME;
  doc["temperature"] = isnan(temperature) ? nullptr : temperature;
  doc["humidity"]    = isnan(humidity)    ? nullptr : humidity;
  doc["pressure"]    = isnan(pressure)    ? nullptr : pressure;
  doc["airQuality"]  = isnan(airQuality)  ? nullptr : airQuality;
  doc["lightLevel"]  = lightLevel;
  doc["uptime"]      = millis() / 1000;

  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  Serial.printf("\n[%s] Avvio\n", NODE_NAME);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nIP: %s\n", WiFi.localIP().toString().c_str());

#if HAS_BME280
  Wire.begin();
  bmeOk = bme.begin(BME280_ADDR);
  Serial.println(bmeOk ? "BME280 OK" : "BME280 NON TROVATO");
  if (bmeOk) {
    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1,
                    Adafruit_BME280::SAMPLING_X1,
                    Adafruit_BME280::SAMPLING_X1,
                    Adafruit_BME280::FILTER_OFF);
  }
#endif

#if HAS_DHT
  dht.begin();
#endif

#if HAS_MQ135 || HAS_LDR
  analogReadResolution(12);
#endif

  server.on("/", handleRoot);
  server.on("/api/sensors", handleAPI);
  server.begin();

  readSensors();
  lastRead = millis();
}

// ===================== LOOP =====================
void loop() {
  server.handleClient();

  if (millis() - lastRead >= SENSOR_INTERVAL) {
    readSensors();
    lastRead = millis();
    Serial.printf("T=%.1fC  H=%.1f%%  P=%.0fhPa  AQ=%.0f  L=%d%%\n",
                  isnan(temperature) ? 0 : temperature,
                  isnan(humidity)    ? 0 : humidity,
                  isnan(pressure)    ? 0 : pressure,
                  isnan(airQuality)  ? 0 : airQuality,
                  lightLevel);
  }
}
