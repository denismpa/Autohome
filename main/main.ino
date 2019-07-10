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
#define SENSORS_MAX 2

#define DEBUG_TEMPERATURES

// Globals
WiFiClient wificli;
PubSubClient mqttcli(wificli);
OneWire oneWire(ONEWIREBUS);
DallasTemperature tempSensor(&oneWire);
float temperatures[SENSORS_MAX];
int doorstate = 1;
uint8_t sensors_addrs[SENSORS_MAX][SENSORS_ADDR_SIZ];

void PrintSensorsAddress(const uint8_t *addr)
{
  char tmp[2];
  for (int i = 0; i < SENSORS_ADDR_SIZ; i++) {
    sprintf(tmp, "%02x", addr[i]);
    Serial.print(tmp);
  }
}

void SaveSensorsAddrs()
{
  int i = 0, j = 0;

  oneWire.reset_search();
  while (oneWire.search(sensors_addrs[i])) {
    if (OneWire::crc8(sensors_addrs[i], 7) != sensors_addrs[i][7])
      goto err;

    if (++i >= SENSORS_MAX)
      break;
  }

  for (i = 0; i < SENSORS_MAX; i++) {
    Serial.print("#");
    Serial.print(i);
    Serial.print(" sensor address: ");
    PrintSensorsAddress(sensors_addrs[i]);
    Serial.println();
  }
  goto out;
err:
  Serial.println('CRC error reading sensor one wire address');
out:
  oneWire.reset_search();
}


// Blink status led n times. For debugging purposes
void LedBlink(unsigned int times, unsigned int speed)
{
  while (times-- > 0) {
    digitalWrite(2, HIGH);
    delay(speed);
    digitalWrite(2, LOW);
    delay(speed);
  }
  delay(1000);
}

void ConnectToWiFi()
{
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WIFIPASSWORD);
  Serial.print("Connecting to ");
  Serial.println(SSID);

  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    LedBlink(2, FAST);
    if ((++i % 16) == 0)
    {
      Serial.println(F(" still trying to connect"));
    }
  }

  Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());
}

void ConnectMQTT()
{
  uint8_t i = 0;

  if (mqttcli.connected())
    return;

  Serial.println("Connecting to MQTT broker " MQTT_SERVER " ...");
  mqttcli.setServer(MQTT_SERVER, MQTT_PORT);
  while (!mqttcli.connected())
  {
    // This take a long time to return if broker is offline
    bool connected = mqttcli.connect("ESP32Client");
    LedBlink(3, FAST);
    if (connected)
    {
      Serial.println("Connected to the broker " MQTT_SERVER "!");
    }
    else
    {
      Serial.print(".");
      if ((++i % 16) == 0)
      {
        Serial.print(F(" still trying to connect "));
        Serial.println(mqttcli.state());
      }
    }
  }
}

void SendMessage(const char* topic, int value)
{
  static char charBuf[10];
  memset(charBuf, 0, sizeof(charBuf));
  itoa(value, charBuf, 10);
  if (mqttcli.publish(topic, charBuf) == false) {
    Serial.println("Error sending message");
  }
}

void ReadTemperatures()
{
  for (int i = 0; i < SENSORS_MAX; i++) {
    if (!tempSensor.requestTemperaturesByAddress(sensors_addrs[i])) {
      Serial.print("Failed to read temperature for #");
      Serial.print(i);
      Serial.print(" sensor with address ");
      PrintSensorsAddress(sensors_addrs[i]);
      Serial.println();
      LedBlink(3, SLOW);
      LedBlink(2, FAST);
    }
    temperatures[i] = tempSensor.getTempC(sensors_addrs[i]);
#ifdef DEBUG_TEMPERATURES
    Serial.print("Temperature #");
    Serial.print(i);
    Serial.print(" read ");
    Serial.println(temperatures[i]);
#endif
  }
}

void ReadSensors()
{
  ReadTemperatures();
  doorstate = digitalRead(DOORSENSORPIN);
}

void setup() {
  pinMode(2, OUTPUT);
  pinMode(DOORSENSORPIN, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("Hello ESP32 World!");
  tempSensor.begin();
  SaveSensorsAddrs();
}

void loop() {
  ConnectToWiFi();
  ConnectMQTT();
  if (!mqttcli.connected())
    return;
  ReadSensors();
  for (int i = 0; i < SENSORS_MAX; i++) {
    static char topic[32] = {0};
    snprintf(topic, sizeof(topic), "sensors/temperature/%d", i);
    SendMessage(topic, temperatures[i]);
  }
  SendMessage("sensors/door/0", doorstate == HIGH);
  SendMessage("sensors/umidity/0", random(50));
  SendMessage("sensors/smoke/0", random(2));
  delay(1000);
}
