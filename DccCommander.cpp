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

#include <Arduino.h>
#include "DccConfig.h"
#include "DccCommander.h"
#include "DccProtocol.h"
#include "DccStateKeeper.h"

DccCommander DccCmd;

DccPacket	 IDLE;

DccPacket	 heap[DCC_QUEUE_MAX_COUNT];

const char* DccCommander::ACKNOWLEDGE 	= "Acknowledge";
const char* DccCommander::QUEUED 		= "Queued";
const char* DccCommander::ERROR     	= "ERROR";
const char* DccCommander::UNKNOWN     	= "UNKNOWN";

DccCommander::DccCommander() 
	:	recycle(heap, DCC_QUEUE_MAX_COUNT) {
	IDLE.idle();
}

void DccCommander::begin() {
	DccRails.begin();
	DccState.begin();
	power(true);
}

void DccCommander::loop() {
	if (!queue.isEmpty())
		return;
		
	DccState.readNextState(queue, recycle);
}

// P0  - power off
// P1  - power on
// RA  - reset All
// RQ  - reset Queue
// RS  - reset Speed State
// HXX...XX - DCC Hex Command
// mXX...XX - DCC Text Command
// MXX...XX - DCC Text Command
// BXX...XX - DCC Text Command
// EXX...XX - DCC Text Command
const char* DccCommander::handleTextCommand(const char* command) {
	switch(*command) {
		case 'P': power(DccPacket::parseBoolean(*(command + 1))); return ACKNOWLEDGE; 
		case 'R': switch(*(command+1)) {
					case 'A': resetAll(); return ACKNOWLEDGE;
					case 'Q': resetQueue(); return ACKNOWLEDGE;
					case 'S': resetSpeedStates(); return ACKNOWLEDGE;
				};
				break;
		case 'H': {
				DccPacket* packet = newPacket();
				if (packet == NULL)
					return ERROR;
					
				packet = packet->parseDccHexCommand(command + 1);
				if (packet == NULL)
					return UNKNOWN;
					
				send(packet);
				return QUEUED;
				};
		case 'm':				
		case 'M':				
		case 'B':				
		case 'E': {
				DccPacket* packet = newPacket();
				if (packet == NULL)
					return ERROR;
					
				packet = packet->parseDccTextCommand(command);
				if (packet == NULL)
					return UNKNOWN;
					
				send(packet);
				return QUEUED;
				};
	}
	return UNKNOWN;
}

DccPacket* DccCommander::nextPacketToSend(DccPacket* sent) {
	if (sent != NULL && sent != &IDLE) {
	 	if (sent->decrementRepeat())
			return sent;
			
		recycle.push(sent);
	}

	if (!queue.isEmpty()) {
		return queue.next();
	}
	
	return &IDLE;
}

void DccCommander::returnBack(DccPacket* unprocessed) {
	if (unprocessed != &IDLE)
		queue.push(unprocessed);
}

void DccCommander::send(DccPacket* packet) {
	DccState.saveState(packet);
	queue.add(packet);
}

boolean DccCommander::power() {
	return DccRails.power();
}

void DccCommander::power(boolean on) {
	DccRails.power(on);
}

void DccCommander::resetAll() {
	power(false);
	resetQueue();
	DccState.resetAll();
	power(true);
}

void DccCommander::resetQueue() {
	while(!queue.isEmpty())
		recycle.push(queue.next());
}

void DccCommander::resetSpeedStates() {
	DccState.resetSpeed();
}




