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
#include "DccStandard.h"
#include "DccPacket.h"

// Process Dcc Hex Command
// All Hex Characters are CAPITAL
// dcc_info, dcc_data[0], ..., dcc_data[dcc_info_size-2]
DccPacket* DccPacket::parseDccHexCommand(const char* s) {
	dcc_info  = parseHex(*s++) << 4;
	dcc_info |= parseHex(*s++);
	int e = size() - 1;
	dcc_data[e] = 0;
	for(int i = 0; i < e; ++i) {
		dcc_data[i]  = parseHex(*s++) << 4;
		dcc_data[i] |= parseHex(*s++);
		dcc_data[e] ^= dcc_data[i];
	}
	return this;
}

// Address:
// m### - Short Multi Function Address (7bit). Missing # or 0 - Broadcast
// M#### - Long Multi Function Address (14bit)
// B####P#O# - Basic Accessory Decoder Address (9bit), Port, Output. Missing # or 511 - Broadcast
// E#### - Extended Accessory Decoder Address (11bit). Missing # or 2047 - Broadcast
DccPacket*  DccPacket::parseDccTextCommand(const char* s) {
	switch(*s++) {
		case 'm': 	if (!mfAddress7(parseNumber(s)).parseDccTextMFCommand(s))
						break;
				  	return this;
		case 'M': 	if (!mfAddress14(parseNumber(s)).parseDccTextMFCommand(s))
					  	break;
				  	return this;
		case 'B': 	{
				  	word address = (isDigit(*(s)) ? parseNumber(s) : DCC_BA_ADDRESS_BROADCAST);
				  	if (*s++ != 'P')
				  		break;
				  	byte port = parseNumber(s);		
				  	if (*s++ != 'O')
				  		break;
				  	byte output = parseNumber(s);
				  	if (!baAddress(address, port, output).parseDccTextBACommand(s))
				  		break;
				  	return this;
				  }
		case 'E': 	if (!eaAddress(isDigit(*(s)) ? parseNumber(s) : DCC_EA_ADDRESS_BROADCAST).parseDccTextEACommand(s))
				  		break;
				  	return this;

	}
	return NULL;
}

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
boolean DccPacket::parseDccTextMFCommand(const char*& s) {
	switch(*s++) {
		case 'f': speed28(true, parseNumber(s)); return true;
		case 'r': speed28(false, parseNumber(s)); return true;
		case 'F': speed128(true, parseNumber(s)); return true;
		case 'R': speed128(false, parseNumber(s)); return true;
		case 'A': functionF0_F4(parseBoolean(s[0]), parseBoolean(s[1]), parseBoolean(s[2]), parseBoolean(s[3]), parseBoolean(s[4])); s+=5; return true;
		case 'B': functionF5_F8(parseBoolean(s[0]), parseBoolean(s[1]), parseBoolean(s[2]), parseBoolean(s[3])); s+=4; return true;
		case 'C': functionF9_F12(parseBoolean(s[0]), parseBoolean(s[1]), parseBoolean(s[2]), parseBoolean(s[3])); s+=4; return true;
		case 'D': functionF13_F20(parseBoolean(s[0]), parseBoolean(s[1]), parseBoolean(s[2]), parseBoolean(s[3]), parseBoolean(s[4]), parseBoolean(s[5]), parseBoolean(s[6]), parseBoolean(s[7])); s+=8; return true;
		case 'E': functionF21_F28(parseBoolean(s[0]), parseBoolean(s[1]), parseBoolean(s[2]), parseBoolean(s[3]), parseBoolean(s[4]), parseBoolean(s[5]), parseBoolean(s[6]), parseBoolean(s[7])); s+=8; return true;
		default: break;
	}
	return false;
}

// Command for Basic Accessory:
// A:    Activate Basic Accessory
// D:    Deactivate Basic Accessory
boolean DccPacket::parseDccTextBACommand(const char*& s) {
	switch(*s) {
		case 'A': activate(true); return true;
		case 'D': activate(false); return true;
	}
	return false;
}

// Command for Extended Accessory:
// S##:  Set State
boolean DccPacket::parseDccTextEACommand(const char*& s) {
	switch(*s++) {
		case 'S': state(parseNumber(s)); return true;
	}
	return false;
}


//Idle
DccPacket* DccPacket::idle() {
	dcc_info = DCC_INFO_SIZE_3 | DCC_INFO_NO_ACKNOWLEDGE | DCC_INFO_NO_REPEAT;
	dcc_data[0] = DCC_ADDRESS_IDLE;
	dcc_data[1] = 0x00;
	dcc_data[2] = dcc_data[0] ^ dcc_data[1];
	return this;
}

DccPacket& DccPacket::mfBroadcast() {
	dcc_data[0] = DCC_ADDRESS_BROADCAST;
	return *this;
}

DccPacket& DccPacket::mfAddress7 (byte address) {
	dcc_data[0] = address & DCC_ADDRESS_SHORT_MASK;
	return *this;
}

DccPacket& DccPacket::mfAddress14(word address) {
	dcc_data[0] = DCC_ADDRESS_LONG_MIN + ((address>>8) & DCC_ADDRESS_SHORT_MASK);
	dcc_data[1] = (address & 0xFF);
	return *this;
}

DccPacket& DccPacket::mfAddress(byte address0, byte address1) {
	dcc_data[0] = address0;
	if (address0 > DCC_ADDRESS_SHORT_MAX)
		dcc_data[1] = address1;

	return *this;
}

DccPacket* DccPacket::speed14 (boolean forward, byte speed) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | ((speed < DCC_MF_SPEED_14_MIN ? DCC_REPEAT_STOP : DCC_REPEAT_SPEED) & DCC_INFO_REPEAT_MASK);
	
	byte command = (forward ? DCC_MF_KIND3_FORWARD_OPERATION : DCC_MF_KIND3_REVERSE_OPERATION)
				 | (speed & DCC_MF_SPEED_14_MASK);
				 			 
	return mfCommand1(command);
}

DccPacket* DccPacket::speed28 (boolean forward, byte speed) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | ((speed < DCC_MF_SPEED_28_MIN ? DCC_REPEAT_STOP : DCC_REPEAT_SPEED) & DCC_INFO_REPEAT_MASK);
	
	byte command = (forward ? DCC_MF_KIND3_FORWARD_OPERATION : DCC_MF_KIND3_REVERSE_OPERATION)
				 | ((speed>>DCC_MF_SPEED_28_HBIT_SHIFT) & DCC_MF_SPEED_28_HBIT_MASK)
				 | ((speed<<DCC_MF_SPEED_28_LBIT_SHIFT) & DCC_MF_SPEED_28_LBIT_MASK);
				 			 
	return mfCommand1(command);
}
DccPacket* DccPacket::speed28 (byte dcc_bits) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | (((dcc_bits & DCC_MF_SPEED_14_MASK) < DCC_MF_SPEED_14_MIN ? DCC_REPEAT_STOP : DCC_REPEAT_SPEED) & DCC_INFO_REPEAT_MASK);
	
	return mfCommand1(dcc_bits);
}

DccPacket* DccPacket::speed128(boolean forward, byte speed) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | ((speed < DCC_MF_SPEED_128_MIN ? DCC_REPEAT_STOP : DCC_REPEAT_SPEED) & DCC_INFO_REPEAT_MASK);
   	         
   	byte command = (forward ? DCC_MF_SPEED_128_FORWARD : DCC_MF_SPEED_128_REVERSE)
   				 | (speed & DCC_MF_SPEED_128_MASK);  
   				        
	return mfCommand2(DCC_MF_KIND8_SPEED_128, command);
}

DccPacket* DccPacket::speed128(byte dcc_bits) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | (((dcc_bits & DCC_MF_SPEED_128_MASK) < DCC_MF_SPEED_128_MIN ? DCC_REPEAT_STOP : DCC_REPEAT_SPEED)  & DCC_INFO_REPEAT_MASK);
   	         
	return mfCommand2(DCC_MF_KIND8_SPEED_128, dcc_bits);
}

DccPacket* DccPacket::functionF0_F4(boolean f0,  boolean f1,  boolean f2,  boolean f3,  boolean f4) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | (DCC_REPEAT_FUNCTION & DCC_INFO_REPEAT_MASK);
	
	byte command = DCC_MF_KIND3_F0_F4
				 | (f0 ? DCC_MF_FUNCTION_F0 : 0)
				 | (f1 ? DCC_MF_FUNCTION_F1 : 0)
				 | (f2 ? DCC_MF_FUNCTION_F2 : 0)
				 | (f3 ? DCC_MF_FUNCTION_F3 : 0)
				 | (f4 ? DCC_MF_FUNCTION_F4 : 0);
				 			 
	return mfCommand1(command);
}

DccPacket* DccPacket::functionF0_F4(byte dcc_bits) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | (DCC_REPEAT_FUNCTION & DCC_INFO_REPEAT_MASK);
	
	return mfCommand1(DCC_MF_KIND3_F0_F4 | dcc_bits);
}

DccPacket* DccPacket::functionF5_F8(boolean f5,  boolean f6,  boolean f7,  boolean f8) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | (DCC_REPEAT_FUNCTION & DCC_INFO_REPEAT_MASK);
	
	byte command = DCC_MF_KIND4_F5_F8
				 | (f5 ? DCC_MF_FUNCTION_F5 : 0)
				 | (f6 ? DCC_MF_FUNCTION_F6 : 0)
				 | (f7 ? DCC_MF_FUNCTION_F7 : 0)
				 | (f8 ? DCC_MF_FUNCTION_F8 : 0);
				 			 
	return mfCommand1(command);
}

DccPacket* DccPacket::functionF5_F8(byte dcc_bits) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | (DCC_REPEAT_FUNCTION & DCC_INFO_REPEAT_MASK);
	
	return mfCommand1(DCC_MF_KIND4_F5_F8 | dcc_bits);
}

DccPacket* DccPacket::functionF9_F12 (boolean f9,  boolean f10, boolean f11, boolean f12) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | (DCC_REPEAT_FUNCTION & DCC_INFO_REPEAT_MASK);
	
	byte command = DCC_MF_KIND4_F9_F12
				 | (f9  ? DCC_MF_FUNCTION_F9  : 0)
				 | (f10 ? DCC_MF_FUNCTION_F10 : 0)
				 | (f11 ? DCC_MF_FUNCTION_F11 : 0)
				 | (f12 ? DCC_MF_FUNCTION_F12 : 0);
				 			 
	return mfCommand1(command);
}

DccPacket* DccPacket::functionF9_F12(byte dcc_bits) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | (DCC_REPEAT_FUNCTION & DCC_INFO_REPEAT_MASK);
	
	return mfCommand1(DCC_MF_KIND4_F9_F12 | dcc_bits);
}

DccPacket* DccPacket::functionF13_F20(boolean f13, boolean f14, boolean f15, boolean f16, boolean f17, boolean f18, boolean f19, boolean f20) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | (DCC_REPEAT_FUNCTION & DCC_INFO_REPEAT_MASK);
	
	byte command = (f13 ? DCC_MF_FUNCTION_F13 : 0)
				 | (f14 ? DCC_MF_FUNCTION_F14 : 0)
				 | (f15 ? DCC_MF_FUNCTION_F15 : 0)
				 | (f16 ? DCC_MF_FUNCTION_F16 : 0)
				 | (f17 ? DCC_MF_FUNCTION_F17 : 0)
				 | (f18 ? DCC_MF_FUNCTION_F18 : 0)
				 | (f19 ? DCC_MF_FUNCTION_F19 : 0)
				 | (f20 ? DCC_MF_FUNCTION_F20 : 0);
				 			 
	return mfCommand2(DCC_MF_KIND8_F13_F20, command);
}

DccPacket* DccPacket::functionF13_F20(byte dcc_bits) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | (DCC_REPEAT_FUNCTION & DCC_INFO_REPEAT_MASK);
	
	return mfCommand2(DCC_MF_KIND8_F13_F20, dcc_bits);
}


DccPacket* DccPacket::functionF21_F28(boolean f21, boolean f22, boolean f23, boolean f24, boolean f25, boolean f26, boolean f27, boolean f28) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | (DCC_REPEAT_FUNCTION & DCC_INFO_REPEAT_MASK);
	
	byte command = (f21 ? DCC_MF_FUNCTION_F21 : 0)
				 | (f22 ? DCC_MF_FUNCTION_F22 : 0)
				 | (f23 ? DCC_MF_FUNCTION_F23 : 0)
				 | (f24 ? DCC_MF_FUNCTION_F24 : 0)
				 | (f25 ? DCC_MF_FUNCTION_F25 : 0)
				 | (f26 ? DCC_MF_FUNCTION_F26 : 0)
				 | (f27 ? DCC_MF_FUNCTION_F27 : 0)
				 | (f28 ? DCC_MF_FUNCTION_F28 : 0);
				 			 
	return mfCommand2(DCC_MF_KIND8_F21_F28, command);
}

DccPacket* DccPacket::functionF21_F28(byte dcc_bits) {
	dcc_info = DCC_INFO_NO_ACKNOWLEDGE 
   	         | (DCC_REPEAT_FUNCTION & DCC_INFO_REPEAT_MASK);
	
	return mfCommand2(DCC_MF_KIND8_F21_F28, dcc_bits);
}

DccPacket* DccPacket::mfCommand1(byte command) {
	if (isAddressShort()) {
		dcc_info |= DCC_INFO_SIZE_3;
		dcc_data[1] = command;
		dcc_data[2] = dcc_data[0] ^ dcc_data[1];
	} else {
		dcc_info |= DCC_INFO_SIZE_4;
		dcc_data[2] = command;
		dcc_data[3] = dcc_data[0] ^ dcc_data[1] ^ dcc_data[2];
	}
	return this;
}

DccPacket* DccPacket::mfCommand2(byte command1, byte command2) {
	if (isAddressShort()) {
		dcc_info |= DCC_INFO_SIZE_4;
		dcc_data[1] = command1;
		dcc_data[2] = command2;
		dcc_data[3] = dcc_data[0] ^ dcc_data[1] ^ dcc_data[2];
	} else {
		dcc_info |= DCC_INFO_SIZE_5;
		dcc_data[2] = command1;
		dcc_data[3] = command2;
		dcc_data[4] = dcc_data[0] ^ dcc_data[1] ^ dcc_data[2] ^ dcc_data[3];
	}
	return this;
}

	// Basic Accessory
DccPacket& DccPacket::baBroadcast(byte port, byte output) {
	dcc_data[0] = DCC_ADDRESS_ACCESSORY_MIN + DCC_BA_ADDRESS_BROADCAST_1;
	dcc_data[1] = DCC_ACCESSORY_KIND_BASIC
				| DCC_BA_ADDRESS_BROADCAST_2
				| ((port << DCC_BA_ADDRESS_PAIR_SHIFT) & DCC_BA_ADDRESS_PAIR_MASK)
				| (output & DCC_BA_ADDRESS_OUTPUT_MASK);
				
	return *this;
}

DccPacket& DccPacket::baAddress(word address, byte port, byte output) {
	dcc_data[0] = DCC_ADDRESS_ACCESSORY_MIN 
				+ (address & DCC_BA_ADDRESS_MASK_1);
				
	dcc_data[1] = DCC_ACCESSORY_KIND_BASIC
				| (((address >> DCC_BA_ADDRESS_SHIFT) & DCC_BA_ADDRESS_MASK_2) ^ DCC_BA_ADDRESS_MASK_2)
				| ((port << DCC_BA_ADDRESS_PAIR_SHIFT) & DCC_BA_ADDRESS_PAIR_MASK)
				| (output & DCC_BA_ADDRESS_OUTPUT_MASK);
				
	return *this;
}

DccPacket* DccPacket::activate(boolean on) {
	dcc_info = DCC_INFO_SIZE_3 
			 | (DCC_REPEAT_ACCESSORY & DCC_INFO_REPEAT_MASK);
	dcc_data[1] |= (on ? DCC_BA_ACTIVATE : DCC_BA_DEACTIVATE);
	dcc_data[2] = dcc_data[0] ^ dcc_data[1];
	return this;
}
	
// Extended Accessory
DccPacket& DccPacket::eaBroadcast() {
	dcc_data[0] = DCC_ADDRESS_ACCESSORY_MIN + DCC_EA_ADDRESS_BROADCAST_1;
	dcc_data[1] = DCC_ACCESSORY_EXTENDED
				| DCC_EA_ADDRESS_BROADCAST_2
				| DCC_EA_ADDRESS_BROADCAST_3;
	return *this;
}

DccPacket& DccPacket::eaAddress(word address) {
	dcc_data[0] = DCC_ADDRESS_ACCESSORY_MIN 
				+ (address & DCC_EA_ADDRESS_MASK_1);
				
	dcc_data[1] = DCC_ACCESSORY_EXTENDED
				| (((address >> DCC_EA_ADDRESS_SHIFT_2) & DCC_EA_ADDRESS_MASK_2) ^ DCC_EA_ADDRESS_MASK_2)
				| (((address >> DCC_EA_ADDRESS_SHIFT_3) & DCC_EA_ADDRESS_MASK_3));
				
	return *this;
}

DccPacket* DccPacket::state(byte newState) {
	dcc_info = DCC_INFO_SIZE_4 
			 | (DCC_REPEAT_ACCESSORY & DCC_INFO_REPEAT_MASK);
	dcc_data[2] = (newState & DCC_EA_STATE_MASK);
	dcc_data[3] = dcc_data[0] ^ dcc_data[1] ^ dcc_data[2];
	return this;
}



