void SendMessage(const char* topic, int value)
{
  static char charBuf[10];
  memset(charBuf, 0, sizeof(charBuf));
  itoa(value, charBuf, 10);
  if (mqttcli.publish(topic, charBuf) == false) {
    Serial.println("Error sending message");
  }
}

