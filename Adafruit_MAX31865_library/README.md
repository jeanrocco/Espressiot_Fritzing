# MODIFIED Adafruit_MAX31865
Arduino Library for Adafruit MAX31865 RTD Sensor

To make the library non-blocking, the "Adafruit_MAX31865::readRTD (void)" function will not be called because of the 10 and 65 msec. delays. It will be done externally using millis() function. In order to do that, "uint8_t  readRegister8(uint8_t addr);", "uint16_t readRegister16(uint8_t addr);" and "writeRegister8(uint8_t addr, uint8_t reg);" are made "public" in Adafruit_MAX31865.h . 
