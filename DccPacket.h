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

#ifndef __DCC_PACKET_H__
#define __DCC_PACKET_H__

#include <Arduino.h>
#include "DccStandard.h"



// Dcc Data Packet Size
//======================================================
// (dcc_info & 0xC0) == 0x00 packet Size 3 bytes
// (dcc_info & 0xC0) == 0x40 packet Size 4 bytes
// (dcc_info & 0xC0) == 0x80 packet Size 5 bytes
// (dcc_info & 0xC0) == 0xC0 packet Size 6 bytes
#define DCC_INFO_SIZE_MASK            (0xC0)
#define DCC_INFO_SIZE_SHIFT           (6)

#define DCC_INFO_SIZE_3               (0x00)
#define DCC_INFO_SIZE_4               (0x40)
#define DCC_INFO_SIZE_5               (0x80)
#define DCC_INFO_SIZE_6               (0xC0)

// Dcc Wait/Acknowledgement Info
//======================================================
// (dcc_info & 0x30) == 0x00 Expect No Acknowledge,      no wait.
// (dcc_info & 0x30) == 0x10 Expect No Acknowledge,      wait till complete
// (dcc_info & 0x30) == 0x20 Expect Acknowledge 1 byte,  wait till complete
// (dcc_info & 0x30) == 0x30 Expect Acknowledge 2 bytes, wait till complete
#define DCC_INFO_ACKNOWLEDGE_MASK     (0x30)

#define DCC_INFO_NO_ACKNOWLEDGE       (0x00)
#define DCC_INFO_NO_ACKNOWLEDGE_WAIT  (0x10)
#define DCC_INFO_ACKNOWLEDGE_1        (0x20)
#define DCC_INFO_ACKNOWLEDGE_2        (0x30)


// Dcc Packet Repeat
//======================================================
// (dcc_info & 0x0F) <= 0x0F repeat count
// (dcc_info & 0x0F) == 0x00 processed
#define DCC_INFO_REPEAT_MASK               (0x0F)

#define DCC_INFO_NO_REPEAT                 (0x00)
#define DCC_INFO_REPEAT_1                  (0x01)
#define DCC_INFO_REPEAT_2                  (0x02)
#define DCC_INFO_REPEAT_3                  (0x03)
#define DCC_INFO_REPEAT_4                  (0x04)
#define DCC_INFO_REPEAT_5                  (0x05)
#define DCC_INFO_REPEAT_6                  (0x06)
#define DCC_INFO_REPEAT_7                  (0x07)
#define DCC_INFO_REPEAT_MAX                (0x0F)

struct DccPacket {

public:

    //+----------------------------------------------------+
    //| DCC Packet Info (size, acknowledgement, repeat)	   |
    //+----------------------------------------------------+
    byte                       dcc_info;

    //+----------------------------------------------------+
    //|        Actual DCC Packet With ERROR byte           |
    //+----------------------------------------------------+
    byte                       dcc_data[DCC_DATA_SIZE_MAX];

    //+----------------------------------------------------+
    //| Reference To the NEXT DCC packet in Queue or Stack |
    //+----------------------------------------------------+
    DccPacket*                 next;

public:
	// dcc_info functions
	byte 		size();

	boolean 	hasToWait();
	boolean 	hasAcknowledge();
	boolean 	isAcknowledgeShort();

	byte     	repeat();
	byte     	decrementRepeat();
	void     	resetRepeat();

	// dcc_data functions
	boolean 	isIdle();

	boolean 	isMultiFunction();
	boolean 	isMultiFunctionBroadcast();
	boolean 	isAddressShort();

	boolean 	isAccessory();
	boolean 	isBasicAccessory();
	boolean 	isBasicAccessoryBroadcast();
	boolean 	isExtendedAccessory();
	boolean 	isExtendedAccessoryBroadcast();

	boolean 	isBroadcast();

public:
	// Building Functions

	// Process Dcc Hex Command
	// All Hex Characters are CAPITAL
	// dcc_info, dcc_data[0], ..., dcc_data[dcc_info_size-1]
	DccPacket*  parseDccHexCommand(const char*s);

	// Process Dcc Text Command
	// All Numbers are decimal
	// Has to parts: Address, and Command

	// Address:
	// m### - Short Multi Function Address (7bit). Missing # or 0 - Broadcast
	// M#### - Long Multi Function Address (14bit)
	// B####P#O# - Basic Accessory Decoder Address (9bit), Port, Output. Missing # or 511 - Broadcast
	// E#### - Extended Accessory Decoder Address (11bit). Missing # or 2047 - Broadcast

	// Command for Multi Function:
	// f###:   Forward Speed (28steps):  0,1 - stop, 2,3 - Emergency Stop
	// r###:   Reverse Speed (28steps):  0,1 - stop, 2,3 - Emergency Stop
	// F###:   Forward Speed (127steps): 0 - stop, 1 - Emergency Stop
	// R###:   Reverse Speed (127steps): 0 - stop, 1 - Emergency Stop
	// A#####:    Function Set F0,   F1,  F2,  F3,  F4 					  (0/1 per position)
	// B####:  	  Function Set F5,   F6,  F7,  F8 	  					  (0/1 per position)
	// C####:  	  Function Set F9,  F10, F11, F12  	  					  (0/1 per position)
	// D########: Function Set F13, F14, F15, F16, F17, F18, F19, F20  	  (0/1 per position)
	// E########: Function Set F21, F22, F23, F24, F25, F26, F27, F28  	  (0/1 per position)

	// Command for Basic Accessory:
	// A:    Activate Basic Accessory
	// D:    Deactivate Basic Accessory

	// Command for Extended Accessory:
	// S##:  Set State

	DccPacket*  parseDccTextCommand(const char* s);

	//Idle
	DccPacket* idle();

	// Multi-Function
	DccPacket& mfBroadcast();
	DccPacket& mfAddress7 (byte address);
	DccPacket& mfAddress14(word address);
	DccPacket& mfAddress(byte address0, byte address1);

	DccPacket* speed14 (boolean forward, byte speed);
	DccPacket* speed28 (boolean forward, byte speed);
	DccPacket* speed128(boolean forward, byte speed);
	DccPacket* speed28(byte dcc_bits);
	DccPacket* speed128(byte dcc_bits);

	DccPacket* functionF0_F4  (boolean f0,  boolean f1,  boolean f2,  boolean f3,  boolean f4);
	DccPacket* functionF5_F8  (boolean f5,  boolean f6,  boolean f7,  boolean f8);
	DccPacket* functionF9_F12 (boolean f9,  boolean f10, boolean f11, boolean f12);
	DccPacket* functionF13_F20(boolean f13, boolean f14, boolean f15, boolean f16, boolean f17, boolean f18, boolean f19, boolean f20);
	DccPacket* functionF21_F28(boolean f21, boolean f22, boolean f23, boolean f24, boolean f25, boolean f26, boolean f27, boolean f28);
	DccPacket* functionF0_F4  (byte dcc_bits);
	DccPacket* functionF5_F8  (byte dcc_bits);
	DccPacket* functionF9_F12 (byte dcc_bits);
	DccPacket* functionF13_F20(byte dcc_bits);
	DccPacket* functionF21_F28(byte dcc_bits);

	DccPacket* mfCommand1(byte command);
	DccPacket* mfCommand2(byte command1, byte command2);

	// Basic Accessory
	DccPacket& baBroadcast(byte port, byte output);
	DccPacket& baAddress  (word address, byte port, byte output);

	DccPacket* activate(boolean on);

	// Extended Accessory
	DccPacket& eaBroadcast();
	DccPacket& eaAddress  (word address);

	DccPacket* state(byte newState);

public:
	boolean parseDccTextMFCommand(const char*& s);
	boolean parseDccTextBACommand(const char*& s);
	boolean parseDccTextEACommand(const char*& s);

	static byte 	parseHex(char ch);
	static boolean 	isDigit(char ch);
	static boolean  parseBoolean(char ch);
	static word 	parseNumber(const char*& ch);

};

inline byte DccPacket::size() {
	return ((dcc_info & DCC_INFO_SIZE_MASK) >> DCC_INFO_SIZE_SHIFT) + DCC_DATA_SIZE_MIN;
}

inline boolean DccPacket::hasToWait() {
	return (dcc_info & (DCC_INFO_ACKNOWLEDGE_MASK)) != 0;
}

inline boolean DccPacket::hasAcknowledge() {
	return (dcc_info & DCC_INFO_ACKNOWLEDGE_MASK) >= DCC_INFO_ACKNOWLEDGE_1;
}

inline boolean DccPacket::isAcknowledgeShort() {
	return ((dcc_info & DCC_INFO_ACKNOWLEDGE_MASK) == DCC_INFO_ACKNOWLEDGE_1);
}

inline byte DccPacket::repeat() {
	return (dcc_info & DCC_INFO_REPEAT_MASK);
}

inline byte DccPacket::decrementRepeat() {
	if (repeat() > 0)
		--dcc_info;

	return repeat();
}

inline void DccPacket::resetRepeat() {
	dcc_info &= ~DCC_INFO_REPEAT_MASK;
}

inline boolean DccPacket::isIdle() {
	return (dcc_data[0] == DCC_ADDRESS_IDLE);
}

inline boolean DccPacket::isMultiFunctionBroadcast() {
	return (dcc_data[0] == DCC_ADDRESS_BROADCAST);
}

inline boolean DccPacket::isMultiFunction() {
	return (dcc_data[0] <= DCC_ADDRESS_SHORT_MAX)
		|| (DCC_ADDRESS_LONG_MIN <= dcc_data[0] && dcc_data[0] <= DCC_ADDRESS_LONG_MAX);
}

inline boolean DccPacket::isAddressShort() {
	return (dcc_data[0] <= DCC_ADDRESS_SHORT_MAX);
}

inline boolean DccPacket::isAccessory() {
	return (DCC_ADDRESS_ACCESSORY_MIN <= dcc_data[0] && dcc_data[0] <= DCC_ADDRESS_ACCESSORY_MAX);
}

inline boolean DccPacket::isBasicAccessory() {
	return isAccessory() && ((dcc_data[1] & DCC_ACCESSORY_KIND_MASK) == DCC_ACCESSORY_KIND_BASIC);
}

inline boolean DccPacket::isBasicAccessoryBroadcast() {
	return (dcc_data[0] == DCC_ADDRESS_ACCESSORY_BROADCAST)
		&& ((dcc_data[1] & (DCC_ACCESSORY_KIND_MASK | DCC_BA_ADDRESS_MASK_2)) == (DCC_ACCESSORY_KIND_BASIC | DCC_BA_ADDRESS_BROADCAST_2));
}

inline boolean DccPacket::isExtendedAccessory() {
	return isAccessory() && ((dcc_data[1] & DCC_ACCESSORY_KIND_MASK) == DCC_ACCESSORY_KIND_EXTENDED);
}

inline boolean DccPacket::isExtendedAccessoryBroadcast() {
	return (dcc_data[0] == DCC_ADDRESS_ACCESSORY_BROADCAST)
		&& (dcc_data[1] == (DCC_ACCESSORY_EXTENDED | DCC_EA_ADDRESS_BROADCAST_2 | DCC_EA_ADDRESS_BROADCAST_3));
}

inline boolean DccPacket::isBroadcast() {
	if (isMultiFunctionBroadcast())
		return true;

	if (dcc_data[0] != DCC_ADDRESS_ACCESSORY_BROADCAST)
		return false;

	return ((dcc_data[1] & (DCC_ACCESSORY_KIND_MASK | DCC_BA_ADDRESS_MASK_2)) == (DCC_ACCESSORY_KIND_BASIC | DCC_BA_ADDRESS_BROADCAST_2))
		|| (dcc_data[1] == (DCC_ACCESSORY_EXTENDED | DCC_EA_ADDRESS_BROADCAST_2 | DCC_EA_ADDRESS_BROADCAST_3));
}

inline byte DccPacket::parseHex(char ch) {
    return 0xF & (('0' <= ch && ch <= '9') ? (uint8_t)(ch - '0') : (uint8_t)(ch - 'A' + 10));
};

inline boolean DccPacket::isDigit(char ch) {
	return ('0' <= ch && ch <= '9');
}

inline word DccPacket::parseNumber(const char*& ch) {
	int v = 0;
	while (isDigit(*ch)) {
		v = (v * 10) + *ch++ -'0';
	}
    return v;
};

inline boolean DccPacket::parseBoolean(char ch) {
	switch(ch) {
		case '1':
		case 'Y':
		case 'y':
		case 'T':
		case 't': return true;
	}
	return false;
}

#endif //__DCC_PACKET_H__
