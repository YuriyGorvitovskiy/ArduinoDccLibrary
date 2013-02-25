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
#include <DccPacket.h>
#include <UnitTest.h>

#include "DccPacketTest.h"

void DccPacketTest::testDataSizeBits() {
    UnitTest::start();

    ASSERT(UnitTest::bits_in_mask(DCC_INFO_SIZE_3, DCC_INFO_SIZE_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_SIZE_4, DCC_INFO_SIZE_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_SIZE_5, DCC_INFO_SIZE_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_SIZE_6, DCC_INFO_SIZE_MASK));

    ASSERT( UnitTest::bits_in_mask(1<<DCC_INFO_SIZE_SHIFT,     DCC_INFO_SIZE_MASK));
    ASSERT(!UnitTest::bits_in_mask(1<<(DCC_INFO_SIZE_SHIFT-1), DCC_INFO_SIZE_MASK));
    
    byte values[] = {
        DCC_INFO_SIZE_3,
        DCC_INFO_SIZE_4,
        DCC_INFO_SIZE_5,
        DCC_INFO_SIZE_6
    };
    ASSERT(UnitTest::unique_values(values, sizeof(values)));
    
    DccPacket TEST;
    TEST.dcc_info = DCC_INFO_SIZE_3 | (0xFF & ~DCC_INFO_SIZE_MASK);
    ASSERT(TEST.size() == 3);
    
    TEST.dcc_info = DCC_INFO_SIZE_4 | (0xFF & ~DCC_INFO_SIZE_MASK);
    ASSERT(TEST.size() == 4);
    
    TEST.dcc_info = DCC_INFO_SIZE_5 | (0xFF & ~DCC_INFO_SIZE_MASK);
    ASSERT(TEST.size() == 5);
    
    TEST.dcc_info = DCC_INFO_SIZE_6 | (0xFF & ~DCC_INFO_SIZE_MASK);
    ASSERT(TEST.size() == 6);
}

void DccPacketTest::testAcknowledgeBits() {
    UnitTest::start();

    ASSERT(UnitTest::bits_in_mask(DCC_INFO_NO_ACKNOWLEDGE,       DCC_INFO_ACKNOWLEDGE_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_NO_ACKNOWLEDGE_WAIT,  DCC_INFO_ACKNOWLEDGE_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_ACKNOWLEDGE_1,        DCC_INFO_ACKNOWLEDGE_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_ACKNOWLEDGE_2,        DCC_INFO_ACKNOWLEDGE_MASK));
    
    byte values[] = {
        DCC_INFO_NO_ACKNOWLEDGE,
        DCC_INFO_NO_ACKNOWLEDGE_WAIT,
        DCC_INFO_ACKNOWLEDGE_1,
        DCC_INFO_ACKNOWLEDGE_2
    };
    ASSERT(UnitTest::unique_values(values, sizeof(values)));
    
    DccPacket TEST;
    TEST.dcc_info = DCC_INFO_NO_ACKNOWLEDGE | (0xFF & ~(DCC_INFO_ACKNOWLEDGE_MASK));
    ASSERT(!TEST.hasToWait());
    ASSERT(!TEST.hasAcknowledge());

    TEST.dcc_info = DCC_INFO_NO_ACKNOWLEDGE_WAIT | (0xFF & ~(DCC_INFO_ACKNOWLEDGE_MASK));
    ASSERT( TEST.hasToWait());
    ASSERT(!TEST.hasAcknowledge());
    
    TEST.dcc_info = DCC_INFO_ACKNOWLEDGE_1 | (0xFF & ~(DCC_INFO_ACKNOWLEDGE_MASK));
    ASSERT(TEST.hasToWait());
    ASSERT(TEST.hasAcknowledge());
    ASSERT(TEST.isAcknowledgeShort());

    TEST.dcc_info = DCC_INFO_ACKNOWLEDGE_2 | (0xFF & ~(DCC_INFO_ACKNOWLEDGE_MASK));
    ASSERT( TEST.hasToWait());
    ASSERT( TEST.hasAcknowledge());
    ASSERT(!TEST.isAcknowledgeShort());
}

void DccPacketTest::testRepeatBits() {
    UnitTest::start();
    
    byte masks[] = {
        DCC_INFO_SIZE_MASK,
        DCC_INFO_ACKNOWLEDGE_MASK,
        DCC_INFO_REPEAT_MASK
    };
    ASSERT(UnitTest::unique_bits(masks, sizeof(masks)));
    
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_NO_REPEAT,   DCC_INFO_REPEAT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_REPEAT_1,    DCC_INFO_REPEAT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_REPEAT_2,    DCC_INFO_REPEAT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_REPEAT_3,    DCC_INFO_REPEAT_MASK)); //5
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_REPEAT_4,    DCC_INFO_REPEAT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_REPEAT_5,    DCC_INFO_REPEAT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_REPEAT_6,    DCC_INFO_REPEAT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_REPEAT_7,    DCC_INFO_REPEAT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_INFO_REPEAT_MAX,  DCC_INFO_REPEAT_MASK)); //10

    byte values[] = {
        DCC_INFO_NO_REPEAT,
        DCC_INFO_REPEAT_1,
        DCC_INFO_REPEAT_2,
        DCC_INFO_REPEAT_3,
        DCC_INFO_REPEAT_4,
        DCC_INFO_REPEAT_5,
        DCC_INFO_REPEAT_6,
        DCC_INFO_REPEAT_7,
        DCC_INFO_REPEAT_MAX
    };
    ASSERT(UnitTest::unique_values(values, sizeof(values)));
    
    DccPacket TEST;
    TEST.dcc_info = DCC_INFO_NO_REPEAT | (0xFF & ~DCC_INFO_REPEAT_MASK);
    ASSERT(TEST.repeat()          == 0);
    ASSERT(TEST.decrementRepeat() == 0);
    ASSERT((TEST.dcc_info & DCC_INFO_REPEAT_MASK) == DCC_INFO_NO_REPEAT);
    ASSERT((TEST.dcc_info | DCC_INFO_REPEAT_MASK) == 0xFF);                      //15
    
    TEST.dcc_info = DCC_INFO_REPEAT_1 | (0xFF & ~DCC_INFO_REPEAT_MASK);
    ASSERT(TEST.repeat()          == 1);
    ASSERT(TEST.decrementRepeat() == 0);
    ASSERT((TEST.dcc_info & DCC_INFO_REPEAT_MASK) == DCC_INFO_NO_REPEAT);
    ASSERT((TEST.dcc_info | DCC_INFO_REPEAT_MASK) == 0xFF);

    TEST.dcc_info = DCC_INFO_REPEAT_2 | (0xFF & ~DCC_INFO_REPEAT_MASK);
    ASSERT(TEST.repeat()          == 2);                                         //20
    ASSERT(TEST.decrementRepeat() == 1);
    ASSERT((TEST.dcc_info & DCC_INFO_REPEAT_MASK) == DCC_INFO_REPEAT_1);
    ASSERT((TEST.dcc_info | DCC_INFO_REPEAT_MASK) == 0xFF);

    TEST.dcc_info = DCC_INFO_REPEAT_3 | (0xFF & ~DCC_INFO_REPEAT_MASK);
    ASSERT(TEST.repeat()          == 3);
    ASSERT(TEST.decrementRepeat() == 2);                                         //25
    ASSERT((TEST.dcc_info & DCC_INFO_REPEAT_MASK) == DCC_INFO_REPEAT_2);
    ASSERT((TEST.dcc_info | DCC_INFO_REPEAT_MASK) == 0xFF);

    TEST.dcc_info = DCC_INFO_REPEAT_4 | (0xFF & ~DCC_INFO_REPEAT_MASK);
    ASSERT(TEST.repeat()          == 4);
    ASSERT(TEST.decrementRepeat() == 3);
    ASSERT((TEST.dcc_info & DCC_INFO_REPEAT_MASK) == DCC_INFO_REPEAT_3);         //30
    ASSERT((TEST.dcc_info | DCC_INFO_REPEAT_MASK) == 0xFF);

    TEST.dcc_info = DCC_INFO_REPEAT_5 | (0xFF & ~DCC_INFO_REPEAT_MASK);
    ASSERT(TEST.repeat()          == 5);
    ASSERT(TEST.decrementRepeat() == 4);
    ASSERT((TEST.dcc_info & DCC_INFO_REPEAT_MASK) == DCC_INFO_REPEAT_4);
    ASSERT((TEST.dcc_info | DCC_INFO_REPEAT_MASK) == 0xFF);                      //35

    TEST.dcc_info = DCC_INFO_REPEAT_6 | (0xFF & ~DCC_INFO_REPEAT_MASK);
    ASSERT(TEST.repeat()          == 6);
    ASSERT(TEST.decrementRepeat() == 5);
    ASSERT((TEST.dcc_info & DCC_INFO_REPEAT_MASK) == DCC_INFO_REPEAT_5);
    ASSERT((TEST.dcc_info | DCC_INFO_REPEAT_MASK) == 0xFF);

    TEST.dcc_info = DCC_INFO_REPEAT_7 | (0xFF & ~DCC_INFO_REPEAT_MASK);    
    ASSERT(TEST.repeat()          == 7);                                         //40
    ASSERT(TEST.decrementRepeat() == 6);
    ASSERT((TEST.dcc_info & DCC_INFO_REPEAT_MASK) == DCC_INFO_REPEAT_6);
    ASSERT((TEST.dcc_info | DCC_INFO_REPEAT_MASK) == 0xFF);

    TEST.dcc_info = DCC_INFO_REPEAT_MAX | (0xFF & ~DCC_INFO_REPEAT_MASK);
    ASSERT(TEST.repeat()          == DCC_INFO_REPEAT_MAX);
    ASSERT(TEST.decrementRepeat() == DCC_INFO_REPEAT_MAX - 1);
    ASSERT((TEST.dcc_info & DCC_INFO_REPEAT_MASK) == DCC_INFO_REPEAT_MAX - 1);   //45
    ASSERT((TEST.dcc_info | DCC_INFO_REPEAT_MASK) == 0xFF);
}

void DccPacketTest::testAddressBits() {
    UnitTest::start();

    DccPacket TEST;

    TEST.dcc_data[0]= 0x00;
    TEST.dcc_data[1]= 0xFF;
    ASSERT( TEST.isMultiFunction());
    ASSERT( TEST.isMultiFunctionBroadcast());
    ASSERT( TEST.isAddressShort());
    ASSERT(!TEST.isAccessory());
    ASSERT(!TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT(!TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());            //8

    TEST.dcc_data[0]= 0x01;
    TEST.dcc_data[1]= 0xFF;
    ASSERT( TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT( TEST.isAddressShort());
    ASSERT(!TEST.isAccessory());
    ASSERT(!TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT(!TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());           //16

    TEST.dcc_data[0]= 0x33;
    TEST.dcc_data[1]= 0x0F;
    ASSERT( TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT( TEST.isAddressShort());
    ASSERT(!TEST.isAccessory());
    ASSERT(!TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT(!TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());          //24

    TEST.dcc_data[0]= 0x7F;
    TEST.dcc_data[1]= 0xFF;
    ASSERT( TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT( TEST.isAddressShort());
    ASSERT(!TEST.isAccessory());
    ASSERT(!TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT(!TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());          //32

    TEST.dcc_data[0]= 0xC0;
    TEST.dcc_data[1]= 0x00;
    ASSERT( TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT(!TEST.isAddressShort());
    ASSERT(!TEST.isAccessory());
    ASSERT(!TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT(!TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());          //40

    TEST.dcc_data[0]= 0xDA;
    TEST.dcc_data[1]= 0xC5;
    ASSERT( TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT(!TEST.isAddressShort());
    ASSERT(!TEST.isAccessory());
    ASSERT(!TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT(!TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());          //48

    TEST.dcc_data[0]= 0xE7;
    TEST.dcc_data[1]= 0xFF;
    ASSERT( TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT(!TEST.isAddressShort());
    ASSERT(!TEST.isAccessory());
    ASSERT(!TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT(!TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());          //56

    TEST.dcc_data[0]= 0x80;
    TEST.dcc_data[1]= 0xFF;
    ASSERT(!TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT(!TEST.isAddressShort());
    ASSERT( TEST.isAccessory());
    ASSERT( TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT(!TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());          //64

    TEST.dcc_data[0]= 0xA0;
    TEST.dcc_data[1]= 0xF0;
    ASSERT(!TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT(!TEST.isAddressShort());
    ASSERT( TEST.isAccessory());
    ASSERT( TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT(!TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());          //72

    TEST.dcc_data[0]= 0xBE;
    TEST.dcc_data[1]= 0x80;
    ASSERT(!TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT(!TEST.isAddressShort());
    ASSERT( TEST.isAccessory());
    ASSERT( TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT(!TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());          //80

    TEST.dcc_data[0]= 0xBF;
    TEST.dcc_data[1]= 0x80;
    ASSERT(!TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT(!TEST.isAddressShort());
    ASSERT( TEST.isAccessory());
    ASSERT( TEST.isBasicAccessory());
    ASSERT( TEST.isBasicAccessoryBroadcast());
    ASSERT(!TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());          //88

    TEST.dcc_data[0]= 0x80;
    TEST.dcc_data[1]= 0x7F;
    ASSERT(!TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT(!TEST.isAddressShort());
    ASSERT( TEST.isAccessory());
    ASSERT(!TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT( TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());          //96

    TEST.dcc_data[0]= 0xA0;
    TEST.dcc_data[1]= 0x7F;
    ASSERT(!TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT(!TEST.isAddressShort());
    ASSERT( TEST.isAccessory());
    ASSERT(!TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT( TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());          //104

    TEST.dcc_data[0]= 0xBE;
    TEST.dcc_data[1]= 0x0F;
    ASSERT(!TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT(!TEST.isAddressShort());
    ASSERT( TEST.isAccessory());
    ASSERT(!TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT( TEST.isExtendedAccessory());
    ASSERT(!TEST.isExtendedAccessoryBroadcast());          //112

    TEST.dcc_data[0]= 0xBF;
    TEST.dcc_data[1]= 0x07;
    ASSERT(!TEST.isMultiFunction());
    ASSERT(!TEST.isMultiFunctionBroadcast());
    ASSERT(!TEST.isAddressShort());
    ASSERT( TEST.isAccessory());
    ASSERT(!TEST.isBasicAccessory());
    ASSERT(!TEST.isBasicAccessoryBroadcast());
    ASSERT( TEST.isExtendedAccessory());
    ASSERT( TEST.isExtendedAccessoryBroadcast());          //120
}

void DccPacketTest::testMultiFunctionBuilds() {
    UnitTest::start();

    DccPacket TEST;

    DccPacket* p = TEST.idle();
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == 0);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xFF);                    //5
    ASSERT( TEST.dcc_data[1] == 0x00);
    ASSERT( TEST.dcc_data[2] == 0xFF);
    
    p = TEST.mfBroadcast().speed14(true, 0);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);           //10
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x00);
    ASSERT( TEST.dcc_data[1] == 0x60);
    ASSERT( TEST.dcc_data[2] == 0x60);

    p = TEST.mfAddress7(0x34).speed14(false, 1);
    ASSERT( p == &TEST);                                  //15
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x34);
    ASSERT( TEST.dcc_data[1] == 0x41);                    //20
    ASSERT( TEST.dcc_data[2] == 0x75);

    p = TEST.mfAddress14(0x57).speed14(true, 0xE);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_SPEED);
    ASSERT(!TEST.hasAcknowledge());                       //25
    ASSERT( TEST.dcc_data[0] == 0xC0);
    ASSERT( TEST.dcc_data[1] == 0x57);
    ASSERT( TEST.dcc_data[2] == 0x6E);
    ASSERT( TEST.dcc_data[3] == 0xF9);

    p = TEST.mfBroadcast().speed28(true, 0);
    ASSERT( p == &TEST);                                  //30
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x00);
    ASSERT( TEST.dcc_data[1] == 0x60);                    //35
    ASSERT( TEST.dcc_data[2] == 0x60);

    p = TEST.mfAddress7(0x34).speed28(false, 2);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);
    ASSERT(!TEST.hasAcknowledge());                       //40
    ASSERT( TEST.dcc_data[0] == 0x34);
    ASSERT( TEST.dcc_data[1] == 0x41);
    ASSERT( TEST.dcc_data[2] == 0x75);

    p = TEST.mfAddress14(0x57).speed28(true, 0x15);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);                            //45
    ASSERT( TEST.repeat() == DCC_REPEAT_SPEED);           
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xC0);
    ASSERT( TEST.dcc_data[1] == 0x57);
    ASSERT( TEST.dcc_data[2] == 0x7A);                    //50
    ASSERT( TEST.dcc_data[3] == 0xED);                   


    p = TEST.mfBroadcast().speed128(false, 0);
    ASSERT( p == &TEST);                                  
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);
    ASSERT(!TEST.hasAcknowledge());                       //55
    ASSERT( TEST.dcc_data[0] == 0x00);
    ASSERT( TEST.dcc_data[1] == 0x3F);                    
    ASSERT( TEST.dcc_data[2] == 0x00);                    
    ASSERT( TEST.dcc_data[3] == 0x3F);

    p = TEST.mfAddress7(0x12).speed128(true, 1);
    ASSERT( p == &TEST);                                  //60
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);
    ASSERT(!TEST.hasAcknowledge());                       
    ASSERT( TEST.dcc_data[0] == 0x12);
    ASSERT( TEST.dcc_data[1] == 0x3F);                    //65
    ASSERT( TEST.dcc_data[2] == 0x81);
    ASSERT( TEST.dcc_data[3] == 0xAC);

    p = TEST.mfAddress14(0x2345).speed128(false, 0x15);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 5);         
    ASSERT( TEST.repeat() == DCC_REPEAT_SPEED);          //70
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xE3);
    ASSERT( TEST.dcc_data[1] == 0x45);                    
    ASSERT( TEST.dcc_data[2] == 0x3F);
    ASSERT( TEST.dcc_data[3] == 0x15);                   //75
    ASSERT( TEST.dcc_data[4] == 0x8C);                    

    p = TEST.mfBroadcast().functionF0_F4(true, true, true, true, true);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());                     //80
    ASSERT( TEST.dcc_data[0] == 0x00);
    ASSERT( TEST.dcc_data[1] == 0x9F);                    
    ASSERT( TEST.dcc_data[2] == 0x9F);

    p = TEST.mfAddress7(0x1).functionF5_F8(true, false, true, false);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);                         //85
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x01);
    ASSERT( TEST.dcc_data[1] == 0xB5);                    
    ASSERT( TEST.dcc_data[2] == 0xB4);                 //90

    p = TEST.mfAddress14(0x7F).functionF9_F12(false, true, false, true);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xC0);                //95
    ASSERT( TEST.dcc_data[1] == 0x7F);                
    ASSERT( TEST.dcc_data[2] == 0xAA);                    
    ASSERT( TEST.dcc_data[3] == 0x15);
	
    p = TEST.mfAddress7(0x7F).functionF13_F20(false, true, false, true, false, true, false, true);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);                        //100
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x7F);
    ASSERT( TEST.dcc_data[1] == 0xDE);
    ASSERT( TEST.dcc_data[2] == 0xAA);                //105
    ASSERT( TEST.dcc_data[3] == 0x0B);


    p = TEST.mfAddress14(0x0).functionF21_F28(true, false, true, false, true, false, true, false);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 5);
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());                   //110
    ASSERT( TEST.dcc_data[0] == 0xC0);
    ASSERT( TEST.dcc_data[1] == 0x00);
    ASSERT( TEST.dcc_data[2] == 0xDF);
    ASSERT( TEST.dcc_data[3] == 0x55);
    ASSERT( TEST.dcc_data[4] == 0x4A);                //115
    
    
    p = TEST.mfAddress7(0x7F).functionF13_F20(0xAA);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);                        
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x7F);                //120
    ASSERT( TEST.dcc_data[1] == 0xDE);
    ASSERT( TEST.dcc_data[2] == 0xAA);                
    ASSERT( TEST.dcc_data[3] == 0x0B);


    p = TEST.mfAddress14(0x0).functionF21_F28(0x55);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 5);                        //125
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());                   
    ASSERT( TEST.dcc_data[0] == 0xC0);
    ASSERT( TEST.dcc_data[1] == 0x00);
    ASSERT( TEST.dcc_data[2] == 0xDF);                //130
    ASSERT( TEST.dcc_data[3] == 0x55);
    ASSERT( TEST.dcc_data[4] == 0x4A);        
    
}

void DccPacketTest::testMultiFunctionParsing() {
    UnitTest::start();

    DccPacket TEST;

    DccPacket* p = TEST.parseDccHexCommand("00FF00");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == 0);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xFF);                    //5
    ASSERT( TEST.dcc_data[1] == 0x00);
    ASSERT( TEST.dcc_data[2] == 0xFF);
    
    p = TEST.parseDccTextCommand("mf");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);           //10
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x00);
    ASSERT( TEST.dcc_data[1] == 0x60);
    ASSERT( TEST.dcc_data[2] == 0x60);

    p = TEST.parseDccTextCommand("m52r2");
    ASSERT( p == &TEST);                                  //15
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x34);
    ASSERT( TEST.dcc_data[1] == 0x41);                    //20
    ASSERT( TEST.dcc_data[2] == 0x75);

    p = TEST.parseDccTextCommand("M87f28");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_SPEED);
    ASSERT(!TEST.hasAcknowledge());                       //25
    ASSERT( TEST.dcc_data[0] == 0xC0);
    ASSERT( TEST.dcc_data[1] == 0x57);
    ASSERT( TEST.dcc_data[2] == 0x6E);
    ASSERT( TEST.dcc_data[3] == 0xF9);

    p = TEST.parseDccTextCommand("mf0");
    ASSERT( p == &TEST);                                  //30
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x00);
    ASSERT( TEST.dcc_data[1] == 0x60);                    //35
    ASSERT( TEST.dcc_data[2] == 0x60);

    p = TEST.parseDccTextCommand("m52r2");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);
    ASSERT(!TEST.hasAcknowledge());                       //40
    ASSERT( TEST.dcc_data[0] == 0x34);
    ASSERT( TEST.dcc_data[1] == 0x41);
    ASSERT( TEST.dcc_data[2] == 0x75);

    p = TEST.parseDccTextCommand("M87f21");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);                            //45
    ASSERT( TEST.repeat() == DCC_REPEAT_SPEED);           
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xC0);
    ASSERT( TEST.dcc_data[1] == 0x57);
    ASSERT( TEST.dcc_data[2] == 0x7A);                    //50
    ASSERT( TEST.dcc_data[3] == 0xED);                   


    p = TEST.parseDccTextCommand("m0R");
    ASSERT( p == &TEST);                                  
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);
    ASSERT(!TEST.hasAcknowledge());                       //55
    ASSERT( TEST.dcc_data[0] == 0x00);
    ASSERT( TEST.dcc_data[1] == 0x3F);                    
    ASSERT( TEST.dcc_data[2] == 0x00);                    
    ASSERT( TEST.dcc_data[3] == 0x3F);

    p = TEST.parseDccTextCommand("m18F1");
    ASSERT( p == &TEST);                                  //60
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);
    ASSERT(!TEST.hasAcknowledge());                       
    ASSERT( TEST.dcc_data[0] == 0x12);
    ASSERT( TEST.dcc_data[1] == 0x3F);                    //65
    ASSERT( TEST.dcc_data[2] == 0x81);
    ASSERT( TEST.dcc_data[3] == 0xAC);

    p = TEST.parseDccTextCommand("M9029R21");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 5);         
    ASSERT( TEST.repeat() == DCC_REPEAT_SPEED);          //70
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xE3);
    ASSERT( TEST.dcc_data[1] == 0x45);                    
    ASSERT( TEST.dcc_data[2] == 0x3F);
    ASSERT( TEST.dcc_data[3] == 0x15);                   //75
    ASSERT( TEST.dcc_data[4] == 0x8C);                    

    p = TEST.parseDccTextCommand("mA10101");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());                     //80
    ASSERT( TEST.dcc_data[0] == 0x00);
    ASSERT( TEST.dcc_data[1] == 0x9A);                    
    ASSERT( TEST.dcc_data[2] == 0x9A);

    p = TEST.parseDccTextCommand("m1B1010");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);                         //85
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x01);
    ASSERT( TEST.dcc_data[1] == 0xB5);                    
    ASSERT( TEST.dcc_data[2] == 0xB4);                 //90

    p = TEST.parseDccTextCommand("M127Cftft");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xC0);                //95
    ASSERT( TEST.dcc_data[1] == 0x7F);                
    ASSERT( TEST.dcc_data[2] == 0xAA);                    
    ASSERT( TEST.dcc_data[3] == 0x15);
	
    p = TEST.parseDccTextCommand("m127DNYNYNYNY");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);                        //100
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x7F);
    ASSERT( TEST.dcc_data[1] == 0xDE);
    ASSERT( TEST.dcc_data[2] == 0xAA);                //105
    ASSERT( TEST.dcc_data[3] == 0x0B);

    p = TEST.parseDccTextCommand("M0Eynynynyn");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 5);
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());                   //110
    ASSERT( TEST.dcc_data[0] == 0xC0);
    ASSERT( TEST.dcc_data[1] == 0x00);
    ASSERT( TEST.dcc_data[2] == 0xDF);
    ASSERT( TEST.dcc_data[3] == 0x55);
    ASSERT( TEST.dcc_data[4] == 0x4A);                //115
}

void DccPacketTest::testAccessoryBuilds() {
    UnitTest::start();

    DccPacket TEST;

    DccPacket* p = TEST.baBroadcast(2, 0).activate(true);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_ACCESSORY);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xBF);                    //5
    ASSERT( TEST.dcc_data[1] == 0x8C);
    ASSERT( TEST.dcc_data[2] == 0x33);
    
    p = TEST.baAddress(0, 0, 1).activate(false);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_ACCESSORY);       //10
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x80);                    
    ASSERT( TEST.dcc_data[1] == 0xF1);
    ASSERT( TEST.dcc_data[2] == 0x71);

    p = TEST.baAddress(0x123, 3, 1).activate(true);
    ASSERT( p == &TEST);                                //15
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_ACCESSORY);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xA3);                    
    ASSERT( TEST.dcc_data[1] == 0xBF);                  //20
    ASSERT( TEST.dcc_data[2] == 0x1C);
    

    p = TEST.eaBroadcast().state(0);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_ACCESSORY);
    ASSERT(!TEST.hasAcknowledge());                     //25
    ASSERT( TEST.dcc_data[0] == 0xBF);                    
    ASSERT( TEST.dcc_data[1] == 0x07);                 
    ASSERT( TEST.dcc_data[2] == 0x00);
    ASSERT( TEST.dcc_data[3] == 0xB8);
    
    p = TEST.eaAddress(0).state(7);
    ASSERT( p == &TEST);                                //30
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_ACCESSORY);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x80);                    
    ASSERT( TEST.dcc_data[1] == 0x71);                  //35               
    ASSERT( TEST.dcc_data[2] == 0x07);
    ASSERT( TEST.dcc_data[3] == 0xF6);

    p = TEST.eaAddress(0x789).state(0x1F);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_ACCESSORY);     //40
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x89); 
    ASSERT( TEST.dcc_data[1] == 0x17);                                 
    ASSERT( TEST.dcc_data[2] == 0x1F);
    ASSERT( TEST.dcc_data[3] == 0x81);                  //45
}

void DccPacketTest::testAccessoryParsing() {
    UnitTest::start();

    DccPacket TEST;

    DccPacket* p = TEST.parseDccTextCommand("BP2OA");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_ACCESSORY);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xBF);                    //5
    ASSERT( TEST.dcc_data[1] == 0x8C);
    ASSERT( TEST.dcc_data[2] == 0x33);
    
    p = TEST.parseDccTextCommand("B0P0O1D");
    p = TEST.baAddress(0, 0, 1).activate(false);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_ACCESSORY);       //10
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x80);                    
    ASSERT( TEST.dcc_data[1] == 0xF1);
    ASSERT( TEST.dcc_data[2] == 0x71);

    p = TEST.parseDccTextCommand("B291P3O1A");
    ASSERT( p == &TEST);                                //15
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_ACCESSORY);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xA3);                    
    ASSERT( TEST.dcc_data[1] == 0xBF);                  //20
    ASSERT( TEST.dcc_data[2] == 0x1C);
    
    p = TEST.parseDccTextCommand("ES0");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_ACCESSORY);
    ASSERT(!TEST.hasAcknowledge());                     //25
    ASSERT( TEST.dcc_data[0] == 0xBF);                    
    ASSERT( TEST.dcc_data[1] == 0x07);                 
    ASSERT( TEST.dcc_data[2] == 0x00);
    ASSERT( TEST.dcc_data[3] == 0xB8);
    
    p = TEST.parseDccTextCommand("E0S7");
    ASSERT( p == &TEST);                                //30
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_ACCESSORY);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x80);                    
    ASSERT( TEST.dcc_data[1] == 0x71);                  //35               
    ASSERT( TEST.dcc_data[2] == 0x07);
    ASSERT( TEST.dcc_data[3] == 0xF6);

    p = TEST.parseDccTextCommand("E1929S31");
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_ACCESSORY);     //40
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x89); 
    ASSERT( TEST.dcc_data[1] == 0x17);                                 
    ASSERT( TEST.dcc_data[2] == 0x1F);
    ASSERT( TEST.dcc_data[3] == 0x81);                  //45
}

void DccPacketTest::testMultiFunctionBits() {
    UnitTest::start();

    DccPacket TEST;

    DccPacket* p = TEST.mfAddress(0x34, 0x00).speed28(0x41);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);
    ASSERT(!TEST.hasAcknowledge());                       
    ASSERT( TEST.dcc_data[0] == 0x34);                    //5
    ASSERT( TEST.dcc_data[1] == 0x41);
    ASSERT( TEST.dcc_data[2] == 0x75);

    p = TEST.mfAddress(0xC0, 0x57).speed28(0x7A);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);                            
    ASSERT( TEST.repeat() == DCC_REPEAT_SPEED);           //10
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xC0);
    ASSERT( TEST.dcc_data[1] == 0x57);
    ASSERT( TEST.dcc_data[2] == 0x7A);                    
    ASSERT( TEST.dcc_data[3] == 0xED);                    //15
    
    p = TEST.mfAddress(0x12, 0x00).speed128(0x81);
    ASSERT( p == &TEST);                                  
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_STOP);
    ASSERT(!TEST.hasAcknowledge());                       
    ASSERT( TEST.dcc_data[0] == 0x12);                    //20
    ASSERT( TEST.dcc_data[1] == 0x3F);                    
    ASSERT( TEST.dcc_data[2] == 0x81);
    ASSERT( TEST.dcc_data[3] == 0xAC);
    
    p = TEST.mfAddress(0xE3, 0x45).speed128(0x15);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 5);                           //25
    ASSERT( TEST.repeat() == DCC_REPEAT_SPEED);          
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xE3);
    ASSERT( TEST.dcc_data[1] == 0x45);                    
    ASSERT( TEST.dcc_data[2] == 0x3F);                   //30
    ASSERT( TEST.dcc_data[3] == 0x15);                   
    ASSERT( TEST.dcc_data[4] == 0x8C);                    

    p = TEST.mfAddress(0x00,0x00).functionF0_F4(0x1F);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 3);
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);      //35
    ASSERT(!TEST.hasAcknowledge());                     
    ASSERT( TEST.dcc_data[0] == 0x00);
    ASSERT( TEST.dcc_data[1] == 0x9F);                    
    ASSERT( TEST.dcc_data[2] == 0x9F);

    p = TEST.mfAddress(0x01, 0x00).functionF5_F8(0x05);
    ASSERT( p == &TEST);                               //40
    ASSERT( TEST.size() == 3);                         
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0x01);
    ASSERT( TEST.dcc_data[1] == 0xB5);                 //45   
    ASSERT( TEST.dcc_data[2] == 0xB4);

    p = TEST.mfAddress(0xC0, 0x7F).functionF9_F12(0x0A);
    ASSERT( p == &TEST);
    ASSERT( TEST.size() == 4);
    ASSERT( TEST.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!TEST.hasAcknowledge());
    ASSERT( TEST.dcc_data[0] == 0xC0);                //50
    ASSERT( TEST.dcc_data[1] == 0x7F);                
    ASSERT( TEST.dcc_data[2] == 0xAA);                    
    ASSERT( TEST.dcc_data[3] == 0x15);
}
    
boolean DccPacketTest::testAll() {
    UnitTest::suite("DccPacket");
  
    testDataSizeBits();
    testAcknowledgeBits();
    testRepeatBits();
    testAddressBits();
    
    testMultiFunctionBuilds();
    testMultiFunctionParsing();
    testAccessoryBuilds();
    testAccessoryParsing();
    
    testMultiFunctionBits();
    
    return UnitTest::report();
}


