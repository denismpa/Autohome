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
    Serial.println((String)"Door: "+doorstate);
  #endif
}

void ReadSmoke()
{
  smokestate = digitalRead(SMOKESENSORPIN);
  #ifdef DEBUG_TEMPERATURES
    Serial.println((String)"Smoke: "+smokestate);
  #endif
}

void ReadSensors()
{
  ReadDS18B20();
  ReadHTU21D();
  ReadDoor();
  ReadSmoke();
}
