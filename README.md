Arduino library for the SSD1306, SSD1309 or SH1106 chip OLED display using I2C.

This is a lightweight library, so there are no graphics and just a simple monospaced 7x5 pixel font. 

```
#include "oled_I2C.h"
 
OLED_I2C  oled;         // instantiate OLED_I2C class as oled object

void setup()   
{
  Serial.begin(9600);    // any I2C errors will show on Serial monitor
 
  oled.init( & Serial ); // init oled & pass address of Serial object
                         // initial display shows chip and pixel sizes
  oled.clearScreen();    // 8 lines x 21 chars at default display pixel size
 
  oled.putRAM("0123456789012345678901X", 0, 0); // last X wont display as >21
                                                // 0,0 displays at top left
  oled.putPROG( PSTR("Hello line7, char5"), 3, 5 ); // string is in progmem
} 
```

© Dave Harris, 2021 (Andover, UK) MERG M2740
