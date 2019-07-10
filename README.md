# ESP32 Firmware

* Read sensors
* Send data to MQTT broker


# Led code

Led blinks mean that something goes wrong. The led can only blink in two
modes, fast or slow. This table translate led blinks to some error:

|Blinks|Error|
|---|---|
|2 FAST|Trying connection with Wifi|
|3 FAST|Trying connection with MQTT Broker|
|3 SLOW + 2 FAST|Error while reading temperature. One sensor may be damaged. Failed temperature reads are also noticeble by the value `-127` which is returned from failed reads. Upper tier can use this for accoutability so I left it.|
 

# Dependencies

* OneWire
* DallasTemperature
* PubSubClient

All dependencies are installable from the _Manage Libraries_ menu at Arduino IDE.
