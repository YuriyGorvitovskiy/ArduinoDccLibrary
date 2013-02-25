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

#ifndef __DCC_COMANDER_H__
#define __DCC_COMANDER_H__

#include <Arduino.h>
#include "DccPacket.h"
#include "DccCollection.h"

class DccCommander {
private:
	DccStack	recycle;
	DccQueue 	queue;

public:
	DccCommander();

	void setup();
	void loop();

	DccPacket* 	nextPacketToSend(DccPacket* sent);
	void        returnBack(DccPacket* unprocessd);
	
public:
	static const char* ACKNOWLEDGE;
	static const char* QUEUED;
	static const char* ERROR;
	static const char* UNKNOWN;

	DccPacket*  newPacket();
	void 		send(DccPacket*);
	
	// P0  - power off
	// P1  - power on
	// RQ  - reset Queue
	// RSA - reset All States
	// RSS - reset Speed State
	// HXX...XX - DCC Hex Command
	// mXX...XX - DCC Text Command. See DccPacket::parseDccTextCommand(..) function description.
	// MXX...XX - DCC Text Command. See DccPacket::parseDccTextCommand(..) function description.
	// BXX...XX - DCC Text Command. See DccPacket::parseDccTextCommand(..) function description.
	// EXX...XX - DCC Text Command. See DccPacket::parseDccTextCommand(..) function description.
	const char*  handleTextCommand(const char* command);
	
	boolean power();
	void 	power(boolean on);

	void	resetAll();
	void	resetQueue();
	void	resetSpeedStates();
};

extern DccCommander DccCmd;

inline DccPacket* DccCommander::newPacket() {
	return recycle.pop();
}

#endif // __DCC_COMANDER_H__
