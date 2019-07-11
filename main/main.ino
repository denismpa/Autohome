#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
// https://github.com/knolleary/pubsubclient/issues/425
// The below define is not working. I'm commenting it out
// so that if it starts to work magically (on updated version
// of the library) the behavior doens't change silently
// #define MQTT_SOCKET_TIMEOUT 1
#include <PubSubClient.h>


// Defines
#define SSID "versatushpc"
#define WIFIPASSWORD "versatushpc.com.br/v1"
#define MQTT_SERVER "172.21.1.248"
#define MQTT_PORT 1883
#define ONEWIREBUS 22
#define DOORSENSORPIN 23
#define FAST 50
#define SLOW 200
#define SENSORS_ADDR_SIZ 8
#define SENSORS_MAX 8

#define DEBUG_TEMPERATURES

// Globals
WiFiClient wificli;
PubSubClient mqttcli(wificli);
OneWire oneWire(ONEWIREBUS);
DallasTemperature tempSensor(&oneWire);
float temperatures[SENSORS_MAX];
int doorstate = 1;
uint8_t sensors_addrs[SENSORS_MAX][SENSORS_ADDR_SIZ];
uint8_t sensors_found = 0;

void setup() {
  pinMode(2, OUTPUT);
  pinMode(DOORSENSORPIN, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("Hello ESP32 World!");
  tempSensor.begin();
  SensorsProbe();
}

void loop() {
  ConnectToWiFi();
  ConnectMQTT();
  if (!mqttcli.connected())
    return;
  ReadSensors();
  for (int i = 0; i < sensors_found; i++) {
    static char topic[32 + SENSORS_ADDR_SIZ * 2] = {0};
    snprintf(topic, sizeof(topic), "sensors/temperature/%s", StaticStringifySensorAddress(sensors_addrs[i]));
    SendMessage(topic, temperatures[i]);
  }
  SendMessage("sensors/door/0", doorstate == HIGH);
  SendMessage("sensors/umidity/0", random(50));
  SendMessage("sensors/smoke/0", random(2));
  delay(1000);
}
