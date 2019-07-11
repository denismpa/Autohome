void publish(const char *topic, const char *msg)
{
  if (mqttcli.publish(topic, msg) == false)
    Serial.println("Error sending message");
}

void SendMessage(const char* topic, long int value)
{
  static char buf[10] = {0};
  itoa(value, buf, 10);
  publish(topic, buf);
}

void SendMessage(const char* topic, double value)
{
  static char buf[10] = {0};
  dtostrf(value, 6, 2, buf);
  publish(topic, buf);
}

void SendMessage(const char *topic, bool value)
{
  if (value)
    publish(topic, "1");
  else
    publish(topic, "0");
}

