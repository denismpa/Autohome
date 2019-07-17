const char *StaticStringifySensorAddress(const uint8_t *addr)
{
  static char space[SENSORS_ADDR_SIZ * 2 + 1] = {0};

  for (int i = 0; i < SENSORS_ADDR_SIZ; i++)
    snprintf(&space[i * 2], 3, "%02x", addr[i]);
  return space;
}

void PrintSensorsAddress(const uint8_t *addr)
{
  Serial.print(StaticStringifySensorAddress(addr));
}

void SensorsProbe()
{
  int i = 0;

  oneWire.reset_search();
  while (oneWire.search(sensors_addrs[i])) {
    if (OneWire::crc8(sensors_addrs[i], 7) != sensors_addrs[i][7]) {
        Serial.println("CRC error reading sensor one wire address");
        memset(sensors_addrs[i], 0, SENSORS_ADDR_SIZ);
        continue;
    }

    i++;
    if (i >= SENSORS_MAX)
      break;
  }
  sensors_found = i;
  Serial.print(i);
  Serial.println(" sensors found");

  for (i = 0; i < sensors_found; i++) {
    Serial.print("#");
    Serial.print(i);
    Serial.print(" sensor address: ");
    PrintSensorsAddress(sensors_addrs[i]);
    Serial.println();
  }
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
