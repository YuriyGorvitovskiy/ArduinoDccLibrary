/**
 ** This is Public Domain Software.
 ** 
 ** The author disclaims copyright to this source code.  
 ** In place of a legal notice, here is a blessing:
 **
 **    May you do good and not evil.
 **    May you find forgiveness for yourself and forgive others.
 **    May you share freely, never taking more than you give.
 **/

#ifndef __DCC_PROTOCOL_H__
#define __DCC_PROTOCOL_H__

#include <Arduino.h>
#include "DccPacket.h"

class DccProtocol {
private:
	uint8_t 	state;       
	boolean 	dcc_positive;
	
	DccPacket*	packet;
	uint8_t   	current_bit;
	uint8_t*  	current_byte;
	uint8_t*  	end_byte;

private:
	void 		configureTimer();
	void 		enableTimer();
	void 		disableTimer();
	
	 
public:
	void 		setup();
	
	void 		power(boolean on);
	boolean 	power();

	void 		timerInterrupt();
	void		startTest();
};

extern DccProtocol DccRails;

#endif