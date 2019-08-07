#include <U8x8lib.h>
//
// U8X8_SSD1306_128X32_UNIVISION_SW_I2C(clock, data [, reset])

double lastOled = millis(); 
U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(5,4);

void setupOled() {
  u8x8.begin();
  u8x8.setFont(u8x8_font_inb21_2x4_f);
  lastOled = millis();
}

void updateOled() {

  if(abs(millis() - lastOled) >= 500.) {       // every half second...
    
    u8x8.setCursor(4,32);
    u8x8.print(gInputTemp,2);
    lastOled = millis();
  }
}
