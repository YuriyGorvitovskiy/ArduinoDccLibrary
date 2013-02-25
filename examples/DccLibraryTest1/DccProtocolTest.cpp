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
#include <DccConfig.h>
#include <DccStandard.h>
#include <DccProtocol.h>
#include <DccCommander.h>
#include <UnitTest.h>

#include "DccProtocolTest.h"

#define TIMER_OUTPUT_MASK         0x7
#define TIMER_OUTPUT_UNKNOWN      0x0
#define TIMER_OUTPUT_SEND_0       0x1
#define TIMER_OUTPUT_SEND_1       0x2
#define TIMER_OUTPUT_CUTOUT_START 0x3
#define TIMER_OUTPUT_CUTOUT_1     0x4
#define TIMER_OUTPUT_CUTOUT_2     0x5
#define TIMER_OUTPUT_WRONG_B      0x7

#define PIN_OUTPUT_MASK           0x8
#define PIN_A_OFF                 0x0
#define PIN_A_ON                  0x8

#define STATISTICS_SIZE_MAX   (DCC_PREAMBULE_SIZE + 1 + 6*9 + 2 + DCC_PREAMBULE_SIZE + 1)

byte  statistics[STATISTICS_SIZE_MAX];

DccPacket DccProtocolTest::TEST;

byte DccProtocolTest::readState() {
    byte    v = digitalRead(DCC_PIN_OUT_A) ? PIN_A_ON : PIN_A_OFF;
    boolean same = (digitalRead(DCC_PIN_OUT_B) == digitalRead(DCC_PIN_OUT_A));
    switch(OCR1A) {
        case TIMER_COUNT_SEND_0:       v |= (same ? TIMER_OUTPUT_WRONG_B : TIMER_OUTPUT_SEND_0);       break;
        case TIMER_COUNT_SEND_1:       v |= (same ? TIMER_OUTPUT_WRONG_B : TIMER_OUTPUT_SEND_1);       break;
        case TIMER_COUNT_CUTOUT_START: v |= (same ? TIMER_OUTPUT_WRONG_B : TIMER_OUTPUT_CUTOUT_START); break;
        case TIMER_COUNT_CUTOUT_END_1: v |= (same ? TIMER_OUTPUT_CUTOUT_1 : TIMER_OUTPUT_WRONG_B);     break;
        case TIMER_COUNT_CUTOUT_END_2: v |= (same ? TIMER_OUTPUT_CUTOUT_2 : TIMER_OUTPUT_WRONG_B);     break;
        default:                       v |= TIMER_OUTPUT_UNKNOWN;                                      break;
    }
    return v;
}

void DccProtocolTest::execute(DccPacket* p) {
        
    DccCmd.resetQueue();
    DccRails.startTest();
    DccCmd.send(p);      
    
    memset(statistics, 0, STATISTICS_SIZE_MAX);
    
    byte* currentByte = statistics;
    int count = DCC_PREAMBULE_SIZE + 1 + p->size() * 9 + DCC_PREAMBULE_SIZE + 1;
    if (p->hasAcknowledge())
        count += 1;

    for(int i = 0 ; i < count; ++i, ++currentByte) {
        DccRails.timerInterrupt();
        *currentByte = (readState() << 4);        
        DccRails.timerInterrupt();
        *currentByte |= readState();        
    }
}

boolean DccProtocolTest::checkBit(boolean expected, byte statistic) {
    byte first = (statistic >> 4);
    byte second = (statistic & 0x0F);
    if ((first & PIN_OUTPUT_MASK) == (second & PIN_OUTPUT_MASK))
        return false;
    if ((first & TIMER_OUTPUT_MASK) != (expected ? TIMER_OUTPUT_SEND_1 : TIMER_OUTPUT_SEND_0))
        return false;
    if ((second & TIMER_OUTPUT_MASK) != (expected ? TIMER_OUTPUT_SEND_1 : TIMER_OUTPUT_SEND_0))
        return false;
    return true;    

}
boolean DccProtocolTest::checkPreambule(byte*& from) {
    for (int si = 0; si < DCC_PREAMBULE_SIZE; ++si) {
        if (!checkBit(1, *from++)) 
            return false;
    }
    // do not step forward. Next 0 bit is treated as a packet bit
    if (!checkBit(0, *from))
        return false;
    return true;    
}

boolean DccProtocolTest::checkData(byte*& from, DccPacket* p) {
    for(byte pi = 0; pi < p->size(); ++pi) {
        if (!checkBit(0, *from++))
            return false;

        for (byte bi = 0x80; bi > 0; bi >>= 1) {
            if (!checkBit(p->dcc_data[pi] & bi, *from++))
                return false;
        }
    }
    if (!checkBit(1, *from++))
        return false;
        
    return true;
}

boolean DccProtocolTest::checkAcknowledge(byte*& from, boolean isShort) {
    byte prev = (*(from-1) & 0x0F);
    byte first = (*from >> 4);
    byte second = (*from & 0x0F);
    ++from;
    if ((first & PIN_OUTPUT_MASK) == (prev & PIN_OUTPUT_MASK))
        return false;
    if ((first & TIMER_OUTPUT_MASK) != TIMER_OUTPUT_CUTOUT_START)
        return false;
    if ((second & TIMER_OUTPUT_MASK) != (isShort ? TIMER_OUTPUT_CUTOUT_1 : TIMER_OUTPUT_CUTOUT_2))
        return false;
        
    return true;
}

void DccProtocolTest::test3BytesCommand() {
    UnitTest::start();
        
    TEST.dcc_info = DCC_INFO_SIZE_3 | DCC_INFO_NO_ACKNOWLEDGE | DCC_INFO_NO_REPEAT;
    TEST.dcc_data[0] = 0x12;
    TEST.dcc_data[1] = 0x34;
    TEST.dcc_data[2] = 0x56;
    
    execute(&TEST);
    
    byte* from = statistics;
    ASSERT(checkPreambule(from));
    ASSERT(checkData(from, &TEST));
    ASSERT(checkPreambule(from));
}

void DccProtocolTest::test4BytesCommand() {
    UnitTest::start();
        
    TEST.dcc_info = DCC_INFO_SIZE_4 | DCC_INFO_NO_ACKNOWLEDGE | DCC_INFO_NO_REPEAT;
    TEST.dcc_data[0] = 0x12;
    TEST.dcc_data[1] = 0x34;
    TEST.dcc_data[2] = 0x56;
    TEST.dcc_data[3] = 0x78;    

    execute(&TEST);
    
    byte* from = statistics;
    ASSERT(checkPreambule(from));
    ASSERT(checkData(from, &TEST));
    ASSERT(checkPreambule(from));
}

void DccProtocolTest::test5BytesCommand() {
    UnitTest::start();
        
    TEST.dcc_info = DCC_INFO_SIZE_5 | DCC_INFO_NO_ACKNOWLEDGE | DCC_INFO_NO_REPEAT;
    TEST.dcc_data[0] = 0x12;
    TEST.dcc_data[1] = 0x34;
    TEST.dcc_data[2] = 0x56;
    TEST.dcc_data[3] = 0x78;    
    TEST.dcc_data[4] = 0x9A;    

    execute(&TEST);
    
    byte* from = statistics;
    ASSERT(checkPreambule(from));
    ASSERT(checkData(from, &TEST));
    ASSERT(checkPreambule(from));
}

void DccProtocolTest::test6BytesCommand() {
    UnitTest::start();
        
    TEST.dcc_info = DCC_INFO_SIZE_6 | DCC_INFO_NO_ACKNOWLEDGE | DCC_INFO_NO_REPEAT;
    TEST.dcc_data[0] = 0x12;
    TEST.dcc_data[1] = 0x34;
    TEST.dcc_data[2] = 0x56;
    TEST.dcc_data[3] = 0x78;    
    TEST.dcc_data[4] = 0x9A;    
    TEST.dcc_data[5] = 0xBC;    

    execute(&TEST);
    
    byte* from = statistics;
    ASSERT(checkPreambule(from));
    ASSERT(checkData(from, &TEST));
    ASSERT(checkPreambule(from));
}

void DccProtocolTest::test1BytesAcknowledge() {
    UnitTest::start();

    TEST.dcc_info = DCC_INFO_SIZE_3 | DCC_INFO_ACKNOWLEDGE_1 | DCC_INFO_NO_REPEAT;
    TEST.dcc_data[0] = 0x12;
    TEST.dcc_data[1] = 0x34;
    TEST.dcc_data[2] = 0x56;

    execute(&TEST);
    
    byte* from = statistics;
    ASSERT(checkPreambule(from));
    ASSERT(checkData(from, &TEST));
    ASSERT(checkAcknowledge(from, true));
    ASSERT(checkPreambule(from));
}

void DccProtocolTest::test2BytesAcknowledge() {
    UnitTest::start();
        
    TEST.dcc_info = DCC_INFO_SIZE_6 | DCC_INFO_ACKNOWLEDGE_2 | DCC_INFO_NO_REPEAT;
    TEST.dcc_data[0] = 0x12;
    TEST.dcc_data[1] = 0x34;
    TEST.dcc_data[2] = 0x56;
    TEST.dcc_data[3] = 0x78;    
    TEST.dcc_data[4] = 0x9A;    
    TEST.dcc_data[5] = 0xBC;    
    
    execute(&TEST);
    
    byte* from = statistics;
    ASSERT(checkPreambule(from));
    ASSERT(checkData(from, &TEST));
    ASSERT(checkAcknowledge(from, false));
    ASSERT(checkPreambule(from));
}

boolean DccProtocolTest::testAll() {
    UnitTest::suite("DccProtocol");
  
    test3BytesCommand();
    test4BytesCommand();
    test5BytesCommand();
    test6BytesCommand();
    test1BytesAcknowledge();
    test2BytesAcknowledge();
    
    return UnitTest::report();
}



