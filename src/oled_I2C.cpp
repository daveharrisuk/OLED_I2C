/* file: oled_I2C.cpp
*--------------------------------------------------------------------------  
*
* library for the SSD1306, SSD1309 or SH1106 chip OLED displays using I2C.
*
* This is a lightweight library, so there are no graphics and just a simple 
* monospaced 7x5 pixel font. 
* 
*  © Dave Harris, 2021 (Andover, UK) MERG M2740
*  
*  For Arduino AVR MEGA* processor
*  
* Version 
*  1.0: DH 14-Jul-21
*  
*/

#include "oled_I2C.h"



/*---------------------- OLED_I2C::_initSeq[] -----------------------------
 *
 * Initialization Sequence array
*/

const uint8_t OLED_I2C::_initSeq[] PROGMEM =
{
  DISPLAY_SLEEP,        /* Display OFF (sleep mode)                         */
  0x20, 0b00,           /* Memory Addressing Mode                           */
                        /* 00=Hor Address, 01=Vert Address, 10=Page Address */
  0xB0,                 /* Page Start Address for Page Addressing Mode, 0-7 */
  0xC8,                 /* COM Output Scan Direction                        */
  0x00,                 /* low column address                               */
  0x10,                 /* high column address                              */
  0x40,                 /* start line address                               */
  0x81, 0x3F,           /* contrast control register                        */
  0xA1,                 /* Segment Re-map. A0=adr mapped; A1=adr 127 mapped */
  0xA6,                 /* display mode. A6=Normal; A7=Inverse              */
  0xA8, OLED_PX_VERT-1, /* Set multiplex ratio(1 to 64)                     */
  0xA4,                 /* Output follows RAM content                       */
  0xD3, 0x00,           /* display offset. no offset                        */
  0xD5,                 /* display clock divide ratio/oscillator frequency  */
  0xF0,                 /* divide ratio                                     */
  0xD9, 0x22,           /* pre-charge period                                */
  0xDA,                 /* com pins hardware configuration                  */

#if OLED_PX_VERT == 64
  0x12,    
#elif OLED_PX_VERT == 32
  0x02,
#endif

  0xDB,                 /* set vcomh                                        */
  0x20,                 /* 0.77 x Vcc                                       */
  0x8D, 0x14,           /* Set DC-DC enable                                 */
  DISPLAY_AWAKE
};



/*------------------------- OLED_I2C::_report_if_I2C_error() -----------------
 *
 * print message if I2C errored
*/

void OLED_I2C::_report_if_I2C_error()
{
  if( I2C_ErrorFlag ) 
  {
    _serialRef->println("!I2C ");
    
    I2C_ErrorFlag = 0;
  }
}


/*------------------------------ OLED_I2C::_txCmd() --------------------------
 *
 * send command byte array to OLED on I2C
*/

void OLED_I2C::_txCmd( uint8_t cmd[], uint8_t siz ) 
{
  i2c_start( OLED_I2C_ADR << 1 );
  
  i2c_byte( DISPLAY_COMMAND );
  
  for( uint8_t byt = 0; byt < siz; byt++ ) 
  {
    i2c_byte( cmd[byt] );
  }
  i2c_stop();
  
  _report_if_I2C_error();
}



/*--------------------------- OLED_I2C::_txDat() ----------------------------
 *
 * send data byte array to OLED on I2C
*/

void OLED_I2C::_txDat( uint8_t dat[], uint16_t siz )
{
  i2c_start( OLED_I2C_ADR << 1 );
  
  i2c_byte( DISPLAY_DATA );
  
  for( uint16_t byt = 0; byt < siz; byt++ ) 
  {
    i2c_byte( dat[byt] );
  }
  i2c_stop();
  
  _report_if_I2C_error();
}



/*----------------------------- OLED_I2C::init() ---------------------------
 *
 * Init OLED and gets Serial obj addr for error printing
*/

void OLED_I2C::init( Stream * serialObj )
{
  _serialRef = serialObj;     /* print errors to Serial Monitor though this */
  
  i2c_init();

  _serialRef->println( buf );     /* buf[] has string of pixel sizes & chip */
  
  uint8_t cmdSeq[ sizeof(_initSeq) ];
  
  for( uint8_t i = 0; i < sizeof(_initSeq); i++ ) 
  {
    cmdSeq[i] = pgm_read_byte( & _initSeq[i] );
  }
  
  _txCmd( cmdSeq, sizeof(cmdSeq) );
  
  clearScreen(); /* also sets xPos/yPos to zero */

  putRAM( buf );
}



/*----------------------------- OLED_I2C::_cursor() -------------------------
 *
 * set _cursor position to chr#, line#    0, 0 is top left.
*/

void OLED_I2C::_cursor( int8_t xPos, int8_t yPos ) /* xPos & yPos default -1 */
{
  int8_t y = _yPos; /* get last pos */
  int8_t x = _xPos; /* get last pos */
  
  if( xPos >= 0 ) /* is xPos specified in call?  -1 if not */
  {
    x = xPos;  /* use new xPos */
  }
  
  if( yPos >= 0 ) /* is yPos specified in call?  -1 if not */
  {
    y = yPos;  /* use new yPos */ 
  }

  if( ( x <= CHARS_WIDE) && ( y <= CHARS_HIGH) ) /* inside screen area? */
  {
    _xPos = x;
    _yPos = y;
    uint8_t xPx = ( x * sizeof(FONT[0]) );
  
    uint8_t cmdSeq[] = 
    {
      (uint8_t) ( 0xb0 + y ),
      0x21,
  #if defined SSD1306 || defined SSD1309
      xPx,    
  #elif defined SH1106 
      0x00 + ( ( 2 + xPx ) & 0x0f ),
      0x10 + ( ( ( 2 + xPx ) & 0xf0 ) >> 4 ),     
  #endif  
      0x7f
    };
  
    _txCmd( cmdSeq, sizeof(cmdSeq) );
  }
}



/*------------------------ OLED_I2C::clearScreen() ----------------------------
 *
 * clear the display screen
*/

void OLED_I2C::clearScreen()
{
  uint8_t buf[ OLED_PX_HOR ];
  
  memset( buf, 0, sizeof(buf) );  /* fast fill buf[] with 0s */
  
  for( int8_t line = CHARS_HIGH -1; line >= 0 ; line-- )
  {
    _cursor( 0, line );
    _txDat( buf, sizeof(buf) );
  }
}



/*---------------------------- OLED_I2C::execute() --------------------------
 *
 * exeute display command... Normal/Inverse, Sleep/Awake
*/

void OLED_I2C::execute( DISPLAY_t cmdByte )
{
  uint8_t cmdSeq[1] = { cmdByte };

  _txCmd( cmdSeq, 1);
}



/*------------------------- OLED_I2C::contrast() ---------------------------
 *
 *
*/

void OLED_I2C::contrast( uint8_t contrast )
{
  uint8_t cmdSeq[2] = { DISPLAY_CONTRAST, contrast };
  _txCmd( cmdSeq, 2 );
}



/*----------------------------- OLED_I2C::_putChar() -------------------------
 *
 * put char on screen, if pos is on-screen & printable. Increments xPos.
*/

void OLED_I2C::_putChar( char chr )
{
  if( _xPos < CHARS_WIDE && chr >= ' ' )   /* is chr on-screen & printable? */
  {
    uint8_t indx = chr - ' ';              /* get index in FONT[]           */
  
    uint8_t dat[sizeof(FONT[0])];
  
  	for( uint8_t byt = 0; byt < sizeof(FONT[0]); byt++ ) /* get font to RAM */
    {
      dat[byt] = pgm_read_byte( & ( FONT[indx][byt] ) ); /* ...from PROGMEM */
    }
    _txDat( dat, sizeof(FONT[0]) );
    
    _xPos++;
  }
}



/*----------------------------- OLED_I2C::putRAM() ------------------------
 *
 * put RAM array of zero-terminated C style string on screen
 * if xPos/yPos not specified, they are -1 and default to last pos
*/

void OLED_I2C::putRAM( const char * ram_str, int8_t xPos, int8_t yPos )
{
  _cursor( xPos, yPos );
  
  while( * ram_str )  /* while not zero char */
  {
    _putChar( * ram_str ++ );
  }
}


/*------------------------------ OLED_I2C::putPROG() -----------------------
 *
 * put flash/PROGMEM array of zero-terminated C style string on screen
 *  if xPos/yPos not specified, they are -1 and default to last pos
*/

void OLED_I2C::putPROG( const char * prog_str, int8_t xPos, int8_t yPos )
{

  _cursor( xPos, yPos );
  
  char chr;
  while( ( chr = pgm_read_byte( prog_str ++ ) ) ) /* while not zero char */
  {
    _putChar( chr );
  }
}


/*----------------------------- eof oled_I2C.cpp ----------------------------*/
