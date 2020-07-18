static uint8_t relays[] = { RELAY1PIN, RELAY2PIN, RELAY3PIN, RELAY4PIN };

void Callback(char *topic, byte *message, unsigned int length)
{
  uint8_t command = message[0] - '0'; // convert '1' to 1
  Serial.print("Message received topic => ");
  Serial.print(topic);
  Serial.print(", message => ");
  for (uint8_t i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println(""); 

  if (!strncmp(topic, "relays/", 7)) {
    uint8_t relay = topic[7] - '0';
    Serial.print("relay => ");
    Serial.println(relay);
    relay_control(relays[relay-1], command, 5000);
  }
}
