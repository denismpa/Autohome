void ReadTemperatures()
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

void ReadSensors()
{
  ReadTemperatures();
  doorstate = digitalRead(DOORSENSORPIN);
}

