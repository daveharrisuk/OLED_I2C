/* file oled_I2C.h
*---------------------------------------------------------------------------  
*
* Arduino library for the SSD1306, SSD1309 or SH1106 chip OLED display on I2C.
*
*
*         © Dave Harris, 2021 (Andover, UK) MERG M2740
*
*
* Comprises oled_I2C.h, oled_I2C.cpp, i2c.h, i2c.c, font_Monospaced7x5.h
* 
* 
* 
* Target: Arduino AVR MEGA processor
*  
*  
* This is a lightweight library, so there are no graphics and just a simple 
* monospaced 7x5 pixel font. Size was everything.
* 
*-------------------------------Example usage--------------------------------- 
*
* #include "oled_I2C.h"
* 
* OLED_I2C  oled;         // instantiate OLED_I2C class as oled object
* 
* void setup()   
* {
*  Serial.begin(9600);    // any I2C errors will show on Serial monitor
*  
*  oled.init( & Serial ); // init oled & pass address of Serial object
*                         // initial display shows chip and pixel sizes
*  oled.clearScreen();    // 8 lines x 21 chars at default display pixel size
*  
*  oled.putRAM("0123456789012345678901X", 0, 0); // last X wont display as >21
*                                                // 0,0 displays at top left
*  oled.putPROG( PSTR("Hello line7, char5"), 3, 5 ); // string is in progmem
* } 
* 
*---------------------------------- Version History--------------------------- 
* 
* 0.0  DH  7-Jul-21  started
* 1.0  DH 14-Jul-21  Tested on Arduino Nano
*  
* 
*------------------------------------- license --------------------------------
*
* This work is licensed under the Creative Commons 
*      Attribution-NonCommercial-ShareAlike 4.0 International License.
* To view a copy of this license,
*  visit http://creativecommons.org/licenses/by-nc-sa/4.0/ or
*  send a letter to Creative Commons, PO Box1866, Mountain View, CA 94042 USA.
*  
*------------------------------------------------------------------------------
*/
#ifndef _oled_I2C_H_
#define _oled_I2C_H_


#include <Arduino.h>



/* define the chip in the OLED display, SH1106 or SSD1306 or SSD1309 */

#define SSD1306

// define the OLED pixel sizes 128x32 or 128x64

#define OLED_PX_HOR  128
#define OLED_PX_VERT  64


/* check the defines are good */

#if defined SSD1306
  #define OLED_CHIP  "SSD1306"
#elif defined SSD1309
  #define OLED_CHIP  "SSD1309"
#elif defined SH1106
  #define OLED_CHIP  "SH1106"
#else
  #error define chip NOT SH1106 || SSD1306 || SSD1309
#endif
#if OLED_PX_HOR != 128
  #error define OLED_PX_HOR NOT 128
#endif
#if OLED_PX_VERT == 32
  #define OLED_SIZE "128x32 "
#elif OLED_PX_VERT == 64
  #define OLED_SIZE "128x64 "
#else
  #error define OLED_PX_VERT NOT 32 or 64
#endif         



/* The arduino Wire library has issues, so another library is used.          */
/* If the I2C is disconnected, the I2C.h library does NOT hang the program   */
/*    This library creates a global variable I2C_ErrorFlag                   */

#define OLED_I2C_ADR ( 0x78 >> 1 )    /* 7 bit slave-adress without r/w-bit  */

#include "i2c.h"	                    /* I2C communication library (basic)   */



/* save much program space by only using Monospaced 7x5 font                 */

#include "font_Monospaced7x5.h"



class OLED_I2C
{
  public:
  
    enum DISPLAY_t : uint8_t                      /* OLED execute commands   */
    {
      DISPLAY_DATA     = 0x40,
      DISPLAY_COMMAND  = 0x00,
      DISPLAY_NORMAL   = 0xA6,
      DISPLAY_INVERSE  = 0xA7,
      DISPLAY_SLEEP    = 0xAE,
      DISPLAY_AWAKE    = 0xAF,
      DISPLAY_CONTRAST = 0x81
    };

    static const int8_t CHARS_WIDE = OLED_PX_HOR / 6;   /* for Monospaced7x5 */
    
    static const int8_t CHARS_HIGH = OLED_PX_VERT / 8;  /* for Monospaced7x5 */

   
                              /* put ram string on screen at xPos, yPos      */
                              
    void putRAM( const char * ram_str, int8_t xPos = -1, int8_t yPos = -1 );

  
                              /* put progmem string on screen at xPos, yPos  */
                              
    void putPROG( const char * prog_str, int8_t xPos = -1, int8_t yPos = -1 ); 

      
    void clearScreen();                      /* clear the screen             */
      
    void execute( DISPLAY_t cmdByte );       /* execute invert/sleep command */
  
    void contrast( uint8_t contrast );       /* adjust display contrast      */

    void init( Stream * serialObj );         /* initialize display           */

  
    char buf[CHARS_WIDE]          /* general purpose display string buffer   */
    {                             /*   initially screen shows the chip/size  */
      OLED_SIZE  OLED_CHIP
    };
    
  
  private:

    Stream * _serialRef;                        /* Serial object ref         */
    
    void _report_if_I2C_error();                /* check error and print msg */

    void _cursor( int8_t xPos, int8_t yPos );   /* cursor to char# and line# */
  
    void _putChar( char chr );                  /* put character on screen   */
  
    void _txCmd( uint8_t cmd[], uint8_t siz );  /* transmit command sequence */ 
    void _txDat( uint8_t dat[], uint16_t siz ); /* transmit data sequence    */
  
    int8_t  _xPos = 0;                          /* track character position  */
    int8_t  _yPos = 0;                          /* track line number         */
      
    static const uint8_t _initSeq[];            /* init Sequence array       */

}; /* end of class OLED_I2C */


#endif /* _oled_I2C_H_ */
