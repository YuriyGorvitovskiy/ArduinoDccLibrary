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

#ifndef __DCC_STATE_KEEPER_H__
#define __DCC_STATE_KEEPER_H__

#include <Arduino.h>
#include "DccPacket.h"
#include "DccCollection.h"


class DccStateKeeper {
private:
	byte 	state_count;	
	byte    generation;
	byte    nextState;
	
public:
	void setup();
	void resetSpeed();
	void resetAll();
	
	void saveState(DccPacket* packet);
	void readNextState(DccQueue& queue, DccStack& heap);

private:
	byte extractStateKind(DccPacket* p);
	void saveBroadcastState(byte stateKind, DccPacket* packet);
	void saveState(word eeprom, byte stateKind, DccPacket* packet);
	
	word findEeprom(DccPacket* packet);
	word appendAddress(DccPacket* p);
	void resetAddress(word eeprom, DccPacket* p);
	
	void updateAccess(word eeprom);

	void updateSpeed28 (word eeprom, DccPacket* p);
	void updateSpeed128(word eeprom, DccPacket* p);

	void updateF0_F4 (word eeprom, DccPacket* p);
	void updateF5_F8 (word eeprom, DccPacket* p);
	void updateF9_F12(word eeprom, DccPacket* p);
	
	void resetSpeed(word eeprom);
	void resetState(word eeprom);
};

extern DccStateKeeper DccState;

#endif //__DCC_STATE_KEEPER_H__