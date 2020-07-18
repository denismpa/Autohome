void ReadDS18B20()
{
  for (int i = 0; i < sensors_found; i++) {
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
    PrintSensorsAddress(sensors_addrs[i]);
    Serial.print(" read ");
    Serial.println(temperatures[i]);
#endif
  }
}

void ReadHTU21D()
{
  htu21d_humidity = htu21d.readHumidity();
  htu21d_temp = htu21d.readTemperature();
#ifdef DEBUG_TEMPERATURES
  Serial.print("Temperature HT21D#");
  //PrintSensorsAddress(sensors_addrs[i]); - verificar se é aplicavel colher o endereco do sensor i2c
  Serial.print(" read ");
  Serial.println(htu21d_temp);
  Serial.print("Humidity HT21D#");
  //PrintSensorsAddress(sensors_addrs[i]); - verificar se é aplicavel colher o endereco do sensor i2c
  Serial.print(" read ");
  Serial.println(htu21d_humidity);
#endif
}

void ReadDoor()
{
  doorstate = digitalRead(DOORSENSORPIN);
#ifdef DEBUG_TEMPERATURES
  Serial.println((String)"Door: " + doorstate);
#endif
}

void ReadSmoke()
{
  smokestate = digitalRead(SMOKESENSORPIN);
#ifdef DEBUG_TEMPERATURES
  Serial.println((String)"Smoke: " + smokestate);
#endif
}


/*
void ReadTestRelay() {
  if (digitalRead(RELAY1CONTROL) == 1) {
#ifdef DEBUG_TEMPERATURES
    Serial.println("RELAY1CONTROL = HIGH, relay 1 should be OFF");
#endif
    relay_control(RELAY1PIN, 0, 0);
  }
  else if (digitalRead(RELAY1CONTROL) == 0) {
#ifdef DEBUG_TEMPERATURES
    Serial.println("RELAY1CONTROL = LOW, relay 1 should be ON");
#endif
    relay_control(RELAY1PIN, 1, 0);
  }
  if (digitalRead(RELAY2CONTROL) == 1) {
#ifdef DEBUG_TEMPERATURES
    Serial.println("RELAY2CONTROL = HIGH, relay 2 should be OFF");
#endif
    relay_control(RELAY2PIN, 0, 0);
  }
  else if (digitalRead(RELAY2CONTROL) == 0) {
#ifdef DEBUG_TEMPERATURES
    Serial.println("RELAY2CONTROL = LOW, relay 2 should be ON");
#endif
    relay_control(RELAY2PIN, 1, 0);
  }
}
*/
void ReadDHT22() {
  dht_humid = dht.readHumidity();
  dht_temp = dht.readTemperature();
 // Serial.println(dht_temp);
//  Serial.println(dht_humid);
#ifdef DEBUG_TEMPERATURES
  Serial.println((String)"DHT Temp: " + dht_temp);
  Serial.println((String)"DHT Humidity: " + dht_humid);
#endif
}
void ReadSensors()
{
  ReadDS18B20();
  ReadHTU21D();
  ReadDoor();
  ReadSmoke();
  ReadDHT22();
  //ReadTestRelay();
  ReadDHT22();
}
