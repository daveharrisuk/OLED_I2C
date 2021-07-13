Arduino library for the SSD1306, SSD1309 or SH1106 chip OLED display using I2C.

This is a lightweight library, so there are no graphics and just a simple monospaced 7x5 pixel font. 

```
// Example usage...
#INCLUDE "oled_I2C.h"

OLED_I2C  oled;  // instantiate class as object. Constructor inits oled.

  if( I2C_ErrorCode ) 
  {
    Serial.println("I2C error"); 
  }
  oled.clearScreen();  // 8 lines x 22 chars at default display pixel count
  oled.cursor( 0, 0 ); // position at top left of screen
  oled.put('0');       // cursor position is then 1, 0 (char pos 1 on line 0)
  oled.put("123456789012345678901x"); // x at end will NOT display as > 22
  oled.cursor( 3, 5 );  // char pos 3 on line 5
  oled.putP( PSTR("3,5 Hello from flash") );  // stored in flash, saves RAM
```

© Dave Harris, 2021 (Andover, UK) MERG M2740
