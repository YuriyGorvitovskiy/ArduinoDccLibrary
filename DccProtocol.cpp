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
#include "DccProtocol.h"
#include "DccCommander.h"
#include "DccStandard.h"

#define  STATE_POWER_OFF       (0)
#define  STATE_PREAMBULE       (1)
#define  STATE_SEND_BYTE       (2)
#define  STATE_BYTE_START_BIT  (3)
#define  STATE_PACKET_END_BIT  (4)
#define  STATE_CUTOUT_WAIT     (5)
#define  STATE_CUTOUT_RUN      (6)

DccProtocol DccRails;

void DccProtocol::setup() {
    state = STATE_POWER_OFF;
    packet = NULL;

    // initialize pins 
    pinMode(DCC_PIN_OUT_A, OUTPUT);
    pinMode(DCC_PIN_OUT_B, OUTPUT);
	
	configureTimer();
}

void DccProtocol::startTest() {
	configureTimer();
	disableTimer();
	DccCmd.returnBack(packet);
	packet = NULL;

    pinMode(DCC_PIN_OUT_A, OUTPUT);
    pinMode(DCC_PIN_OUT_B, OUTPUT);

	state = STATE_CUTOUT_RUN;
	dcc_positive = true;
}


void DccProtocol::power(boolean on) {
	if (on) {
    	if (state != STATE_POWER_OFF)
        	return;
		state = STATE_CUTOUT_RUN;
		enableTimer();
	} else {
		disableTimer();
    	state = STATE_POWER_OFF;
		digitalWrite(DCC_PIN_OUT_A, LOW);
		digitalWrite(DCC_PIN_OUT_B, LOW);
		
		DccCmd.returnBack(packet);
		packet = NULL;
	}
}

boolean DccProtocol::power() {
	return state == STATE_POWER_OFF;
}

void DccProtocol::configureTimer() {
    // Initialize Timer/Counter Control Register http://www.atmel.com/Images/doc8161.pdf
    
    /* CTC mode: Clear timer on compare match. 
     *           When the timer counter reaches the compare match register, the timer will be cleared.
     *
     * (WGM13, WGM12, WGM11, WGM10) = (0, 1, 0, 0) <==> CTC mode. 
     * (CS12, CS11, CS10)           = (0, 1, 0)    <==> Prescalar = 8
     * (COM1A1, COM1A0)             = (0, 0)       <==> Normal port operation, OC1A disconnected from timer. 
     * (COM1B1, COM1B0)             = (0, 0)       <==> Normal port operation, OC1B disconnected from timer. 
     * (FOC1A)                      = (0)          <==> Force Output Compare for Channel A - OFF
     * (FOC1B)                      = (0)          <==> Force Output Compare for Channel B - OFF
     * (ICNC1)                      = (0)          <==> Input Capture Noise Canceler - OFF
     * (ICES1)                      = (0)          <==> Input Capture Edge Select - OFF
     */
    TCCR1A = (0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
    TCCR1B = (0<<ICNC1)  | (0<<ICES1)  | (0<<WGM13)  | (1<<WGM12)  | (0<<CS12)  | (1<<CS11) | (0<<CS10);
    TCCR1C = (0<<FOC1A)  | (0<<FOC1A);

    //Timer Counter set up to output 1
    OCR1A = TIMER_COUNT_SEND_1;
}

void DccProtocol::enableTimer() {
	// Reset timer counter
	TCNT1 = 0;
	OCR1A = TIMER_COUNT_SEND_1;
	dcc_positive = true;
        
    /*
     * TIMSK1 – Timer/Counter1 Interrupt Mask Register
     * (ICIE1)  = (0) <==> Timer/Counter1, Input Capture Interrupt Enable - OFF
     * (OCIE1B) = (0) <==> Timer/Counter1, Output Compare B Match Interrupt Enable - OFF
     * (OCIE1A) = (1) <==> Timer/Counter1, Output Compare A Match Interrupt Enable - ON
     * (TOIE1)  = (0) <==> Timer/Counter1, Overflow Interrupt Enable - OFF
     */
    TIMSK1 = (0 << ICIE1) | (0 << OCIE1B) | (1 << OCIE1A) | (0 << TOIE1);
}

void DccProtocol::disableTimer() {
	// Reset timer counter
	TCNT1 = 0;
	
    /*
     * TIMSK1 – Timer/Counter1 Interrupt Mask Register
     * (ICIE1)  = (0) <==> Timer/Counter1, Input Capture Interrupt Enable - OFF
     * (OCIE1B) = (0) <==> Timer/Counter1, Output Compare B Match Interrupt Enable - OFF
     * (OCIE1A) = (0) <==> Timer/Counter1, Output Compare A Match Interrupt Enable - OFF
     * (TOIE1)  = (0) <==> Timer/Counter1, Overflow Interrupt Enable - OFF
     */
    TIMSK1 = (0 << ICIE1) | (0 << OCIE1B) | (0 << OCIE1A) | (0 << TOIE1);
}

void DccProtocol::timerInterrupt() {
    if (state == STATE_CUTOUT_WAIT) {
        digitalWrite(DCC_PIN_OUT_A, LOW);
        digitalWrite(DCC_PIN_OUT_B, LOW);
        
        state = STATE_CUTOUT_RUN;
        OCR1A = packet->isAcknowledgeShort() ? TIMER_COUNT_CUTOUT_END_1 : TIMER_COUNT_CUTOUT_END_2;
        
        dcc_positive = true; // to be sure that we come to switch after cutout
        return;
    } 
    digitalWrite(DCC_PIN_OUT_A, dcc_positive ? LOW : HIGH);
    digitalWrite(DCC_PIN_OUT_B, dcc_positive ? HIGH : LOW);

    dcc_positive = !dcc_positive;
    
    if (dcc_positive) 
        return;
    
    switch(state) {
        case STATE_PREAMBULE: 
            if (--current_bit)
                return;
                
            packet       = DccCmd.nextPacketToSend(packet);
            current_byte = packet->dcc_data;
            end_byte     = current_byte + packet->size();
            current_bit  = 0x80;
            state = STATE_BYTE_START_BIT;
            OCR1A = TIMER_COUNT_SEND_0;
            return;
            
        case STATE_BYTE_START_BIT:
            state = STATE_SEND_BYTE;
            current_bit = 0x80;
            //No return intentiosionally to follow into case STATE_SEND_BYTE;
        case STATE_SEND_BYTE:
            if (current_bit) {
                OCR1A = ((*current_byte) & current_bit) ? TIMER_COUNT_SEND_1 : TIMER_COUNT_SEND_0;
                current_bit >>= 1;
                return;
            }
            if (++current_byte == end_byte) {
                state = STATE_PACKET_END_BIT;
                OCR1A = TIMER_COUNT_SEND_1;
                return;
            }
            state = STATE_BYTE_START_BIT;
            OCR1A = TIMER_COUNT_SEND_0;
            return;    
            
        case STATE_PACKET_END_BIT:
            if (packet->hasAcknowledge()) {
                state = STATE_CUTOUT_WAIT;
                OCR1A = TIMER_COUNT_CUTOUT_START;
                return;
            }
         //No return intentionally to follow into case STATE_CUTOUT_RUN;
        case STATE_CUTOUT_RUN:
            current_bit = DCC_PREAMBULE_SIZE;
            state = STATE_PREAMBULE;
            OCR1A = TIMER_COUNT_SEND_1;
            return;
    }
}

// This is the Interrupt Service Routine (ISR) for Timer1 compare match.
ISR(TIMER1_COMPA_vect) {
    DccRails.timerInterrupt();
}
