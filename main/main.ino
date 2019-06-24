#include <WiFi.h>
#include <ArduinoJson.h>


// https://github.com/knolleary/pubsubclient/issues/425
// The below define is not working. I'm commenting it out
// so that if it starts to work magically (on updated version
// of the library) the behavior doens't change silently
// #define MQTT_SOCKET_TIMEOUT 1
#include <PubSubClient.h>


// Connection configuration. Edit accordingly
#define SSID "WIFI_SSID"
#define WIFIPASSWORD "WIFI_PASSWORD"
#define MQTT_SERVER "192.168.0.107"
#define MQTT_PORT 1883


// Globals :) Oh sweet embedded development ...
WiFiClient wificli;
PubSubClient mqttcli(wificli);

#define FAST 50
#define SLOW 200
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

void ReadSensors()
{

}

void SendMessage(const char* topic, int value)
{
  // JSON can be sent as below. I'm avoiding it
  // to save resources
  //StaticJsonDocument<300> json;
  //json["sensor"] = 1;
  //json["type"] = "temp";
  //json["value"] = random(100);
  //char JSONmessageBuffer[100];
  //serializeJson(json, JSONmessageBuffer);.

  static char charBuf[10];
  memset(charBuf, 0, sizeof(charBuf));
  itoa(value, charBuf, 10);
  if (mqttcli.publish(topic, charBuf) == false) {
    Serial.println("Error sending message");
  }
}

void setup() {
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  Serial.println("Hello ESP32 World!");
}

void loop() {
  ConnectToWiFi();
  ConnectMQTT();
  if (!mqttcli.connected())
    return;
  ReadSensors();
  SendMessage("sensors/temperature/cold_corridor/0", random(100));
  SendMessage("sensors/temperature/hot_corridor/0", random(100));
  SendMessage("sensors/umidity/0", random(50));
  SendMessage("sensors/door/0", random(2));
  SendMessage("sensors/smoke/0", random(2));
  delay(1000);
}
