/* file: oled_I2C.cpp
 *--------------------------------------------------------------------------  
 *
 * library for the SSD1306, SSD1309 or SH1106 chip OLED displays using I2C.
 *
 *  © Dave Harris, 2021 (Andover, UK) MERG M2740
*/

#include "oled_I2C.h"



/*---------------------- OLED_I2C::initSeq[] -----------------------------
 *
 * Initialization Sequence array
*/

const uint8_t OLED_I2C::initSeq[] PROGMEM =
{
  DISPLAY_SLEEP,        // Display OFF (sleep mode)
  0x20, 0b00,           // Memory Addressing Mode
                        // 00=Hor Address, 01=Vert Address, 10=Page Address
  0xB0,                 // Page Start Address for Page Addressing Mode, 0-7
  0xC8,                 // COM Output Scan Direction
  0x00,                 // low column address
  0x10,                 // high column address
  0x40,                 // start line address
  0x81, 0x3F,           // contrast control register
  0xA1,                 // Segment Re-map. A0=addr mapped; A1=addr 127 mapped.
  0xA6,                 // display mode. A6=Normal; A7=Inverse
  0xA8, OLED_PX_VERT-1, // Set multiplex ratio(1 to 64)
  0xA4,                 // Output follows RAM content
  0xD3, 0x00,           // display offset. no offset
  0xD5,                 // display clock divide ratio/oscillator frequency
  0xF0,                 // divide ratio
  0xD9, 0x22,           // pre-charge period
  0xDA,                 // com pins hardware configuration
#if OLED_PX_VERT == 64
  0x12,    
#elif OLED_PX_VERT == 32
  0x02,
#else
#error define of OLED vertical pixels is not 32 or 64
#endif
  0xDB,                 // set vcomh
  0x20,                 // 0.77 x Vcc
  0x8D, 0x14,           // Set DC-DC enable 
};



/*--------------------------- OLED_I2C::txCmd() -------------------------------
 *
 * send command byte array to OLED on I2C
*/

void OLED_I2C::txCmd( uint8_t cmd[], uint8_t siz ) 
{
  i2c_start( ( OLED_I2C_ADR << 1 ) | 0 );
  
  i2c_byte( 0x00 );                       // send 0x00 for start command
  for( uint8_t byt = 0; byt < siz; byt++ ) 
  {
    i2c_byte( cmd[byt] );
  }
  i2c_stop();
}



/*--------------------------- OLED_I2C::txDat() -------------------------------
 *
 * send data byte array to OLED on I2C
*/

void OLED_I2C::txDat( uint8_t dat[], uint16_t siz ) 
{
    i2c_start( ( OLED_I2C_ADR << 1 ) | 0 );
    i2c_byte( 0x40 );                        // send 0x40 for start data
    for( uint16_t byt = 0; byt < siz; byt++ ) 
    {
      i2c_byte( dat[byt] );
    }
    i2c_stop();
}



/*------------------------ OLED_I2C constructor -----------------------------
 *
 * OLED_I2C constructor
*/

OLED_I2C::OLED_I2C()
{
  i2c_init();

  uint8_t cmdSeq[ sizeof( initSeq ) +1 ];
  
  for( uint8_t i = 0; i < sizeof( initSeq ); i++ ) 
  {
    cmdSeq[i] = ( pgm_read_byte( & initSeq[i] ) );
  }
  cmdSeq[ sizeof( initSeq ) ] = DISPLAY_AWAKE;
  
  txCmd( cmdSeq, sizeof( cmdSeq ) );
  
  clearScreen();
  
  put( buf );
}



/*-------------------------- OLED_I2C::cursor() -------------------------------
 *
 *
*/

void OLED_I2C::cursor( uint8_t chr, uint8_t lin )
{
  if( ( chr <= NUM_CHARS) && ( lin <= NUM_LINES ) ) // inside display area?
  {
    chrPos = chr;
    
    uint8_t xPx = chr * sizeof( FONT[0] );
  
    uint8_t cmdSeq[] = 
    {
      ((uint8_t) ( 0xb0 + lin )), 0x21,
      
  #if defined SSD1306 || defined SSD1309
  
      xPx,
      
  #elif defined SH1106
  
      0x00 + ( ( 2 + xPx ) & 0x0f ), 0x10 + ( ( ( 2 + xPx ) & 0xf0 ) >> 4 ),
      
  #else
   #error "chip define NOT SH1106 || SSD1306 || SSD1309"
  #endif
  
      0x7f
    };
  
    txCmd( cmdSeq, sizeof( cmdSeq ) );
  }
}



/*-------------------- OLED_I2C::clearScreen() --------------------------------
 *
 *
*/

void OLED_I2C::clearScreen()
{
  uint8_t buf[ OLED_PX_HOR ];
  
  memset( buf, 0x00, sizeof( buf ) );
  for( uint8_t line = 0; line < OLED_PX_VERT / 8; line++ )
  {
    cursor( 0, line );
    txDat( buf, sizeof( buf ) );
  }
  cursor( 0, 0 );
}



/*--------------------------- OLED_I2C::execute() -----------------------------
 *
 * exeute display command (Normal/Inverse, Sleep/Awake)
*/

void OLED_I2C::execute( DISPLAY_t cmdByte )
{
  uint8_t cmdSeq[1] = { cmdByte };

  txCmd( cmdSeq, 1);
}



/*---------------------------- OLED_I2C::contrast() --------------------------
 *
 *
*/

void OLED_I2C::contrast( uint8_t contrast )
{
  uint8_t cmdSeq[2] = { 0x81, contrast };
  txCmd( cmdSeq, sizeof( cmdSeq ) );
}



/*------------------------------ OLED_I2C::put() ------------------------------
 *
 * put char on screen
*/

void OLED_I2C::put( char chr )
{
  if( chrPos >= NUM_CHARS || chr < ' ' )
  {
    return;  // character does not fit line or is un-printable
  }
  
  chrPos++;
  
  chr = chr - ' ';      // make char index in FONT

  uint8_t dat[ sizeof( FONT[0] ) ];
    
	for( uint8_t byt = 0; byt < sizeof( FONT[0] ); byt++ )  // get font into ram
  {
    dat[byt] = ( pgm_read_byte( & ( FONT[ (uint8_t)chr] [byt] ) ) );
  }
  txDat( dat, sizeof( FONT[0] ) );
}



/*-------------------------------- OLED_I2C::putS() ---------------------------
 *
 * put RAM string on screen
*/

void OLED_I2C::put( const char * ram_str )
{
  while( * ram_str ) 
  {
      put( * ram_str ++ );
  }
}


/*-------------------------------- OLED_I2C::putP() ---------------------------
 *
 * put flash/PROGMEM string on screen
*/

void OLED_I2C::putP( const char * prog_str )
{
  uint8_t chr;
  while( ( chr = pgm_read_byte( prog_str ++ ) ) )
  {
    put( chr );
  }
}


/*----------------------------- eof oled_I2C.cpp ----------------------------*/
