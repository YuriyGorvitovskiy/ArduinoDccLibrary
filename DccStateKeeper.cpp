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
#include <EEPROM.h>

#include "DccConfig.h"
#include "DccStateKeeper.h"


#define DCC_EEPROM_ADDR_COUNT 			(DCC_STATE_EEPROM_ADDR)
#define DCC_EEPROM_ADDR_GENERATION 		(DCC_STATE_EEPROM_ADDR + 1)

#define DCC_EEPROM_ADDR_STATE_0	   		(DCC_STATE_EEPROM_ADDR + 2)
#define DCC_EEPROM_STATE_SIZE	   		(6)

//First DccAddress byte
#define DCC_EEPROM_STATE_ADDRESS_0		(0)

//Second DccAddress byte if present
#define DCC_EEPROM_STATE_ADDRESS_1		(1)

//Generation number when state was changed
#define DCC_EEPROM_STATE_ACCESSED		(2)

//Always active
//Speed format base on DCC_EEPROM_STATE_INFO bits
#define DCC_EEPROM_STATE_SPEED			(3)	 

//Also include:
//1 bit 0x80 speed format 128 or 28
//1 bit 0x40 F5-F8  active
//1 bit 0x20 F9-F12 active
#define DCC_EEPROM_STATE_INFO			(4)   
#define DCC_EEPROM_STATE_INFO_MASK		(0xE0)   
#define DCC_EEPROM_STATE_SPEED_128		(0x80)   
#define DCC_EEPROM_STATE_ACTIVE_F5_F8	(0x40)   
#define DCC_EEPROM_STATE_ACTIVE_F9_F12	(0x20)   

//Always active
#define DCC_EEPROM_STATE_F0_F4			(4)   
#define DCC_EEPROM_STATE_F0_F4_MASK		(0x1F)   

//Active base on DCC_EEPROM_STATE_INFO bits
#define DCC_EEPROM_STATE_F5_F12			(5)
#define DCC_EEPROM_STATE_F5_F8_MASK 	(0xF0)
#define DCC_EEPROM_STATE_F5_F8_SHIFT 	(4)

#define DCC_EEPROM_STATE_F9_F12_MASK 	(0x0F)

#define GENERATION_COUNT				DCC_STATE_MAX_COUNT

#define STATE_KIND_UNKNOWN				(0)
#define STATE_KIND_SPEED_28				(1)
#define STATE_KIND_SPEED_128			(2)
#define STATE_KIND_SPEED_F0_F4			(3)
#define STATE_KIND_SPEED_F5_F8			(4)
#define STATE_KIND_SPEED_F9_F12			(5)
#define STATE_KIND_RESET_SPEED   		(6)
#define STATE_KIND_RESET_STATE 			(7)

DccStateKeeper DccState;

void DccStateKeeper::begin() {
	nextState = 0;
	state_count = EEPROM.read(DCC_EEPROM_ADDR_COUNT);
	generation = EEPROM.read(DCC_EEPROM_ADDR_GENERATION);
	if (state_count > DCC_STATE_MAX_COUNT || generation >= GENERATION_COUNT)
		resetAll();
}

void DccStateKeeper::resetSpeed() {
	saveBroadcastState(STATE_KIND_RESET_SPEED, NULL);
}

void DccStateKeeper::resetAll() {
	nextState = 0;
	state_count = 0;
	generation = 0;

	EEPROM.write(DCC_EEPROM_ADDR_COUNT, state_count);
	EEPROM.write(DCC_EEPROM_ADDR_GENERATION, generation);
}


void DccStateKeeper::saveState(DccPacket* packet) {
	if (!packet->isMultiFunction() || packet->isIdle())
		return;

	byte stateKind = extractStateKind(packet);
	if (stateKind == STATE_KIND_UNKNOWN)
		return;
		
	if (packet->isMultiFunctionBroadcast()) {
		saveBroadcastState(stateKind, packet);
		return;
	}
		
	word eeprom = findEeprom(packet);
	saveState(eeprom, stateKind, packet);
	
	updateAccess(eeprom);
}

void DccStateKeeper::readNextState(DccQueue& queue, DccStack& heap) {
	if (state_count == 0)
		return;

	word eeprom = DCC_EEPROM_ADDR_STATE_0 + nextState * DCC_EEPROM_STATE_SIZE;
	
	byte address0 	= EEPROM.read(eeprom + DCC_EEPROM_STATE_ADDRESS_0); 
	byte address1 	= EEPROM.read(eeprom + DCC_EEPROM_STATE_ADDRESS_1); 
	byte speed 	  	= EEPROM.read(eeprom + DCC_EEPROM_STATE_SPEED); 
	byte info_f0_f4 = EEPROM.read(eeprom + DCC_EEPROM_STATE_INFO);
	byte f5_f12 	= EEPROM.read(eeprom + DCC_EEPROM_STATE_F5_F12);
	
	if (info_f0_f4 & DCC_EEPROM_STATE_SPEED_128)	
		queue.add(heap.pop()->mfAddress(address0, address1).speed128(speed));
	else
		queue.add(heap.pop()->mfAddress(address0, address1).speed28(speed));

	queue.add(heap.pop()->mfAddress(address0, address1).functionF0_F4(info_f0_f4 & DCC_EEPROM_STATE_F0_F4_MASK));
	
	if (info_f0_f4 & DCC_EEPROM_STATE_ACTIVE_F5_F8)
		queue.add(heap.pop()->mfAddress(address0, address1).functionF5_F8((f5_f12 & DCC_EEPROM_STATE_F5_F8_MASK) >> DCC_EEPROM_STATE_F5_F8_SHIFT));
		
	if (info_f0_f4 & DCC_EEPROM_STATE_ACTIVE_F9_F12)
		queue.add(heap.pop()->mfAddress(address0, address1).functionF9_F12(f5_f12 & DCC_EEPROM_STATE_F9_F12_MASK));
		
	nextState = (nextState + 1) % state_count;
}

byte DccStateKeeper::extractStateKind(DccPacket* packet) {
	byte command = packet->dcc_data[packet->isAddressShort() ? 1 : 2];
	switch(command & DCC_MF_KIND3_MASK) {
		case DCC_MF_KIND3_CONTROL:				return (command == DCC_MF_DECODER_SOFT_RESET) ? STATE_KIND_RESET_SPEED : (command == DCC_MF_DECODER_HARD_RESET) ? STATE_KIND_RESET_STATE : STATE_KIND_UNKNOWN;
		case DCC_MF_KIND3_ADVANCED_OPERATION: 	return (command == DCC_MF_KIND8_SPEED_128) ? STATE_KIND_SPEED_128 : STATE_KIND_UNKNOWN; 
		case DCC_MF_KIND3_REVERSE_OPERATION:
		case DCC_MF_KIND3_FORWARD_OPERATION: 	return STATE_KIND_SPEED_28;
		case DCC_MF_KIND3_F0_F4: 				return STATE_KIND_SPEED_F0_F4;
		case DCC_MF_KIND3_F5_F12: 				return ((command & DCC_MF_KIND4_MASK) == DCC_MF_KIND4_F5_F8) ? STATE_KIND_SPEED_F5_F8 : STATE_KIND_SPEED_F9_F12;
	}
	return STATE_KIND_UNKNOWN;
}


void DccStateKeeper::saveBroadcastState(byte stateKind, DccPacket* packet) {
	for (word eeprom  = DCC_EEPROM_ADDR_STATE_0 + (state_count - 1) * DCC_EEPROM_STATE_SIZE; 
		 	  eeprom >= DCC_EEPROM_ADDR_STATE_0; 
		 	  eeprom -= DCC_EEPROM_STATE_SIZE) 
		saveState(eeprom, stateKind, packet);
}

void DccStateKeeper::saveState(word eeprom, byte stateKind, DccPacket* packet) {
	switch(stateKind) {
		case STATE_KIND_SPEED_28:	 	updateSpeed28(eeprom, packet); break;
		case STATE_KIND_SPEED_128:	 	updateSpeed128(eeprom, packet); break;
		case STATE_KIND_SPEED_F0_F4:	updateF0_F4(eeprom, packet); break;
		case STATE_KIND_SPEED_F5_F8:	updateF5_F8(eeprom, packet); break;
		case STATE_KIND_SPEED_F9_F12:	updateF9_F12(eeprom, packet); break;
		case STATE_KIND_RESET_SPEED: 	resetSpeed(eeprom); break;
		case STATE_KIND_RESET_STATE: 	resetState(eeprom); break;
	}
}

word DccStateKeeper::findEeprom(DccPacket* packet) {
	byte address0 = packet->dcc_data[0];
	byte address1 = packet->isAddressShort() ? 0 : packet->dcc_data[1];

	byte oldest_access = generation + GENERATION_COUNT;
	word oldest_eeprom = -1;
	
	for (word eeprom  = DCC_EEPROM_ADDR_STATE_0 + (state_count - 1) * DCC_EEPROM_STATE_SIZE; 
		 	  eeprom >= DCC_EEPROM_ADDR_STATE_0; 
		 	  eeprom -= DCC_EEPROM_STATE_SIZE) {
		 	  
		if (   EEPROM.read(eeprom + DCC_EEPROM_STATE_ADDRESS_0) == address0 
			&& EEPROM.read(eeprom + DCC_EEPROM_STATE_ADDRESS_1) == address1)
			return eeprom;

		byte access = EEPROM.read(eeprom + DCC_EEPROM_STATE_ACCESSED);
		if (access <= generation)
			access += GENERATION_COUNT;
		if (access <= oldest_access)
			oldest_eeprom = eeprom;
	}
	
	if (state_count < DCC_STATE_MAX_COUNT)
		return appendAddress(packet);
		
	resetAddress(oldest_eeprom, packet);	
	return oldest_eeprom;
}

word DccStateKeeper::appendAddress(DccPacket* packet) {
	word eeprom =  DCC_EEPROM_ADDR_STATE_0 + state_count * DCC_EEPROM_STATE_SIZE;
	resetAddress(eeprom, packet);
	
	state_count = state_count + 1;
	EEPROM.write(DCC_EEPROM_ADDR_COUNT, state_count);
	
	return eeprom;
}

void DccStateKeeper::resetAddress(word eeprom, DccPacket* p) {
	generation = (generation + 1) % GENERATION_COUNT;
	EEPROM.write(DCC_EEPROM_ADDR_GENERATION, generation);
	
	EEPROM.write(eeprom + DCC_EEPROM_STATE_ADDRESS_0, p->dcc_data[0]);
	EEPROM.write(eeprom + DCC_EEPROM_STATE_ADDRESS_1, p->isAddressShort() ? 0 : p->dcc_data[1]);
	
	resetState(eeprom);
}

void DccStateKeeper::updateAccess(word eeprom) {
	EEPROM.write(eeprom + DCC_EEPROM_STATE_ACCESSED, generation);
}

void DccStateKeeper::updateSpeed28(word eeprom, DccPacket* p) {
	byte state = EEPROM.read(eeprom + DCC_EEPROM_STATE_INFO) & ~DCC_EEPROM_STATE_SPEED_128;
	EEPROM.write(eeprom + DCC_EEPROM_STATE_INFO, state);

	EEPROM.write(eeprom + DCC_EEPROM_STATE_SPEED, p->dcc_data[p->isAddressShort() ? 1 : 2]);
}

void DccStateKeeper::updateSpeed128(word eeprom, DccPacket* p) {
	byte state = EEPROM.read(eeprom + DCC_EEPROM_STATE_INFO) | DCC_EEPROM_STATE_SPEED_128;
	EEPROM.write(eeprom + DCC_EEPROM_STATE_INFO, state);

	EEPROM.write(eeprom + DCC_EEPROM_STATE_SPEED, p->dcc_data[p->isAddressShort() ? 2 : 3]);
}

void DccStateKeeper::updateF0_F4(word eeprom, DccPacket* p) {
	byte state = EEPROM.read(eeprom + DCC_EEPROM_STATE_F0_F4) & ~DCC_EEPROM_STATE_F0_F4_MASK;
	
	EEPROM.write(eeprom + DCC_EEPROM_STATE_F0_F4, state | (p->dcc_data[p->isAddressShort() ? 1 : 2] & DCC_MF_FUNCTION_F0_F4_MASK));
}

void DccStateKeeper::updateF5_F8(word eeprom, DccPacket* p) {
	byte state = EEPROM.read(eeprom + DCC_EEPROM_STATE_INFO) | DCC_EEPROM_STATE_ACTIVE_F5_F8;
	EEPROM.write(eeprom + DCC_EEPROM_STATE_INFO, state);

	state = EEPROM.read(eeprom + DCC_EEPROM_STATE_F5_F12) & ~DCC_EEPROM_STATE_F5_F8_MASK;
	EEPROM.write(eeprom + DCC_EEPROM_STATE_F5_F12, state | ((p->dcc_data[p->isAddressShort() ? 1 : 2] & DCC_MF_FUNCTION_F5_F8_MASK) << DCC_EEPROM_STATE_F5_F8_SHIFT));
}

void DccStateKeeper::updateF9_F12(word eeprom, DccPacket* p) {
	byte state = EEPROM.read(eeprom + DCC_EEPROM_STATE_INFO) | DCC_EEPROM_STATE_ACTIVE_F9_F12;
	EEPROM.write(eeprom + DCC_EEPROM_STATE_INFO, state);

	state = EEPROM.read(eeprom + DCC_EEPROM_STATE_F5_F12) & ~DCC_EEPROM_STATE_F9_F12_MASK;
	EEPROM.write(eeprom + DCC_EEPROM_STATE_F5_F12, state | (p->dcc_data[p->isAddressShort() ? 1 : 2] & DCC_MF_FUNCTION_F9_F12_MASK));
}

void DccStateKeeper::resetSpeed(word eeprom) {
	byte speed = EEPROM.read(eeprom + DCC_EEPROM_STATE_SPEED);
	if (EEPROM.read(eeprom + DCC_EEPROM_STATE_INFO) & DCC_EEPROM_STATE_SPEED_128)
		EEPROM.write(eeprom + DCC_EEPROM_STATE_SPEED, speed & DCC_MF_SPEED_128_DIRECTION_MASK);
	else	
		EEPROM.write(eeprom + DCC_EEPROM_STATE_SPEED, speed & DCC_MF_KIND3_MASK);
}

void DccStateKeeper::resetState(word eeprom) {
	EEPROM.write(eeprom + DCC_EEPROM_STATE_INFO,  0);
	EEPROM.write(eeprom + DCC_EEPROM_STATE_SPEED, DCC_MF_KIND3_FORWARD_OPERATION | DCC_MF_SPEED_28_STOP);
	EEPROM.write(eeprom + DCC_EEPROM_STATE_F0_F4, 0);
	EEPROM.write(eeprom + DCC_EEPROM_STATE_F5_F12, 0);
}
	

