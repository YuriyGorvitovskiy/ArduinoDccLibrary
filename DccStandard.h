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
#ifndef __DCC_STANDARD_H__
#define __DCC_STANDARD_H__

#include <Arduino.h>


/** 
 * DCC signal is looking like this:
 *
 * +V -+  +--+    +----+    +----+  +--+             +--+
 *     |  |  |    |    |    |    |  |  |             |  |
 * 0V  |  |  |    |    |    |    |  |  | +========+  |  |
 *     |  |  |    |    |    |    |  |  | |        |  |  |
 * -V  +--+  +----+    +----+    +--+  +-+        +--+  +-
 *
 *     ^     ^         ^         ^     ^ ^        ^     ^
 *     A     B         C         D     E F        G     H
 *
 *    A-B: sending bit 1 t = 2 *  58us
 *    B-C: sending bit 0 t = 2 * 100us
 *    C-D: sending bit 0 t = 2 * 100us
 *    D-E: sending bit 1 t = 2 *  58us
 *    E-F: preparing for cutout t = 28us
 *    F-G: cutout t(for 1 byte feedback) = 224us - 28us = 196us
 *                t(for 2 byte feedback) = 448us - 28us = 420us
 *    G-H: sending bit 1 t = 2 *  58us
 *
 * To generate Positive and Negative voltage we will use 2 digital outputs pins, that set in oposit value.
 *   - In that case if pinA = 1 and pinB = 0 the output between 2 pins is Positive,
 *   - In that case if pinA = 0 and pinB = 1 the output between 2 pins is Negative,
 *   - In that case if pinA = 0 and pinB = 0 the output between 2 pins is 0 - cutout.
 * 
 * To switch pins in the proper time we will use timer. Good article: http://letsmakerobots.com/node/28278
 */
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega328__)  || defined(__AVR_ATmega328P__) 

/** There are at least 3 timers in ATmega168 and ATmega328:
 *   - Timer 0: is  8-bit counter, used by system functions like delay() and better to be avoided to be used. For example, using it will break WiFi commnication,
 *   - Timer 1: is 16-bit counter, will be used by us,
 *   - Timer 2: is  8-bit counter, could't be used, because it doesn't have enough precission to support DCC writing and Reading Decoder Feedback.
 *
 * Timer can perform switch of pins for us, but each timer is associated with specific pins:
 *   - Timer 0: pin  5 & pin  6
 *   - Timer 1: pin  9 & pin 10
 *   - Timer 2: pin 11 & pin  3
 *
 * To catch conflict with other library, or shield on pins is very easy. For example on RedBack WiFi board pins 9-14 are used to communicate with WiFi chip.
 *
 * To avoid this conflict, and reliably generate DCC signal we will:
 *    1. Disable pin usage by the timer,
 *    2. Define Timer Interupt function,
 *    3. Switch pin values, first thing in the interupt function
 *    4. Calculate and set Timer Count for the next interrupt .
 *
 * Timer Count is a value that defines the time between interrupt calls. In our case it defines the time of next interrupt call.
 * To calculte timer_count we will use the following formula: http://www.instructables.com/id/Arduino-Timer-Interrupts/
 *
 *    interrupt frequency (Hz) = (Arduino clock speed) / (prescaler * (compare match register + 1))
 *
 *      - Arduino clock speed      = for ATmega168 and ATmega328 is 16,000,000Hz
 *      - compare match register   = timer_counter
 *      - interrupt frequency (Hz) = 1,000,000 / interrupt_delay(us) 
 *      - prescalar                = 8 (you can use 1, 8, 64, 256, 1024)
 *
 *  => 1,000,000 / interrupt_delay(us) = 16,000,000 / 8 * (timer_counter + 1)
 *
 *  => 1 / interrupt_delay(us) = 2 / (timer_counter + 1)
 *
 *  => (timer_counter + 1) = 2 * interrupt_delay(us)
 *
 *     +---------------------------------------------+
 *  => | timer_counter = 2 * interrupt_delay(us) - 1 |
 *     +---------------------------------------------+
 *
 */

#if (F_CPU == 16000000L)

// Timer delay to send bit with value 0 on DCC rail = 100us
#define  TIMER_COUNT_SEND_0       (199) 

// Timer delay to send bit with value 1 on DCC rail = 58us
#define  TIMER_COUNT_SEND_1       (115) 

// Timer delay after last packet bit to DCC rail cutout = 28us
#define  TIMER_COUNT_CUTOUT_START  (55) 

// Timer delay for cutout to recieve 1 byte feedback from decoder = 224us - 28us = 196us
#define  TIMER_COUNT_CUTOUT_END_1 (391) 

// Timer delay for cutout to recieve 2 byte feedback from decoder =  448us - 28us = 420us
#define  TIMER_COUNT_CUTOUT_END_2 (839) 

#else

#error Unsupported CPU speed

#endif //F_CPU

#elif defined(__MK20DX128__)

/** There are 2 timers on MK20DX128:
 *   - Timer 0: is 16-bit counter, used by system functions like delay() and better to be avoided to be used. For example, using it will break WiFi commnication,
 *   - Timer 1: is 16-bit counter, will be used by us,
 */
// Allowed only 0 or 1
#define DCC_TIMER 0

/**
 * Timer Count is a value that defines the time between interrupt calls. In our case it defines the time of next interrupt call.
 * To calculte timer_count we will use the following formula: http://www.instructables.com/id/Arduino-Timer-Interrupts/
 *
 *    (interrupt frequency (Hz)) = (Arduino clock speed(Hz)) / (prescaler * (compare match register + 1))
 *
 *      - Arduino clock speed      = for Teense 3.0 is 24,000,000Hz, 48,000,000Hz, 96,000,000Hz
 *      - compare match register   = timer_counter
 *      - interrupt frequency (Hz) = 1,000,000 / interrupt_delay(us) 
 *      - interrupt frequency (Hz) = 1,000     / interrupt_delay(ms) 
 *      - prescalar                = 8 (you can use 1, 2, 4, 8, 32, 64, 128)
 *
 *    (prescaler * (compare match register + 1)) = (Arduino clock speed(Hz)) / (interrupt frequency (Hz));
 *     compare match register = (Arduino clock speed(Hz)) / ((interrupt frequency (Hz)) * prescaler) - 1;
 *     compare match register = (Arduino clock speed(Hz)) * interrupt_delay(us) / (1,000,000 * prescaler) - 1;
 *     compare match register = (Arduino clock speed(Hz)) * interrupt_delay(ms) / (1,000     * prescaler) - 1;
 */
#define FTM_PRESCALE_FACTOR      (3)
#define FTM_PRESCALE             (1 << FTM_PRESCALE_FACTOR)
#define FTM_MOD_FOR_MICROSEC(us) (((F_CPU/1000000)*(us)/FTM_PRESCALE) - 1)
#define FTM_MOD_FOR_MILLISEC(ms) (((F_CPU/1000)*(ms)/FTM_PRESCALE) - 1)

// Timer delay to send bit with value 0 on DCC rail = 100us
#define  TIMER_COUNT_SEND_0       	FTM_MOD_FOR_MICROSEC(100) 

// Timer delay to send bit with value 1 on DCC rail = 58us
#define  TIMER_COUNT_SEND_1       	FTM_MOD_FOR_MICROSEC(58) 

// Timer delay after last packet bit to DCC rail cutout = 28us
#define  TIMER_COUNT_CUTOUT_START  	FTM_MOD_FOR_MICROSEC(28) 

// Timer delay for cutout to recieve 1 byte feedback from decoder = 224us - 28us = 196us
#define  TIMER_COUNT_CUTOUT_END_1 	FTM_MOD_FOR_MICROSEC(196) 

// Timer delay for cutout to recieve 2 byte feedback from decoder =  448us - 28us = 420us
#define  TIMER_COUNT_CUTOUT_END_2 	FTM_MOD_FOR_MICROSEC(420) 

#else

#error Unsupported CPU type

#endif //CPU TYPE

/**
 	EXTRACT FROM:  	Extended Packet Formats For Digital Command Control, All Scales
 					http://www.nmra.org/standards/DCC/standards_rps/RP-921%202006%20Aug%2021.pdf
	 				Copyright 1999-2006 by National Model Railroad Association, Inc.

	NMRA RECOMMENDED PRACTICES 
	Extended Packet Formats For Digital Command Control, All Scales 
	2006 August 21 RP 9.2.1 
	
	This Recommended Practice received approval from the NMRA Board of Trustees in July 1995, March 1997, July 2003 and January 2006. 
	Changes since the last approved version are indicated by change bars on the left or right side of the document. Minor changes made to agree with RP-9.2.2. 
	Copyright 2006, by National Model Railroad Association, Inc. 
	NMRA Technical Department #635
	
5  	The NMRA Communications Standard for Digital Communications (S-9.2) provides a minimal, basic packet format 
	required for interoperability.  This RECOMMENDED PRACTICE provides Extended Packet Formats that provide 
	the framework to enable realistic operations to occur.  These formats adhere to the general packet format as defined 
	in S-9.2. While the baseline packet has a length of 3 data bytes separated by a "0" bit, a packet using the extended 
	packet format definition may have a length of between 3 and 6 data bytes each separated by a "0" bit.  
*/
#define DCC_DATA_SIZE_MIN             (3)
#define DCC_DATA_SIZE_MAX             (6)

/**	
10 
	Format Definitions 
	Within this recommended practice, bits within the address and data bytes will be defined using the following 
	abbreviations.  Individual bytes within a specific packet format are separated by spaces.  Bytes which are within 
15  square [] brackets can occur one or more times as necessary.  Bits are numbered from right to left with bit 0 (the 
	right most bit) being the least significant bit (LSB) and bit 7 (the left most bit) being the most significant bit (MSB).  
		A = Address bit 
		0 = Bit with the value of "0" 
20  	1 = Bit with the value of "1" 
		U = bit with undefined value either a "1" or a "0" is acceptable 
		B = Bit Position / Address 
		C = Instruction Type field 
		F = Flag to determine Instruction Implementation 
25  	L = Low Order Binary State Control Address 
		H = High Order Binary State Control Address 
		S = Decoder Sub Address 
		V = CV Address Bit 
		D = Data  
30  	X = Signal Head Aspect Data Bit 
		E = Error Detection Bit 
	A: Address Partitions 
	=====================	
	The first data byte of an Extended Packet Format packet contains the primary address.  In order to allow for 
	different types of decoders this primary address is subdivided into fixed partitions as follows. 
35 
		Address   00000000 			(0)					: Broadcast address 
		Addresses 00000001-01111111 (1-127)  (inclusive): Multi-Function decoders with 7 bit addresses 
40 		Addresses 10000000-10111111 (128-191)(inclusive): Basic Accessory Decoders with 9 bit addresses and Extended Accessory Decoders with 11-bit addresses 
		Addresses 11000000-11100111 (192-231)(inclusive): Multi Function Decoders with 14 bit addresses 
		Addresses 11101000-11111110 (232-254)(inclusive): Reserved for Future Use 
		Address   11111111 			(255)				: Idle Packet 
*/
#define DCC_ADDRESS_BROADCAST  	 		(0x00) 
#define DCC_ADDRESS_IDLE  	   	 		(0xFF) 

#define DCC_ADDRESS_SHORT_MASK   		(0x7F)
#define DCC_ADDRESS_SHORT_MIN  			(0x01)
#define DCC_ADDRESS_SHORT_MAX   		(0x7F)

#define DCC_ADDRESS_ACCESSORY_BROADCAST	(0xBF) 
#define DCC_ADDRESS_ACCESSORY_MIN		(0x80) 
#define DCC_ADDRESS_ACCESSORY_MAX		(0xBF) 

#define DCC_ADDRESS_LONG_MASK   		(0x3F)
#define DCC_ADDRESS_LONG_MIN   			(0xC0)
#define DCC_ADDRESS_LONG_MAX    		(0xE7)

#define DCC_ADDRESS_RESERVED_MIN   		(0xE8)
#define DCC_ADDRESS_RESERVED_MAX    	(0xFE)

/** 

50 	B: Broadcast Command for Multi Function Digital Decoders  
	========================================================
	The format for this packet is: 
	
	{preamble}  0  00000000  0  {instruction-bytes}  0  EEEEEEEE  1 
55 
	Instructions addressed to "broadcast address" 00000000 must be executed by all Multi Function Digital Decoders.  
	The single instruction has the same definition as defined by the Multi Function Digital Decoder packet and can be 
	one, two, or three bytes in length depending on the instruction.  Digital Decoders should ignore any instruction they 
	do not support.  The manufacturer must document which broadcast commands a decoder supports. 
	
60  C: Instruction Packets for Multi Function Digital Decoders 
	========================================================== 
	The formats for these packets are: 
	
	{preamble}  0  [ AAAAAAAA ]  0  {instruction-bytes}  0  EEEEEEEE  1 
	
65  Multi Function Digital Decoders are used for the purpose of controlling one or more motors and/or accessories.  
	The packet format used to control these devices consists of between 3 and 6 bytes where the first bytes are address 
	bytes followed by one or two instruction bytes and ended by an error control byte.
	The first address byte contains 8 bits of address information.  If the most significant bits of the address are "11"and 
70  the remaining bits are not “111111”, then a second address byte must immediately follow.  This second address byte 
	will then contain an additional 8 bits of address data.  When 2 bytes of address information are present they are 
	separated by a "0" bit.  The most significant bit of two byte addresses is bit 5 of the first address byte. (bits #6 and 
	#7 having the value of "1" in this case.  
75  Instruction-bytes are data bytes used to send commands to Multi Function Digital Decoders.  Although it is unlikely 
	that all Digital Decoders will implement all instructions, it is important that if they support packets with more than a 
	single instruction, they can sufficiently parse the packet to be able to recognize if a byte is a new instruction or the 
	second byte of a previous instruction. 
80  Each instruction (indicated by {instruction-bytes}) is defined to be:  
		{instruction-bytes} =  CCCDDDDD, 
							   CCCDDDDD  0  DDDDDDDD, or 
							   CCCDDDDD  0  DDDDDDDD  0  DDDDDDDD 
85 
	Each instruction consists of a 3-bit instruction type field followed by a 5-bit data field.  Some instructions have one 
	or two or three additional bytes of data. The 3-bit instruction type field is defined as follows: 
	
		000 Decoder and Consist Control Instruction  
90  	001 Advanced Operation Instructions 
		010 Speed and Direction Instruction for reverse operation 
		011 Speed and Direction Instruction for forward operation 
		100 Function Group One Instruction 
		101 Function Group Two Instruction 
95  	110 Future Expansion 
		111 Configuration Variable Access Instruction 
		
*/
#define DCC_MF_KIND2_MASK 					(0xC0) 
#define DCC_MF_KIND3_MASK 					(0xE0) 
#define DCC_MF_KIND4_MASK 					(0xF0)
#define DCC_MF_KIND8_MASK 					(0xFF)

#define DCC_MF_KIND2_SPEED_OPERATION  		(0x40) 

#define DCC_MF_KIND3_CONTROL 				(0x00) 
#define DCC_MF_KIND3_ADVANCED_OPERATION		(0x20) 
#define DCC_MF_KIND3_REVERSE_OPERATION		(0x40) 
#define DCC_MF_KIND3_FORWARD_OPERATION		(0x60) 
#define DCC_MF_KIND3_F0_F4					(0x80) 
#define DCC_MF_KIND3_F5_F12					(0xA0) 
#define DCC_MF_KIND3_FUTURE_EXPANSION		(0xC0) 
#define DCC_MF_KIND3_CONFIG_VARIABLE_ACCESS	(0xE0) 

/**
	The last byte of the packet is the Error Detection Byte, which is calculated the same as is done in the baseline packet 
	using all address, and all instruction bytes (see S-9.2). 
100 
	Decoder and Consist Control Instruction (000) 
	---------------------------------------------
	With the exception of the decoder acknowledgment function (00001111), only a single decoder and consist control 
	instruction may be contained in a packet. 
	
	Decoder Control (0000) 
	----------------------
105 The decoder control instructions are intended to set up or modify decoder configurations.   
	This instruction has the format of  
	
	{instruction byte} = 0000CCCF, or {instruction byte} = 0000CCCF DDDDDDDD 
110 
	This instruction (0000CCCF) allows specific decoder features to be set or cleared as defined by the value of D ("1" 
	indicates set).  When the decoder has decoder acknowledgment enabled, receipt of a decoder control instruction 
	shall be acknowledged with an operations mode acknowledgment. 
115  	CCC = 000 D = "0": 	Digital Decoder Reset - A Digital Decoder Reset shall erase all volatile memory 
							(including and speed and direction data), and return to its initial power up state as defined in RP-
							9.2.4 section A. Command Stations shall not send packets to addresses 112-127 for 10 packet 
							times following a Digital Decoder Reset. This is to ensure that the decoder does not start 
							executing service mode instruction packets as operations mode packets (Service Mode 
120  						instruction packets have a short address in the range of 112 to 127 decimal.) 
  		D = "1": Hard Reset - Configuration Variables 29, 31 and 32 are reset to its factory default 
							conditions, CV#19 is set to "00000000" and a Digital Decoder reset (as in the above instruction) 
							shall be performed.   
		CCC = 001 Factory Test Instruction - This instruction is used by manufacturers to test decoders at the 
125  		factory. It must not be sent by any command station during normal operation.  This 
			instruction may be a multi-byte instruction. 
		CCC = 010 Reserved for future use 
		CCC = 011 Set Decoder Flags (see below) 
		CCC = 100 Reserved for future use 
130  	CCC = 101 Set Advanced Addressing (CV#29 bit 5) 
		CCC = 110 Reserved for future use 
		CCC = 111 D = 1: Decoder Acknowledgment Request.
*/
#define DCC_MF_KIND4_DECODER_CONTROL 		(0x00) 

#define DCC_MF_DECODER_CONTROL_MASK 		(0x0F) 

#define DCC_MF_DECODER_RESET 				(0x00) 
#define DCC_MF_DECODER_SOFT_RESET 			(0x00) 
#define DCC_MF_DECODER_HARD_RESET 			(0x01) 

#define DCC_MF_DECODER_FACTORY_TEST 		(0x02) 

#define DCC_MF_DECODER_SET_FLAGS	 		(0x07) 
#define DCC_MF_DECODER_UNSET_FLAGS	 		(0x06) 

#define DCC_MF_DECODER_SET_SHORT_ADDRESS	(0x0A) 
#define DCC_MF_DECODER_SET_LONG_ADDRESS		(0x0B) 

#define DCC_MF_DECODER_ACKNOWLEDGMENT		(0x0F) 

/**		
		  
135 Set Decoder Flags
	-----------------
	This instruction is under re-evaluation by the NMRA DCC Working Group.  
	Manufacturers should contact the NMRA DCC Coordinator before implementing this instruction. 
	Set Decoder Flags is an expanded decoder control function that allows for the command station to turn on or 
	off flags within a specific decoder or within a group of decoders. 
140 
	Format: 
		{instruction bytes} = 0000011D CCCC0SSS 
		
145  	SSS is the decoder’s sub-address.  This allows for up to 7 decoders to share the same decoder primary address, 
			yet have certain functions (such as Configuration Variable Access Instructions) be performed on an individual basis. 
			If SSS = 000 then the operation affects all decoders within the group.  The decoder sub-address is defined 
			in CV15.
150  	CCCC is defined in the following table: 

			  CCCC  |          Meaning          |              Action if D = 1                 	|               Scope 
			--------+---------------------------+-----------------------------------------------+-----------------------------
			  0000  | Disable 111 Instructions  | Instruction is ignored for all sub addresses.	| Until next Digital Decoder 
				    | This instruction also 	|												| Reset Packet is received. 
				    | applies to accessory 		|												|		
				    | decoders.                 |                                              	|
			--------+---------------------------+-----------------------------------------------+-----------------------------
			  0100  | Disable Decoder           | Acknowledgement and Address are not          	| Until power is removed long 
			        | Acknowledgement Request   | transmitted in response to a Decoder			| enough to power down the
			        | Instruction			   	| Acknowledgement Instruction for all sub 		| decoder. 
					|							| addresses. 									|
			--------+---------------------------+-----------------------------------------------+-----------------------------
			  0101 	| Activate Bi-Directional 	| B-Directional Communications are enabled 		| Permanent (sets CV 29 (or 
					| Communications 			| per CVs (See note below). 					| 541), bit 3) or internal Flag 
					|							|												| if sent to Consist Address. 
			--------+---------------------------+-----------------------------------------------+-----------------------------
			 1000 	| Set Bi-Directional 		| Bi-Directional communications are enabled		| Permanent (sets CV [32]16, 
					| Communications 			| for specified sub address; all other sub 		| bit 0) 
					|							| addresses are disabled. (Not valid at Consist |
					|							| Address). 									|
			--------+---------------------------+-----------------------------------------------+-----------------------------
			 1001 	| Set 111 Instruction 		| Enables 111 Instructions for specified sub-	| Permanent (sets CV [32]16, 
			 		|							| address, all other sub addresses are disabled.| bit 1) 
			 		|							| (Not valid at Consist Address). 				|
			--------+---------------------------+-----------------------------------------------+-----------------------------
			 1111 	| Accept 111 Instructions 	| All multi-CV programming instructions are  	| One-Time 
			 		|							| now valid. 									|
		Note:  	This command is valid at both the decoder’s base address and (if active) the consist address.  If sent to 
			   	the base address, the command affects both the base address and the active consist address (if any). If 
155  		   	sent to the consist address, and D=0 this command has no effect on the base address. If sent to the 
				consist address, and D=1 this command has no effect. 
*/
#define DCC_MF_DECODER_FLAG_MASK  					(0xF0) 

#define DCC_MF_DECODER_FLAG_DISABLE_CV_CMD  		(0x00) 
#define DCC_MF_DECODER_FLAG_ALLOW_CV_CMD  			(0x0F) 
#define DCC_MF_DECODER_FLAG_ALLOW_SUB_CV_CMD  		(0x09) 

#define DCC_MF_DECODER_FLAG_DISABLE_ACKNOWLEDGE 	(0x40) 
#define DCC_MF_DECODER_FLAG_ALLOW_ACKNOWLEDGE  		(0x50) 
#define DCC_MF_DECODER_FLAG_ALLOW_SUB_ACKNOWLEDGE  	(0x80) 

#define DCC_MF_DECODER_FLAGS_SUBADDR_MASK 			(0x07) 
#define DCC_MF_DECODER_FLAGS_SUBADDR_ALL 			(0x00) 
#define DCC_MF_DECODER_FLAGS_SUBADDR_MIN 			(0x01) 
#define DCC_MF_DECODER_FLAGS_SUBADDR_MAX 			(0x07) 

/**				
 	Consist Control (0001) 
 	----------------------
	This instruction controls consist setup and activation or deactivation. 
160 
	When Consist Control is in effect, the decoder will ignore any speed or direction instructions addressed to its 
	normal locomotive address (unless this address is the same as its consist address). Speed and direction instructions 
	now apply to the consist address only. 
	
165 Functions controlled by instruction 100 and 101 will continue to respond to the decoders baseline address.  
	Functions controlled by instructions 100 and 101 also respond to the consist address if the appropriate bits in CVs 
	#21,22 have been activated. 
	
	By default all forms of Bi-directional communication are not activated in response to commands sent to the consist 
170 address until specifically activated by a Decoder Control instruction.  Operations mode acknowledgement and Data 
	Transmission applies to the appropriate commands at the respective decoder addresses.   
	The format of this instruction is: 
	
		 {instruction bytes} =   0001CCCC   0   0AAAAAAA 
175 
	A value of “1” in bit 7 of the second byte is reserved for future use. Within this instruction CCCC contains a consist 
	setup instruction, and the AAAAAAA in the second byte is a seven bit consist address.  If the address is "0000000" 
	then the consist is deactivated. If the address is non-zero, then the consist is activated. 
	
	If the consist is deactivated (by setting the consist to ‘0000000’), the Bi-Directional communications settings are set 
180 as specified in CVs 26-28. 
	
	When operations-mode acknowledgement is enabled, all consist commands must be acknowledged via operationsmode acknowledgement. 
	The format for CCCC shall be: 
185 
		CCCC=0010 	Set the consist address as specified in the second byte, and activate the consist.  The consist 
					address is stored in bits 0-6 of CV #19 and bit 7 of CV #19 is set to a value of 0. The direction 
					of this unit in the consist is the normal direction. If the consist address is 0000000 the consist 
					is deactivated. 
190 
		CCCC=0011 	Set the consist address as specified in the second byte and activate the consist.  The consist 
					address is stored in bits 0-6 of CV #19 and bit 7 of CV#19 is set to a value of 1. The direction 
					of this unit in the consist is opposite its normal direction. If the consist address is 0000000 the 
					consist is deactivated. 
195 
		All other values of CCCC are reserved for future use. 
*/		
#define DCC_MF_KIND4_CONSIST_CONTROL 		(0x10) 

#define DCC_MF_CONSIST_CONTROL_MASK 		(0x0F) 
#define DCC_MF_CONSIST_SET_ADDRESS_NORMAL	(0x02) 
#define DCC_MF_CONSIST_SET_ADDRESS_REVERSE	(0x03) 

#define DCC_MF_CONSIST_ADDRESS_MASK			(0x7F) 
#define DCC_MF_CONSIST_ADDRESS_DISABLE		(0x00) 
#define DCC_MF_CONSIST_ADDRESS_MIN			(0x01) 
#define DCC_MF_CONSIST_ADDRESS_MAX			(0x7F) 

/**		
	Advanced Operations Instruction (001) 
	-------------------------------------
	These instructions control advanced decoder functions.  Only a single advanced operations instruction may be 
	contained in a packet. 
200 
	The format of this instruction is:
	
		 001CCCCC  0  DDDDDDDD  
		 
	The 5-bit sub-instruction CCCCC allows for 32 separate Advanced Operations Sub-Instructions.   
	
205  	CCCCC = 11111: 	128 Speed Step Control - Instruction "11111" is used to send one of 126 Digital Decoder speed 
						steps. The subsequent single byte shall define speed and direction with bit 7 being direction ("1" is forward and "0" 
						is reverse) and the remaining bits used to indicate speed.  The most significant speed bit is bit 6. A data-byte value 
						of U0000000 is used for stop, and a data-byte value of U0000001 is used for emergency stop. This allows up to 126 
						speed steps. When operations mode acknowledgment is enabled, receipt of a 128 Speed Step Control packet must 
210  					be acknowledged with an operations mode acknowledgement. 
		CCCCC = 11110: 	Restricted Speed Step Instruction - Instruction “11110” is used to restrict the maximum speed of 
						a decoder.  Bit 7 of the data byte (‘DDDDDDDD’ above) is used to enable (‘0’) or disable (‘1’) restricted speed 
						operation.  Bits 0-5 of the Data byte are the Speed Steps defined in S-9.2  In 128 speed step mode, the maximum 
						restricted speed is scaled from 28 speed mode. When operations mode acknowledgment is enabled, receipt of 
215  					a Restricted Speed Instruction must be acknowledged with an operations mode acknowledgement. 

	The remaining 30 instructions are reserved for future use. 
 */
 
#define DCC_MF_KIND8_SPEED_128					(0x3F) 
#define DCC_MF_KIND8_SPEED_LIMIT				(0x3E) 

#define DCC_MF_SPEED_128_MASK					(0x7F) 
#define DCC_MF_SPEED_128_STOP					(0x00) 
#define DCC_MF_SPEED_128_EMERGENCY_STOP			(0x01) 
#define DCC_MF_SPEED_128_MIN					(0x02) 
#define DCC_MF_SPEED_128_MAX					(0x7F) 
#define DCC_MF_SPEED_128_DIRECTION_MASK			(0x80) 
#define DCC_MF_SPEED_128_FORWARD				(0x80) 
#define DCC_MF_SPEED_128_REVERSE				(0x00) 

 /**	
	
220 Speed and Direction Instructions (010 and 011) 
	----------------------------------------------
	
	These two instructions have these formats: 
    	for Reverse Operation  010DDDDD 
    	for Forward Operation  011DDDDD 
    	
225 A speed and direction instruction is used send information to motors connected to Multi Function Digital Decoders.  
	Instruction "010" indicates a Speed and Direction Instruction for reverse operation and instruction "011" indicates a 
	Speed and Direction Instruction for forward operation.  In these instructions the data is used to control speed with 
	bits 0-3 being defined exactly as in S-9.2 Section B. If Bit 1 of CV#29 has a value of one (1), then bit 4 is used as 
	an intermediate speed step, as defined in S-9.2, Section B. If Bit 1 of CV#29 has a value of zero (0), then bit 4 shall 
	be used to control FL. FL is used for the control of the headlights.
	
230 In this mode, Speed U0000 is stop, speed U0001 is emergency stop, speed U0010 is the 
	first speed step and speed U1111 is full speed.  This provides 14 discrete speed steps in each direction.   
                                                          
	If a decoder receives a new speed step that is within one step of current speed step, the Digital Decoder may select a 
	step half way between these two speed steps.  This provides the potential to control 56 speed steps should the 
	command station alternate speed packets. 
	
235 Decoders may ignore the direction information transmitted in a broadcast packet for Speed and Direction commands 
	that do not contain stop or emergency stop information. 
	When operations mode acknowledgment is enabled, receipt of any speed and direction packet must be 
	acknowledged with an operations mode acknowledgement. 
	
*/
	
#define DCC_MF_SPEED_14_MASK			(0x0F) 
#define DCC_MF_SPEED_14_STOP			(0x00)
#define DCC_MF_SPEED_14_EMERGENCY_STOP	(0x01)
#define DCC_MF_SPEED_14_MIN				(0x02)
#define DCC_MF_SPEED_14_MAX				(0x0F)

#define DCC_MF_SPEED_28_MASK			(0x1F)
#define DCC_MF_SPEED_28_STOP			(0x00)
#define DCC_MF_SPEED_28_EMERGENCY_STOP	(0x02)
#define DCC_MF_SPEED_28_MIN				(0x04)
#define DCC_MF_SPEED_28_MAX				(0x1F)

#define DCC_MF_SPEED_28_HBIT_MASK		(0x0F)
#define DCC_MF_SPEED_28_HBIT_SHIFT		(1)

#define DCC_MF_SPEED_28_LBIT_MASK		(0x10)
#define DCC_MF_SPEED_28_LBIT_SHIFT		(4)
 
/**	
	
	Function Group One Instruction (100)
	------------------------------------
	
	Any function in this packet group may be directionally qualified
240 
	The format of this instruction is 
			100DDDDD 

	Up to 5 auxiliary functions (functions FL and F1-F4) can be controlled by the Function Group One instruction.  Bits 
	0-3 shall define the value of functions F1-F4 with function F1 being controlled by bit 0 and function F4 being 
245 controlled by bit 3.  A value of "1" shall indicate that the function is "on" while a value of "0" shall indicate that the 
	function is "off".  If Bit 1 of CV#29 has a value of one (1), then bit 4 controls function FL, otherwise bit 4 has no meaning. 
	
	When operations mode acknowledgment is enabled, receipt of a function group 1 packet must be acknowledged 
250 according with an operations mode acknowledgement. 
*/

#define DCC_MF_FUNCTION_F0_F4_MASK			(0x1F) 
#define DCC_MF_FUNCTION_F0					(0x10) 
#define DCC_MF_FUNCTION_F1					(0x01) 
#define DCC_MF_FUNCTION_F2					(0x02) 
#define DCC_MF_FUNCTION_F3					(0x04) 
#define DCC_MF_FUNCTION_F4					(0x08) 

/**
	Function Group Two Instruction (101)
	------------------------------------
	
	Any function in this packet group may be directionally qualified.
	
	This instruction has the format 101SDDDD 
255 
	Up to 8 additional auxiliary functions (F5-F12) can be controlled by a Function Group Two instruction.  Bit 4 
	defines the use of Bits 0-3.  When bit 4 (S) is ‘1’, Bits 0-3 (DDDD) shall define the value of functions F5-F8 with 
	function F5 being controlled by bit 0 and function F8 being controlled by bit 3. When bit 4 (S) is ‘0’, Bits 0-3 
	(DDDD) shall define the value of functions F9-F12 with function F9 being controlled by bit 0 and function F12 
	being controlled by bit 3.  A value of "1" shall indicate that the function is "on" while a value of "0" shall indicate 
260 that the function is "off".   

	When operations mode acknowledgment is enabled, receipt of function group 2 packet shall be acknowledged 
	according with an operations mode acknowledgement. 
*/

#define DCC_MF_KIND4_F5_F8					(0xB0)
#define DCC_MF_KIND4_F9_F12					(0xA0)

#define DCC_MF_FUNCTION_F5_F8_MASK			(0x0F) 
#define DCC_MF_FUNCTION_F5					(0x01) 
#define DCC_MF_FUNCTION_F6					(0x02) 
#define DCC_MF_FUNCTION_F7					(0x04) 
#define DCC_MF_FUNCTION_F8					(0x08) 

#define DCC_MF_FUNCTION_F9_F12_MASK			(0x0F) 
#define DCC_MF_FUNCTION_F9					(0x01) 
#define DCC_MF_FUNCTION_F10					(0x02) 
#define DCC_MF_FUNCTION_F11					(0x04) 
#define DCC_MF_FUNCTION_F12					(0x08) 
/**	
	
265 Feature Expansion Instruction (110)  
	-----------------------------------
	
	The instructions in this group provide for support of additional features within decoder. (See TN-3-05) 
	
	The format of two byte instructions in this group is: 	110CCCCC   0   DDDDDDDD 
			
	The format of three byte instructions in this group is: 110CCCCC   0   DDDDDDDD   0   DDDDDDDD 
	
270 The 5-bit sub-instruction CCCCC allows for 32 separate Feature Expansion Sub-instructions.  

		CCCCC = 00000:  Binary State Control Instruction long form – Sub instruction "00000" is a three byte instruction 
						and provides for control of one of 32767 binary states within the decoder.  The two bytes following this instruction 
						byte have the format DLLLLLLL   0   HHHHHHHH".  Bits 0-6 of the first data byte (LLLLLLL) shall define the 
275  					low order bits of the binary state address; bits 0-7 of the second data byte (HHHHHHHH) shall define the high 
						order bits of binary state address.  The addresses range from 1 to 32767.  Bit 7 of the second byte (D) defines the 
						binary state.  A value of "1" shall indicate that the binary state is "on" while a value of "0" shall indicate that the 
						binary state is "off".  The value of 0 for the address is reserved as broadcast to clear or set all 32767 binary states.  
						An instruction "11000000   0   00000000   0   00000000" sets all 32767 binary states to off. 
280 
						Binary states accessed with all high address bits set to zero would be the same as accessed by the short form of the 
						binary state control.  Command stations shall use the short form in this case, i.e. Binary State Controls 1 to 127 
						should always be addressed using the short form.  Decoders supporting the long form shall support the short form as 
						well. 
*/
#define DCC_MF_KIND8_LONG_STATE_CONTROL 		(0xC0)

/**						
285 
		CCCCC = 11101:  Binary State Control Instruction short form – Sub-instruction “11101” is a two byte instruction 
						and provides for control of one of 127 binary states within the decoder.  The single byte following this instruction 
						byte has the format DLLLLLLL.  Bits 0-6 of the second byte (LLLLLLL) shall define the number of the binary 
						state starting with 1 and ending with 127.  Bit 7 (D) defines the binary state.  A value of "1" shall indicate the binary 
290  					state is "on" while a value of "0" shall indicate the binary state is "off".  The value of 0 for LLLLLLL is reserved as 
						broadcast to clear or set all 127 binary states accessible by the short form of the binary state control.  An instruction 
						"11011101   0   00000000" sets all 127 binary states accessed by this instruction to off. 
						
						Binary State Controls are quite similar to Functions, as they may control any output, sound or any other feature of 
295  					digital nature within a decoder in direct response to a packet received.  But Binary State Controls do have a different 
						access method and function space.  Therefore they have a different name. 
						
						Binary state control packets – both short and long form – will not be refreshed.  Therefore non-volatile storage of 
						the function status is suggested.  When operations mode acknowledgment is enabled, receipt of a Binary State 
300  					Control packet shall be acknowledged accordingly with an operations mode acknowledgment.  Consult the 
						Technical Note(s) for additional information on this instruction.  (See TN-4-05)  
*/
#define DCC_MF_KIND8_SHORT_STATE_CONTROL 		(0xDD)

/**						
		CCCCC = 11110:  F13-F20 Function Control – Sub-instruction “11110” is a two byte instruction and provides for 
						control of eight (8) additional auxiliary functions F13-F20.  The single byte following this instruction byte indicates 
305  					whether a given function is turned on or off, with the least significant bit (Bit 0) controlling F13, and the most 
						significant bit (bit 7) controlling F20.  A value of “1” in D for a given function shall indicate the function is “on” 
						while a value of “0” in D for a given function shall indicate a given function is “off”.  It is recommended, but not 
						required, that the status of these functions be saved in decoder storage such as NVRAM.  It is not required, and 
						should not be assumed that the state of these functions is constantly refreshed by the command station.  Command 
310  					Stations that generate these packets, and which are not periodically refreshing these functions, must send at least 
						two repetitions of these commands when any function state is changed.  When operations mode acknowledgment is 
						enabled, receipt of an F13-F20 Function Control packet shall be acknowledged accordingly with an operations 
						mode acknowledgement.  Consult the Technical Note(s), TN-4-05, for additional information on this instruction. 
*/
#define DCC_MF_KIND8_F13_F20 				(0xDE)

#define DCC_MF_FUNCTION_F13					(0x01) 
#define DCC_MF_FUNCTION_F14					(0x02) 
#define DCC_MF_FUNCTION_F15					(0x04) 
#define DCC_MF_FUNCTION_F16					(0x08) 
#define DCC_MF_FUNCTION_F17					(0x10) 
#define DCC_MF_FUNCTION_F18					(0x20) 
#define DCC_MF_FUNCTION_F19					(0x40) 
#define DCC_MF_FUNCTION_F20					(0x80) 


/**					
315  	CCCCC = 11111:  F21-F28 Function Control – Sub-instruction “11111” is a two byte instruction and provides for 
						control of eight (8) additional auxiliary functions F21-F28.  The single byte following this instruction byte indicates 
						whether a given function is turned on or off, with the least significant bit (Bit 0) controlling F21, and the most 
						significant bit (bit 7) controlling F28.  A value of “1” in D for a given function shall indicate the function is “on” 
						while a value of “0” in D for a given function shall indicate a given function is “off”.  It is recommended, but not 
320  					required that the status of these functions be saved in decoder storage such as NVRAM.  It is not required, and 
						should not be assumed that the state of these functions is constantly refreshed by the command station.  Command 
						Stations that generate these packets, and which are not periodically refreshing these functions, must send at least 
						two repetitions of these commands when any function state is changed.  When operations mode acknowledgment is 
						enabled, receipt of an F21-F28 Function Control packet shall be acknowledged accordingly with an operations 
325  					mode acknowledgement.  Consult the Technical Note(s), TN-4-05, for additional information on this instruction. 
*/
#define DCC_MF_KIND8_F21_F28 				(0xDF)

#define DCC_MF_FUNCTION_F21					(0x01) 
#define DCC_MF_FUNCTION_F22					(0x02) 
#define DCC_MF_FUNCTION_F23					(0x04) 
#define DCC_MF_FUNCTION_F24					(0x08) 
#define DCC_MF_FUNCTION_F25					(0x10) 
#define DCC_MF_FUNCTION_F26					(0x20) 
#define DCC_MF_FUNCTION_F27					(0x40) 
#define DCC_MF_FUNCTION_F28					(0x80) 

/**
		The remaining 28 sub-instructions are reserved by the NMRA for future use. The NMRA shall not issue a NMRA Conformance Warrant 
		for any product that uses an instruction or subinstruction that has been reserved by the NMRA.

	Configuration Variable Access Instruction (111) 
	-----------------------------------------------
	
	The Configuration Variable Access instructions are intended to set up or modify Configurations Variables either on 
330 the programming track or on the main line.  There are two forms of this instruction.  The short form is for modifying 
	selected frequently modified Configuration Variables.  The long form is for verifying or modifying any selected 
	Configuration Variable.  Only a single configuration variable access instruction may be contained in a packet.   
	
	Configuration Variable Access Acknowledgment 
	--------------------------------------------
	
	If a configuration variable access acknowledgment is required, and the decoder has decoder operations-mode 
335 acknowledgment enabled, the decoder shall respond with an operations mode acknowledgment.  
                                                          
	Configuration Variable Access Instruction - Short Form 
	------------------------------------------------------
	
	This instruction has the format of  
	
			1111CCCC  0  DDDDDDDD 
340 
	The 8 bit data DDDDDDDD is placed in the configuration variable identified by CCCC according to the following 
	table. 
		
		CCCC = 0000 - Not available for use 
345  	CCCC = 0010 - Acceleration Value (CV#23) 
		CCCC = 0011 - Deceleration Value (CV#24) 
		CCCC = 1001 – See RP-9.2.3, Appendix B 

		The remaining values of CCCC are reserved and will be selected by the NMRA as need is determined. 
350 
	Only a single packet is necessary to change a configuration variable using this instruction.  If the decoder 
	successfully receives this packet, it shall respond with an operations mode acknowledgment.  
*/
#define DCC_MF_KIND4_CV_SHORT_ACCESS	(0xF0) 

#define DCC_MF_CV_SHORT_MASK			(0x0F) 
#define DCC_MF_CV_SHORT_ACCELERATION	(0x02) 
#define DCC_MF_CV_SHORT_DECELERATION	(0x03) 

/**	
	
	Configuration Variable Access Instruction - Long Form 
	-----------------------------------------------------
	
	The long form allows the direct manipulation of all CVs. Because of the length of this instruction, care must be taken  
	to ensure that the maximum time between packets is not exceeded. This instruction is valid both when the Digital Decoder 
355 has its long address active and short address active. Digital Decoders shall not act on this instruction if sent to its 
	consist address. The format of the instructions using Direct CV addressing is: 
	
			1110CCVV   0   VVVVVVVV   0   DDDDDDDD 
			
360 The actual Configuration Variable desired is selected via the 10-bit address with the 2-bit address (VV) in the first 
	data byte being the most significant bits of the address. The Configuration variable being addressed is the provided 
	10-bit address plus 1. For example, to address CV#1 the 10 bit address is “00 00000000”. 

	The defined values for Instruction type (CC) are:  
365  	CC=00 Reserved for future use 
		CC=01 Verify byte  
 		CC=11 Write byte 
 		CC=10 Bit manipulation 
 		
370 	Type = "01" VERIFY BYTE   
 			The contents of the Configuration Variable as indicated by the 10-bit address are compared with the data 
			byte (DDDDDDDD).  If the decoder successfully receives this packet and the values are identical, the Digital 
			Decoder shall respond with the contents of the CV as the Decoder Response Transmission, if enabled. 
		
375  	Type = "11" WRITE BYTE   
 			The contents of the Configuration Variable as indicated by the 10-bit address are replaced by the data byte 
			(DDDDDDDD).  Two identical packets are needed before the decoder shall modify a configuration variable. Note 
			that CV 17 and CV 18 are a “paired CV”. A “paired CV” refers to a pair of CVs which taken together hold 
			one piece of data.  A WRITE BYTE instruction to CV17 will take effect only when CV18 is written. Other paired 
			CVs will work in a similar manner. See RP-9.2.2 for more information on paired CVs.  
			These two packets need not be back to back on the track.  However any other packet to the same decoder will 
			invalidate the write operation.  (This includes broadcast packets.)  If the decoder successfully receives this second 
380		  	identical packet, it shall respond with a configuration variable access acknowledgment.  

		Type = "10" BIT MANIPULATION 
 			The bit manipulation instructions use a special format for the data byte (DDDDDDDD): 
385    	
				111CDBBB 
                                                          
			Where BBB represents the bit position within the CV, D contains the value of the bit to be verified or written, and C 
			describes whether the operation is a verify bit or a write bit operation. 
			
390         	C = "1" WRITE BIT 
        		C = "0" VERIFY BIT 
        		
			The VERIFY BIT and WRITE BIT instructions operate in a manner similar to the VERIFY BYTE and WRITE 
			BYTE instructions (but operates on a single bit).  Using the same criteria as the VERIFY BYTE instruction, an 
395  		operations mode acknowledgment will be generated in response to a VERIFY BIT instruction if appropriate.  Using 
			the same criteria as the WRITE BYTE instruction, a configuration variable access acknowledgment will be 
			generated in response to the second identical WRITE BIT instruction if appropriate.  
*/
#define DCC_MF_KIND4_CV_LONG_ACCESS		(0xE0) 

#define DCC_CV_LONG_OP_MASK				(0x0C)
#define DCC_CV_VERIFY					(0x04) 
#define DCC_CV_WRITE					(0x0C) 
#define DCC_CV_BIT_OP					(0x08) 

#define DCC_CV_MASK_1					(0x03)
#define DCC_CV_MASK_2					(0xFF)

#define DCC_CV_BIT_OP_MASK				(0x10) 
#define DCC_CV_BIT_VERIFY				(0x00) 
#define DCC_CV_BIT_WRITE				(0x10) 

#define DCC_CV_BIT_VALUE_0				(0x00) 
#define DCC_CV_BIT_VALUE_1				(0x08) 

#define DCC_CV_BIT_MASK					(0x07) 
#define DCC_CV_BIT_0					(0x00) 
#define DCC_CV_BIT_1					(0x01) 
#define DCC_CV_BIT_2					(0x02) 
#define DCC_CV_BIT_3					(0x03) 
#define DCC_CV_BIT_4					(0x04) 
#define DCC_CV_BIT_5					(0x05) 
#define DCC_CV_BIT_6					(0x06) 
#define DCC_CV_BIT_7					(0x07) 

/**	
	D: Accessory Digital Decoder Packet Formats 
	===========================================
	
	Accessory Digital Decoders are intended for control of a number of simple functions such as switch machine 
400 control or turning on and off lights.  It is permissible to develop Digital Decoders that respond to multiple addresses 
	so that more devices can be controlled by a single Digital Decoder. 

	Basic Accessory Decoder Packet Format 
	-------------------------------------
	
	The format for packets intended for Accessory Digital Decoders is: 
	
405  	{preamble}  0  10AAAAAA  0  1AAACDDD  0  EEEEEEEE  1 

	Accessory Digital Decoders can be designed to control momentary or constant-on devices, the duration of time each 
	output is active being controlled by configuration variables CVs #515 through 518.   Bit 3 of the second byte "C" is 
	used to activate or deactivate the addressed device.  (Note if the duration the device is intended to be on is less than 
410 or equal the set duration, no deactivation is necessary.)  Since most devices are paired, the convention is that bit "0" 
	of the second byte is used to distinguish between which of a pair of outputs the accessory decoder is activating or 
	deactivating.  Bits 1 and 2 of byte two are used to indicate which of 4 pairs of outputs the packet is controlling.  The 
	most significant bits of the 9-bit address are bits 4-6 of the second data byte.  By convention these bits (bits 4-6 of 
	the second data byte) are in ones complement.   
415 
	If operations-mode acknowledgement is enabled, receipt of a basic accessory decoder packet must be acknowledged 
	with an operations-mode acknowledgement. 
*/

#define DCC_ACCESSORY_KIND_MASK		(0x80) 
#define DCC_ACCESSORY_KIND_BASIC	(0x80) 
//used to read the packet
#define DCC_ACCESSORY_KIND_EXTENDED	(0x00) 
//used to build the packet
#define DCC_ACCESSORY_EXTENDED		(0x01) 

#define DCC_BA_ADDRESS_MASK_1		(0x3F) 
#define DCC_BA_ADDRESS_MASK_2		(0x70) 
#define DCC_BA_ADDRESS_SHIFT		(2) 

#define DCC_BA_ADDRESS_PAIR_MASK	(0x06) 
#define DCC_BA_ADDRESS_PAIR_SHIFT	(1) 
#define DCC_BA_ADDRESS_PAIR_0		(0x00) 
#define DCC_BA_ADDRESS_PAIR_1		(0x02) 
#define DCC_BA_ADDRESS_PAIR_2		(0x04) 
#define DCC_BA_ADDRESS_PAIR_3		(0x06) 

#define DCC_BA_ADDRESS_OUTPUT_MASK	(0x01) 
#define DCC_BA_ADDRESS_OUTPUT_0		(0x00) 
#define DCC_BA_ADDRESS_OUTPUT_1		(0x01) 

#define DCC_BA_ACTIVATE_MASK		(0x08) 
#define DCC_BA_ACTIVATE				(0x08) 
#define DCC_BA_DEACTIVATE			(0x00) 

/**
	Extended Accessory Decoder Control Packet Format 
	------------------------------------------------
	
	The Extended Accessory Decoder Control Packet is included for the purpose of transmitting aspect control to signal 
420 decoders or data bytes to more complex accessory decoders.  Each signal head can display one aspect at a time. 
	
		{preamble} 0  10AAAAAA 0 0AAA0AA1 0 000XXXXX 0 EEEEEEEE 1 
		
	XXXXX is for a single head. A value of 00000 for XXXXX indicates the absolute stop aspect.  All other aspects 
	represented by the values for XXXXX are determined by the signaling system used and the prototype being 
425 modeled. 

	If operations-mode acknowledgement is enabled, receipt of an extended accessory decoder packet must be 
	acknowledged with an operations-mode acknowledgement. 

*/

#define DCC_EA_ADDRESS_MASK_1		(0x3F) 
#define DCC_EA_ADDRESS_MASK_2		(0x70)
#define DCC_EA_ADDRESS_MASK_3		(0x06) 
#define DCC_EA_ADDRESS_SHIFT_2		(2) 
#define DCC_EA_ADDRESS_SHIFT_3		(8) 

#define DCC_EA_STATE_MASK			(0x1F) 
#define DCC_EA_STATE_STOP			(0x00) 
#define DCC_EA_STATE_MIN			(0x01) 
#define DCC_EA_STATE_MAX			(0x1F) 

/**
	Broadcast Command for Accessory Decoders 
	----------------------------------------
	
430 	Broadcast Command for Basic Accessory Decoders 
		----------------------------------------------
	
		The format for the broadcast instruction is: 
		
			{preamble}  0  10111111  0  1000CDDD  0  EEEEEEEE  1 
		
435 	This packet shall be executed by all accessory decoders.   CDDD is defined as specified in the paragraph 
		on Basic Accessory Decoder Packet Format. 
*/
#define DCC_BA_ADDRESS_BROADCAST		(0x1FF) 
#define DCC_BA_ADDRESS_BROADCAST_1		(0x3F) 
#define DCC_BA_ADDRESS_BROADCAST_2		(0x00) 


/**	
		Broadcast Command for Extended Accessory Decoders 
		-------------------------------------------------
	
		The format for the broadcast instruction is: 
440 
			{preamble}  0  10111111  0  00000111  0  000XXXXX  0  EEEEEEEE  1 
		
		All extended accessory decoders must execute this packet. XXXXX is defined as specified in the 
		paragraph on Extended Accessory Decoder Packet Format. 
*/
#define DCC_EA_ADDRESS_BROADCAST		(0x7FF) 
#define DCC_EA_ADDRESS_BROADCAST_1		(0x3F) 
#define DCC_EA_ADDRESS_BROADCAST_2		(0x00) 
#define DCC_EA_ADDRESS_BROADCAST_3		(0x06) 

/**		
445 
	Accessory Decoder Configuration Variable Access Instruction 
	-----------------------------------------------------------
	
	Accessory decoders can have their Configuration variables changed in the same method as locomotive decoders 
450 using the Configuration Variable Access Instruction - Long Form instruction defined above. For the purpose of 
	this instruction, the accessory decoders’ address is expanded to two bytes in the following method.  If 
	operationsmode acknowledgement is enabled, the receipt of an Accessory Decoder Configuration Variable Access instruction 
	must be acknowledged in the same manner as the Configuration Variable Access Instruction – Long Form. 
	
455 	Basic Accessory Decoder Packet address for operations mode programming 
		----------------------------------------------------------------------

			10AAAAAA 0 1AAACDDD  
		
		Where DDD is used to indicate the output whose CVs are being modified and C=1. 
460  	If CDDD = 0000 then the CVs refer to the entire decoder. The resulting packet would be 

			{preamble}  10AAAAAA  0  1AAACDDD  0  (1110CCVV   0   VVVVVVVV   0   DDDDDDDD)  0  EEEEEEEE 1 
465       			Accessory Decoder Address   (Configuration Variable Access Instruction)    Error Byte
*/
#define DCC_BA_CV_ADDRESS_MASK			(0x0F)
#define DCC_BA_CV_ADDRESS_DECODER 		(0x00)
#define DCC_BA_CV_ADDRESS_OUTPUT 		(0x08)
#define DCC_BA_CV_ADDRESS_P0_O0 		(0x08)
#define DCC_BA_CV_ADDRESS_P0_O1 		(0x09)
#define DCC_BA_CV_ADDRESS_P1_O0 		(0x0A)
#define DCC_BA_CV_ADDRESS_P1_O1 		(0x0B)
#define DCC_BA_CV_ADDRESS_P2_O0 		(0x0C)
#define DCC_BA_CV_ADDRESS_P2_O1 		(0x0D)
#define DCC_BA_CV_ADDRESS_P3_O0 		(0x0E)
#define DCC_BA_CV_ADDRESS_P3_O1 		(0x0F)

#define DCC_BA_KIND4_CV_LONG_ACCESS		(0xE0)

/**        
  		Extended Decoder Control Packet address for operations mode programming 
  		-----------------------------------------------------------------------
  		
			10AAAAAA 0 0AAA0AA1 
470 
		Please note that the use of 0 in bit 3 of byte 2 is to ensure that this packet cannot be confused with the legacy 
		accessory-programming packets. The resulting packet would be: 
		
			{preamble} 10AAAAAA 0 0AAA0AA1 0 (1110CCVV   0   VVVVVVVV   0   DDDDDDDD) 0 EEEEEEEE 1 
475                Signal Decoder Address   (Configuration Variable Access Instruction) Error Byte 

*/

#define DCC_EA_KIND4_CV_LONG_ACCESS		(0xE0)

/**        
	E: Operations Mode Acknowledgment 
	=================================
	
	The operations-mode acknowledgment mechanism as defined in RP-9.3.1 and RP-9.3.2 are the only valid 
	acknowledgement in operations mode.  Whenever an acknowledgment is requested, the decoder shall respond using 
480 this mechanism described in RP-9.3.1 and RP-9.3.2.

	Appendix A. 
	===========

	This Appendix contains additional useful information and/or legacy instructions. A DCC product need not 
	implement any items described in this appendix. 
	
	Accessory Decoder Configuration Variable Access Instruction
	------------------------------------------------------------
	
	For backward compatibility, decoders should check the length of instruction packets when bit 7 of byte 2 is zero.
485 
	The following command is included for backward compatibility for some older accessory decoders.  Its use is 
	discouraged in new decoder designs. 
	
	The format for Accessory Decoder Configuration Variable Access Instructions is:  
490 
		{preamble}  0  10AAAAAA  0  0AAA11VV  0  VVVVVVVV  0  DDDDDDDD  0  EEEEEEEE  1 
	
	Where:  
495  	A = Decoder address bits  
		V = Desired CV address - (CV 513 = 10 00000000)  
		D = Data for CV  
		
	The bit patterns described by VV VVVVVVVV in the second and third bytes and DDDDDDDD in the fourth byte 
500 are also identical to the corresponding bits in the Configuration Variable Access Instruction - Long Form (see RP-9.2.1).  

	The purpose of this instruction is to provide a means of programming all parameters of an accessory decoder after it 
	is installed on the layout. It is recommended that Command Stations exercise caution if changes to the address (CV 513 
505 and CV 521) are allowed.  
                                                          
*/

#endif //__DCC_STANDARD_H__