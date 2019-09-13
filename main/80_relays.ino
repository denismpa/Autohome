
/*
 * index = relay index
 * operation 1 = onoff
 * operation2 = pulse
 * duration = pulse duration

*/

uint8_t relay_control(uint8_t index,  uint8_t operation, uint8_t duration ){

  if (operation != 1 && operation != 2){
    Serial.print("no such operation");
    return 1;
  } else if (operation == 1){
      onoff(index);
    } else{
      pulse(index, duration);
      }
}

uint8_t onoff(uint8_t index){
  if (index != 1  && index != 2){
    Serial.print("no such relay address");
    return 1;
  } else if (index == 1){
      digitalWrite(RELAY1PIN, !digitalRead(RELAY1PIN));
    } else{
      digitalWrite(RELAY2PIN, !digitalRead(RELAY2PIN));
      }
}

uint8_t pulse(uint8_t index, uint8_t duration){
  onoff(index);
  delay(duration);
  onoff(index);
  
}
