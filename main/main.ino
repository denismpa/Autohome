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
#include <Adafruit_Sensor.h>
#include <DHT.h>

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
#define ONEWIREBUS 15
#define DOORSENSORPIN 23
#define SMOKESENSORPIN 25
#define RELAY1PIN 33
#define RELAY2PIN 32
#define RELAY1CONTROL 19
#define RELAY2CONTROL 21
#define DHTPIN 26
#define DHTTYPE DHT22

//below lines enable or disable MQTT and WIFI
#define USEMQTT
#define USEWIFI

#define DEBUG_TEMPERATURES

DHT dht(DHTPIN, DHTTYPE);

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
float dht_temp = 0;
float dht_humid = 0;


void setup() {
  pinMode(2, OUTPUT);
  pinMode(DOORSENSORPIN, INPUT_PULLUP);
  pinMode(SMOKESENSORPIN, INPUT_PULLUP);
  pinMode(RELAY1PIN, OUTPUT);
  pinMode(RELAY2PIN, OUTPUT);
  digitalWrite(RELAY1PIN, HIGH);
  digitalWrite(RELAY2PIN, HIGH);
  pinMode(RELAY1CONTROL, INPUT_PULLUP);
  pinMode(RELAY2CONTROL, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("Hello ESP32 World!");
  tempSensor.begin();
  SensorsProbe();
  //  Wire.begin(I2C_SDA, I2C_SCL);
  htu21d.begin();
  dht.begin();
}

void loop() {
#ifdef USEWIFI
  ConnectToWiFi();
#endif
#ifdef USEMQTT
  ConnectMQTT();
  if (!mqttcli.connected())
    return;
  else
    mqttcli.loop();
#endif
  ReadSensors();

#ifdef USEMQTT
  for (int i = 0; i < sensors_found; i++) {
    static char topic[32 + SENSORS_ADDR_SIZ * 2] = {0};
    snprintf(topic, sizeof(topic), "sensors/temperature/%s", StaticStringifySensorAddress(sensors_addrs[i]));
    SendMessage(topic, temperatures[i]);
  }
  if (htu21d_temp != ERROR_I2C_TIMEOUT && htu21d_temp != ERROR_BAD_CRC)
    SendMessage("sensors/temperature/HTU21D", htu21d_temp);
  if (htu21d_humidity != ERROR_I2C_TIMEOUT && htu21d_humidity != ERROR_BAD_CRC)    
    SendMessage("sensors/humidity/0", htu21d_humidity);
  if (!isnan(dht_humid))
    SendMessage("sensors/humidity/1", dht_humid);
  SendMessage("sensors/door/0", doorstate == HIGH);
  SendMessage("sensors/smoke/0", smokestate == HIGH);
  SendMessage("relays/1/status", (long int)GPIO_REG_READ(GPIO_OUT1_REG) & 0x2); // RELAY1PIN
  SendMessage("relays/2/status", (long int)GPIO_REG_READ(GPIO_OUT1_REG) & 0x1); // RELAY2PIN
#endif
  delay(1000);
#ifdef DEBUG_TEMPERATURES
  Serial.println("\n ########################## \n");
#endif
}
