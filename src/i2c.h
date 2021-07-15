/* file: i2c.h
 *  
 *  i2c library
 *
 *  Created by Michael Köhler on 09.10.17.
 *
 *  Refactored by Dave Harris 11 July 2021
*/

#ifndef _i2c_h_
#define _i2c_h_

#ifdef __cplusplus
extern "C" {
#endif
	

#define F_I2C			100000UL  // clock I2c
#define PSC_I2C		1		      // prescaler I2c
#define SET_TWBR	( F_CPU / F_I2C - 16UL ) / (PSC_I2C * 2UL )

#include <stdio.h>
#include <avr/io.h>


extern uint8_t I2C_ErrorFlag;	/* is true on error. Caller must set false */


void    i2c_init();				              // init hw-i2c
void    i2c_start( uint8_t i2c_addr );	// send i2c_start_condition
void    i2c_stop();				              // send i2c_stop_condition
void    i2c_byte( uint8_t byt );		    // send data_byte
uint8_t i2c_readAck();                  // read byte with ACK
uint8_t i2c_readNAck();                 // read byte with NACK


#ifdef __cplusplus
}
#endif
	
#endif /* _i2c_h */
