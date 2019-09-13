#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// https://github.com/knolleary/pubsubclient/issues/425
// The below define is not working. I'm commenting it out
// so that if it starts to work magically (on updated version
// of the library) the behavior doens't change silently
// #define MQTT_SOCKET_TIMEOUT 1
#include <PubSubClient.h>
#include <Wire.h>
#include <SparkFunHTU21D.h>

// Defines
#define SSID "VersatusHPC_Monitoring"
#define WIFIPASSWORD "versatushpc"
#define MQTT_SERVER "192.168.4.1"
#define MQTT_PORT 1883
#define FAST 50
#define SLOW 200
#define SENSORS_ADDR_SIZ 8
#define SENSORS_MAX 8
//PINS
#define ONEWIREBUS 19
#define DOORSENSORPIN 23
#define SMOKESENSORPIN 18
#define RELAY1PIN 12
#define RELAY2PIN 13

//below lines enable or disable MQTT and WIFI
//#define USEMQTT
//#define USEWIFI

#define DEBUG_TEMPERATURES

// Globals
WiFiClient wificli;
PubSubClient mqttcli(wificli);
OneWire oneWire(ONEWIREBUS);
DallasTemperature tempSensor(&oneWire);
float temperatures[SENSORS_MAX];
bool doorstate = 0;
uint8_t sensors_addrs[SENSORS_MAX][SENSORS_ADDR_SIZ];
uint8_t sensors_found = 0;
HTU21D htu21d;
float htu21d_humidity = 0;
float htu21d_temp = 0;
bool smokestate = 0;

void setup() {
  pinMode(2, OUTPUT);
  pinMode(DOORSENSORPIN, INPUT_PULLUP);
  pinMode(SMOKESENSORPIN, INPUT_PULLUP);
  pinMode(RELAY1PIN, OUTPUT);
  pinMode(RELAY2PIN, OUTPUT);
  digitalWrite(RELAY1PIN, LOW);
  digitalWrite(RELAY2PIN, LOW);
  Serial.begin(115200);
  Serial.println("Hello ESP32 World!");
  tempSensor.begin();
  SensorsProbe();
//  Wire.begin(I2C_SDA, I2C_SCL);
  htu21d.begin();
}

void loop() {
  #ifdef USEWIFI
    ConnectToWiFi();
  #endif
  #ifdef USEMQTT
    ConnectMQTT();
    if (!mqttcli.connected())
      return;
  #endif
  ReadSensors();
  #ifdef USEMQTT
  for (int i = 0; i < sensors_found; i++) {
    static char topic[32 + SENSORS_ADDR_SIZ * 2] = {0};
    snprintf(topic, sizeof(topic), "sensors/temperature/%s", StaticStringifySensorAddress(sensors_addrs[i]));
    SendMessage(topic, temperatures[i]);
  }
  SendMessage("sensors/temperature/HTU21D", htu21d_temp);
  SendMessage("sensors/door/0", doorstate == HIGH);
  SendMessage("sensors/humidity/0", htu21d_humidity);
  SendMessage("sensors/smoke/0", smokestate == HIGH);
  #endif
  delay(1000);
}
