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
    LedBlink(2, FAST); // delays for 1+ sec
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
        mqttcli.setCallback(Callback);
      if (!mqttcli.subscribe("relays/1")) {
        Serial.println("Subscribe error relays/1");
        mqttcli.disconnect();
      }
      if (!mqttcli.subscribe("relays/2")) {
        Serial.println("Subscribe error relays/2");
        mqttcli.disconnect();
      }
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
