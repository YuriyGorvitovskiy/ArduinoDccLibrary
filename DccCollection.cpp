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
#include "DccCollection.h"

#define FILTER_KIND_UNKNOWN 		(0x0)
#define FILTER_KIND_MF_SPEED_28 	(0x1)
#define FILTER_KIND_MF_SPEED_128 	(0x2)
#define FILTER_KIND_MF_F0_F4 		(0x3)
#define FILTER_KIND_MF_F5_F8 		(0x4)
#define FILTER_KIND_MF_F9_F12 		(0x5)
#define FILTER_KIND_MF_F13_F20 		(0x6)
#define FILTER_KIND_MF_F20_F28 		(0x7)
#define FILTER_KIND_BA_OUTPUT 		(0x8)
#define FILTER_KIND_EA_OUTPUT 		(0x9)

DccStack::DccStack() {
	top = NULL;
}

DccStack::DccStack(DccPacket* packet, byte count) {
	if (packet == NULL || count <= 0) {
		top = NULL;
		return;
	}
		
	top = &packet[0];
	for(int i = 1; i < count; ++i)
		packet[i-1].next = &packet[i];
		
	packet[count-1].next = NULL;
}

byte DccStack::size() {
	byte count = 0;
	for(DccPacket* p = top; p != NULL; p = p->next)
		++count;

	return count;	
}

DccQueue::DccQueue() {
	first = last = NULL;
}

byte DccQueue::size() {
	byte count = 0;
	for(DccPacket* p = first; p != NULL; p = p->next)
		++count;

	return count;	
}

byte DccQueue::extractFilterKind(DccPacket* packet, boolean shortAddress) {
	if (packet->isMultiFunction()) {
		byte command = packet->dcc_data[shortAddress ? 1 : 2];
		switch(command & DCC_MF_KIND3_MASK) {
			case DCC_MF_KIND3_ADVANCED_OPERATION: 	return (command == DCC_MF_KIND8_SPEED_128) ? FILTER_KIND_MF_SPEED_128 : FILTER_KIND_UNKNOWN; 
			case DCC_MF_KIND3_REVERSE_OPERATION:
			case DCC_MF_KIND3_FORWARD_OPERATION: 	return FILTER_KIND_MF_SPEED_28;
			case DCC_MF_KIND3_F0_F4: 				return FILTER_KIND_MF_F0_F4;
			case DCC_MF_KIND3_F5_F12: 				return ((command & DCC_MF_KIND4_MASK) == DCC_MF_KIND4_F5_F8) ? FILTER_KIND_MF_F5_F8 : FILTER_KIND_MF_F9_F12;
			case DCC_MF_KIND3_FUTURE_EXPANSION:		return (command == DCC_MF_KIND8_F13_F20) ? FILTER_KIND_MF_F13_F20 : (command == DCC_MF_KIND8_F21_F28) ? FILTER_KIND_MF_F20_F28 : FILTER_KIND_UNKNOWN;	
			default:								return FILTER_KIND_UNKNOWN;
		}	
	} else if (packet->isBasicAccessory()) {
		return packet->size() == 3 ? FILTER_KIND_BA_OUTPUT : FILTER_KIND_UNKNOWN;
	} else if (packet->isExtendedAccessory()) {
		return packet->size() == 4 ? FILTER_KIND_EA_OUTPUT : FILTER_KIND_UNKNOWN;
	}
	return FILTER_KIND_UNKNOWN;
}

boolean DccQueue::replaceSameKindPacket(DccPacket* packet, boolean resetRepeat) {
	boolean shortAddress = packet->isAddressShort();
	byte kind = extractFilterKind(packet, shortAddress);
	if (kind == FILTER_KIND_UNKNOWN)
		return false;
	
	boolean broadcast = packet->isBroadcast();
	boolean changed = false;
	for(DccPacket* qp = first; qp != NULL; qp = qp->next)	{
		if (!broadcast && qp->dcc_data[0] != packet->dcc_data[0])
			continue;
			
		boolean qpShortAddress = qp->isAddressShort();	
		if(kind != extractFilterKind(qp, qpShortAddress))
			continue;
			
		switch(kind) {
			case FILTER_KIND_MF_SPEED_28: 
			case FILTER_KIND_MF_F0_F4:
			case FILTER_KIND_MF_F5_F8:
			case FILTER_KIND_MF_F9_F12:
							//broadcast address also short
							if (!shortAddress && qp->dcc_data[1] != packet->dcc_data[1])
								continue;
							qp->dcc_data[qpShortAddress ? 1 : 2] = packet->dcc_data[shortAddress ? 1 : 2];
							break;
			case FILTER_KIND_MF_SPEED_128: 
			case FILTER_KIND_MF_F13_F20: 
			case FILTER_KIND_MF_F20_F28: 
							//broadcast address also short
							if (!shortAddress && qp->dcc_data[1] != packet->dcc_data[1])
								continue;
							qp->dcc_data[qpShortAddress ? 2 : 3] = packet->dcc_data[shortAddress ? 2 : 3];
							break;
			case FILTER_KIND_BA_OUTPUT: 
							if (broadcast) {
								if (((qp->dcc_data[1] ^ packet->dcc_data[1]) & DCC_BA_ADDRESS_PAIR_MASK) != 0)
									continue;
								qp->dcc_data[1] = (qp->dcc_data[1] & DCC_BA_ADDRESS_MASK_2)	| (packet->dcc_data[1] & ~DCC_BA_ADDRESS_MASK_2);
								break;
							}
							//matching address and port
							if (((qp->dcc_data[1] ^ packet->dcc_data[1]) & (DCC_BA_ADDRESS_MASK_2 | DCC_BA_ADDRESS_PAIR_MASK)) != 0)
								continue;
							qp->dcc_data[1] = packet->dcc_data[1];
							break;
			case FILTER_KIND_EA_OUTPUT: 
							if (!broadcast && qp->dcc_data[1] != packet->dcc_data[1])
								continue;
							qp->dcc_data[2] = packet->dcc_data[2];
							break;
			default: 		
							continue;
		}
		changed = true;
		if (resetRepeat)
			qp->resetRepeat();
	}
	return changed;
}
