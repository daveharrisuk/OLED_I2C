/* file oled_I2C.h
*---------------------------------------------------------------------------  
*
* library for the SSD1306, SSD1309 or SH1106 chip OLED display using I2C.
*
* This is a lightweight library, so there are no graphics and just a simple 
* monospaced 7x5 pixel font. 
* 
*  // Example usage...
*  OLED_I2C  oled;  // instantiate class as object. Constructor inits oled.
*  
*  if( I2C_ErrorCode ) { Serial.println("I2C error"); }
*  oled.clearScreen();  // 8 lines x 22 chars at default display pixel count
*  oled.cursor( 0, 0 ); // position at top left of screen
*  oled.put('0');       // cursor position is then 1, 0 (char pos 1 on line 0)
*  oled.put("123456789012345678901x"); // x at end will NOT display (>22)
*  oled.cursor( 3, 5 );  // char pos 3 on line 5
*  oled.putP( PSTR("3,5 Hello from flash") );  // stored in flash, saves RAM
*
* 
* 
* © Dave Harris, 2021 (Andover, UK) MERG M2740
* 
* This work is licensed under the Creative Commons 
*      Attribution-NonCommercial-ShareAlike 4.0 International License
* To view a copy of this license,
*  visit http://creativecommons.org/licenses/by-nc-sa/4.0/ or
*  send a letter to Creative Commons, PO Box1866, Mountain View, CA 94042 USA.
*/
#ifndef _oled_I2C_H_
#define _oled_I2C_H_

#include <Arduino.h>


// define the chip in the OLED display, SH1106 or SSD1306 or SSD1309

#define SSD1306

// define the OLED pixel sizes

#define OLED_PX_HOR  128
#define OLED_PX_VERT  64


/* The arduino Wire library has issues, so 3rd party library is used */
 
#define OLED_I2C_ADR  ( 0x78 >> 1 )     // 7 bit slave-adress without r/w-bit

#include "i2c.h"	                      // I2C communication


/* save much flash space using this font */

#include "font_Monospaced7x5.h"



class OLED_I2C
{
  public:
  
    enum DISPLAY_t : uint8_t                   // OLED execute commands
    {
      DISPLAY_NORMAL  = 0xA6,
      DISPLAY_INVERSE = 0xA7,
      DISPLAY_SLEEP   = 0xAE,
      DISPLAY_AWAKE   = 0xAF
    };
    
    static const uint8_t NUM_CHARS = OLED_PX_HOR / 6;   // for Monospaced7x5
    
    static const uint8_t NUM_LINES = OLED_PX_VERT / 8;  // for Monospaced7x5
    
    void cursor( uint8_t chr, uint8_t lin ); // set cursor, char# and line#
  
    void put( char chr );                    // put character on screen
  
    void put( const char * ram_str );        // put ram string on screen
  
    void putP( const char * prog_str );      // put progmem string on screen
   
    void clearScreen();                      // clear the screen
  
    void execute( DISPLAY_t cmdByte );       // execute invert/sleep commands
  
    void contrast( uint8_t contrast );       // adjust display contrast 

    OLED_I2C();                              // Constructor inits display
  
    char buf[NUM_CHARS]           // general purpose display string buffer
    {                             // initially it shows the display size.
      #if OLED_PX_VERT == 32
       "128x32"
      #elif OLED_PX_VERT == 64
       "128x64"
      #else
        #error define OLED_PX_VERT NOT 32 || 64
      #endif
    };
    
    #if OLED_PX_HOR != 128
     #error define OLED_PX_HOR NOT 128
    #endif    
  
  private:
    
    void txCmd( uint8_t cmd[], uint8_t siz );  // transmit command sequence
  
    void txDat( uint8_t dat[], uint16_t siz ); // transmit data sequence
  
    uint8_t chrPos = 0;                        // track character position
  
    static const uint8_t initSeq[];            // initialization Sequence array
 
}; /* end of class OLED_I2C */


#endif /* _oled_I2C_H_ */
