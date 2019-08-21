# Espressiot_Fritzing

My implementation of Espressiot ( https://github.com/Schm1tz1/ESPressIoT ) ...

A good place to file my doc.

Added code for espressiot using a modified Adafruit MAX31865 RTD library and code for a second portable ESP8266 with Oled display.

I made the library non-blocking by changing the 10 msec. (bias enable) and a 65 msec. (conversion time) delays using millis().

I was having issues running the Oled display on the same Espressiot's ESP8266, too much delays I suppose. Lazily, after briefly looking for a non-blocking Oled driver, I decided to use a spare ESP8266 module whose sole purpose would be to get the temperature from an MQTT broker and display it on the Oled. This worked very well, and proved to be very flexible because it is portable.

Every seconds, the Espressiot-PID code "publishes" the RTD temperature to an MQTT broker, Mosquitto, which is running on a Raspberry PI (it was already running for Home Assistant...). Every 1/2 seconds, the Oled-display-ESP8266's code, which "subscribes" to the MQTT broker temperature, will update the Oled displays.




