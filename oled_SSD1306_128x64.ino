/* file: oled_SSD1306_128x64.ino
*------------------------------------------------------------
*
* example sketch for oled_I2C library
*
*
* © Dave Harris, 2021 (Andover, UK) MERG M2740
*/

#include "oled_I2C.h"


OLED_I2C  oled;  // instantiate oled object, constructor inits OLED.


void setup() 
{
  Serial.begin( 115200 );
  
  if( I2C_ErrorCode )
  {
    Serial.println("I2C error");
  }
  Serial.println( oled.buf ); // buf has the oled size

  delay(700);
  oled.clearScreen();
  oled.cursor( 0, 0 );
  
  oled.put('0');
  oled.put("!@#$%^&*_+-=(){}[]<>.");

  
  oled.cursor( 0, 1 );
  oled.put("123456789012345678901");

  oled.cursor( 0, 2 );
  oled.put('2');
  oled.putP( PSTR(" Hello") );  

  oled.cursor( 1, 3 );
  oled.put('3');
  oled.putP( PSTR(" Hi flash") ); 

  oled.cursor( 2, 4 );
  oled.putP( PSTR("4 Hello from flash") ); 

  oled.cursor( 3, 5 );
  oled.putP( PSTR("5 Hello from flash") ); 
  
  oled.execute( oled.DISPLAY_NORMAL );
  
  oled.cursor( 4, 6 ); 
  oled.putP( PSTR("6 Hello from flash") ); 

  oled.cursor( 5, 7 );
  oled.putP( PSTR("7 Hello from flash") );
}

void loop() 
{

}
