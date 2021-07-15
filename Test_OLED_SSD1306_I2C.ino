/* file: Test_OLED_SSD1306_I2C.ino
*------------------------------------------------------------
*
* example sketch for oled_I2C library. default SSD1306 & 128x64 defines.
*
*
* © Dave Harris, 2021 (Andover, UK) MERG M2740
*/

#include <oled_I2C.h>


OLED_I2C  oled;  /* instantiate oled object */


void setup() 
{
  Serial.begin( 115200 );
  Serial.println("\nOLED ino");
  
  oled.init( & Serial );  /* init oled and pass it the Serial object */
  
  delay(700); /* delay NOT required. Allows init screen message to be seen */
  
  oled.clearScreen();
  
  oled.putRAM( "0!@#$%^&*_+-=(){}[]<>.", 0, 0 );

  oled.putRAM( "123456789012345678901",0, 1 );

  oled.putPROG( PSTR("2 Hello"), 0, 2 );  

  oled.putPROG( PSTR("3 Hi flash"), 1, 3 ); 

  oled.putPROG( PSTR("4 Hello from flash"), 2, 4 ); 

  oled.putPROG( PSTR("5 Hello from flash"), 3, 5 ); 
  
   
  oled.putPROG( PSTR("6 Hello from flash"), 4, 6 ); 

  oled.putPROG( PSTR("7 Hello from flash"), 5, 7 );
  
  delay(700);
  oled.clearScreen();
  
  oled.contrast( 200 );
  
  Serial.println("loop");
}

void loop() 
{
  for(uint8_t i= 0; i < 12; i++)
  {
    sprintf( oled.buf, "  cell %2d  ", i );
    oled.putRAM( oled.buf, ( i % 2 == 0 ? 0 : 11 ), ( i >> 1 ) + 2 );
  }
  delay(1000);
}
