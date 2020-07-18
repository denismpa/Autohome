
/*
   index = value of RELAY1PIN for relay1 | value of RELAY2PIN for relay2
   operation = on = 1 | off = 0 | pulse = 2
   duration = pulse duration

*/

uint8_t relay_control(uint8_t index,  uint8_t operation, uint8_t duration ) {
  if (index != RELAY1PIN  && index != RELAY2PIN  && index != RELAY3PIN && index != RELAY4PIN) {
    Serial.print("no such relay address");
    return 1;
  }
  switch (operation) {
    case 0:
      #ifdef DEBUG_TEMPERATURES
      Serial.println((String)"setting relay " + index + "to OFF with HIGH output");
     #endif
      digitalWrite(index, HIGH);
      break;
    case 1:
      #ifdef DEBUG_TEMPERATURES
      Serial.println((String)"setting relay " + index + "to ON with LOW output");
      #endif
      digitalWrite(index, LOW);
      break;
    case 2:
      #ifdef DEBUG_TEMPERATURES
      Serial.println((String)"toggling relay " + index);
      #endif
      digitalWrite(index, !digitalRead(index));
      delay(duration);
      digitalWrite(index, !digitalRead(index));
      break;
    default:
      Serial.println("ERROR: No such Operation for Relays. Use 1 for on, 0 for off or 2 for pulse");
      return 1;
      break;
  }
}
