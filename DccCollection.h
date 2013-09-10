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

#ifndef __DCC_COLLECTION_H__
#define __DCC_COLLECTION_H__

#include <Arduino.h>
#include "DccPacket.h"

class DccQueue {
	
private:
	DccPacket* 		 first;
	DccPacket* 		 last;
	
public:
	DccQueue();
	
	void 		add(DccPacket* packet);
	void 		push(DccPacket* packet);
	DccPacket* 	next();
	
	DccPacket* 	getFirst();
	DccPacket* 	getLast();
	
	byte 		size();
	boolean 	isEmpty();
	
	//If provided packet has the same address (of if specified command is broadcast) and same command (Speed, Function, Accessory Output)
	//Then command will be substituted, with provided one
	//In case the resetRepeatsToZero is true, also substituted command repeat will be reduced to 0
	boolean 	replaceSameKindPacket(DccPacket* packet, boolean resetRepeat);
	
private:
	byte 		extractFilterKind(DccPacket* packet, boolean shortAddress);
};

class DccStack {
	
private:

	DccPacket* top;
	
public:
	DccStack();
	DccStack(DccPacket packet[], byte count);
	
	void 		push(DccPacket* packet);
	DccPacket* 	pop();
	
	DccPacket* 	getTop();
	
	byte 		size();
	boolean 	isEmpty();
};


inline void DccQueue::push(DccPacket* packet) {
	packet->next = first;
	first = packet;
	if (last == NULL)
		last = first;
}

inline void DccQueue::add(DccPacket* packet) {
	packet->next = NULL;
	
	// add() can be interrupted by next() call and should be safe
	// at every atomic operation 
	DccPacket* lst = last;
	if (lst != NULL)
		lst->next = packet;
		
	last = packet;
	if (first == NULL)
		first = last;
}

inline DccPacket* DccQueue::next() {
	DccPacket* packet = first;
	if (packet != NULL) {
		first = packet->next;
		if (packet == last)
			last = NULL;
	}
	return packet;
}

inline DccPacket* DccQueue::getFirst() {
	return first;
}

inline DccPacket* DccQueue::getLast() {
	return last;
}

inline boolean DccQueue::isEmpty() {
	return first == NULL;
}

inline void DccStack::push(DccPacket* packet) {
	packet->next = top;
	top = packet;
}

inline DccPacket* DccStack::getTop() {
	return top;
}

inline DccPacket* DccStack::pop() {
	DccPacket* packet = top;
	if (packet != NULL)
		top = packet->next;
		
	return packet;
}

inline boolean DccStack::isEmpty() {
	return top == NULL;
}



#endif //__DCC_QUEUE_H__
