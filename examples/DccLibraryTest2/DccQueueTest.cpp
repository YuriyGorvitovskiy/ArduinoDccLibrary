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
#include <DccCollection.h>
#include <UnitTest.h>

#include "DccQueueTest.h"


void DccQueueTest::testConstructor() {
    UnitTest::start();

    DccQueue  test;
    ASSERT(test.getFirst() == NULL);
    ASSERT(test.getLast()  == NULL);
}

void DccQueueTest::testAdd() {
    UnitTest::start();
  
    DccQueue  test;
    DccPacket pack1;
    DccPacket pack2;
    DccPacket pack3;
    
    test.add(&pack1);    
    ASSERT(test.getFirst() == &pack1);
    ASSERT(test.getLast()  == &pack1);
    ASSERT(pack1.next      == NULL);
    
    test.add(&pack2);    
    ASSERT(test.getFirst() == &pack1);
    ASSERT(test.getLast()  == &pack2);
    ASSERT(pack1.next      == &pack2);
    ASSERT(pack2.next      == NULL);
    
    test.add(&pack3);    
    ASSERT(test.getFirst() == &pack1);
    ASSERT(test.getLast()  == &pack3);
    ASSERT(pack1.next      == &pack2);
    ASSERT(pack2.next      == &pack3);
    ASSERT(pack3.next      == NULL);
}

void DccQueueTest::testPush() {
    UnitTest::start();

    DccQueue  test;
    DccPacket pack1;
    DccPacket pack2;
    DccPacket pack3;
    
    test.push(&pack1);    
    ASSERT(test.getFirst() == &pack1);
    ASSERT(test.getLast()  == &pack1);
    ASSERT(pack1.next  == NULL);
    
    test.push(&pack2);    
    ASSERT(test.getFirst() == &pack2);
    ASSERT(test.getLast()  == &pack1);
    ASSERT(pack2.next  == &pack1);
    ASSERT(pack1.next  == NULL);
    
    test.push(&pack3);    
    ASSERT(test.getFirst() == &pack3);
    ASSERT(test.getLast()  == &pack1);
    ASSERT(pack3.next  == &pack2);
    ASSERT(pack2.next  == &pack1);
    ASSERT(pack1.next  == NULL);
}

void DccQueueTest::testNext() {
    UnitTest::start();
  
    DccQueue  test;
    DccPacket pack1;
    DccPacket pack2;
    DccPacket pack3;
    
    test.add(&pack1);    
    test.add(&pack2);    
    test.add(&pack3);    

    ASSERT(test.next()     == &pack1);
    ASSERT(test.getFirst() == &pack2);
    ASSERT(test.getLast()  == &pack3);
    ASSERT(pack2.next      == &pack3);
    ASSERT(pack3.next      == NULL);
    
    ASSERT(test.next()     == &pack2);
    ASSERT(test.getFirst() == &pack3);
    ASSERT(test.getLast()  == &pack3);
    ASSERT(pack3.next      == NULL);

    ASSERT(test.next()     == &pack3);
    ASSERT(test.getFirst() == NULL);
    ASSERT(test.getLast()  == NULL);

    ASSERT(test.next()     == NULL);
    ASSERT(test.getFirst() == NULL);
    ASSERT(test.getLast()  == NULL);
}

void DccQueueTest::testSize() {
    UnitTest::start();
    
    DccQueue  test;
    DccPacket pack1;
    DccPacket pack2;
    ASSERT(test.size() == 0);
    
    test.add(&pack1);    
    ASSERT(test.size() == 1);
    
    test.add(&pack2);    
    ASSERT(test.size() == 2);
    
    test.next();    
    ASSERT(test.size() == 1);
    
    test.next();    
    ASSERT(test.size() == 0);

    test.next();    
    ASSERT(test.size() == 0);
}

void DccQueueTest::testIsEmpty() {
    UnitTest::start();
    
    DccQueue  test;
    DccPacket pack1;
    DccPacket pack2;

    test.add(&pack1);    
    ASSERT(!test.isEmpty());
    
    test.add(&pack2);    
    ASSERT(!test.isEmpty());
    
    test.next();    
    ASSERT(!test.isEmpty());
    
    test.next();    
    ASSERT(test.isEmpty());

    test.next();    
    ASSERT(test.isEmpty());
}

void DccQueueTest::testReplaceSpeedKindPacket() {
    UnitTest::start();
    
    DccQueue  test;
    DccPacket pack1;
    DccPacket pack2;
    pack1.mfAddress7(0x23).speed14(true, 0xA);
    pack2.mfAddress14(0x23).speed128(true,0xA);
    test.add(&pack1);    
    test.add(&pack2);    
    
    DccPacket packR;
    packR.mfAddress14(0x23).speed128(true,0xB);
    ASSERT(test.replaceSameKindPacket(&packR, true));
    
    ASSERT(test.size() == 2);
    ASSERT(test.getFirst() == &pack1);
    ASSERT(test.getLast() == &pack2);

    ASSERT(pack1.repeat() == DCC_REPEAT_SPEED);                        //5
    ASSERT(pack1.size() == 3);
    ASSERT(pack1.dcc_data[0] == 0x23);
    ASSERT(pack1.dcc_data[1] == (DCC_MF_KIND3_FORWARD_OPERATION | 0xA));

    ASSERT(pack2.repeat() == 0);
    ASSERT(pack2.size() == 5);                                        //10
    ASSERT(pack2.dcc_data[0] == DCC_ADDRESS_LONG_MIN);
    ASSERT(pack2.dcc_data[1] == 0x23);
    ASSERT(pack2.dcc_data[2] == DCC_MF_KIND8_SPEED_128);
    ASSERT(pack2.dcc_data[3] == 0x8B);
    
    packR.mfAddress7(0x23).speed14(false,0xC);
    ASSERT(test.replaceSameKindPacket(&packR, false));                //15

    ASSERT(test.size() == 2);
    ASSERT(test.getFirst() == &pack1);
    ASSERT(test.getLast() == &pack2);

    ASSERT(pack1.repeat() == DCC_REPEAT_SPEED);
    ASSERT(pack1.size() == 3);                                        //20
    ASSERT(pack1.dcc_data[0] == 0x23);
    ASSERT(pack1.dcc_data[1] == (DCC_MF_KIND3_REVERSE_OPERATION | 0xC));

    ASSERT(pack2.repeat() == 0);
    ASSERT(pack2.size() == 5);
    ASSERT(pack2.dcc_data[0] == DCC_ADDRESS_LONG_MIN);                //25
    ASSERT(pack2.dcc_data[1] == 0x23);
    ASSERT(pack2.dcc_data[2] == DCC_MF_KIND8_SPEED_128);
    ASSERT(pack2.dcc_data[3] == 0x8B);
    
    packR.mfBroadcast().speed28(true,0xD);
    ASSERT(test.replaceSameKindPacket(&packR, true));
    packR.mfBroadcast().speed128(true,0xE);
    ASSERT(test.replaceSameKindPacket(&packR, true));                //30

    ASSERT(test.size() == 2);
    ASSERT(test.getFirst() == &pack1);
    ASSERT(test.getLast() == &pack2);

    ASSERT(pack1.repeat() == 0);
    ASSERT(pack1.size() == 3);                                       //35                          
    ASSERT(pack1.dcc_data[0] == 0x23);
    ASSERT(pack1.dcc_data[1] == (DCC_MF_KIND3_FORWARD_OPERATION | 0x16));

    ASSERT(pack2.repeat() == 0);
    ASSERT(pack2.size() == 5);
    ASSERT(pack2.dcc_data[0] == DCC_ADDRESS_LONG_MIN);               //40
    ASSERT(pack2.dcc_data[1] == 0x23);
    ASSERT(pack2.dcc_data[2] == DCC_MF_KIND8_SPEED_128);
    ASSERT(pack2.dcc_data[3] == 0x8E);                              
}

void DccQueueTest::testReplaceFunctionKindPacket() {
    UnitTest::start();
    
    DccQueue  test;
    DccPacket pack1;
    DccPacket pack2;
    DccPacket pack3;
    DccPacket pack4;
    DccPacket pack5;
    pack1.mfAddress7(0x23).functionF0_F4(0x1A);
    pack2.mfAddress14(0x23).functionF5_F8(0x0B);
    pack3.mfAddress7(0x23).functionF9_F12(0x0C);
    pack4.mfAddress14(0x23).functionF13_F20(0xDE);
    pack5.mfAddress7(0x23).functionF21_F28(0xF0);
    test.add(&pack1);    
    test.add(&pack2);    
    test.add(&pack3);    
    test.add(&pack4);    
    test.add(&pack5);    
    
    DccPacket packR;
    packR.mfAddress7(0x23).functionF0_F4(0x01);
    ASSERT(test.replaceSameKindPacket(&packR, true));
    packR.mfAddress14(0x23).functionF5_F8(0x02);
    ASSERT(test.replaceSameKindPacket(&packR, true));
    packR.mfAddress7(0x23).functionF9_F12(0x03);
    ASSERT(test.replaceSameKindPacket(&packR, false));
    packR.mfAddress14(0x23).functionF13_F20(0x45);
    ASSERT(test.replaceSameKindPacket(&packR, false));
    packR.mfAddress7(0x23).functionF21_F28(0x67);
    ASSERT(test.replaceSameKindPacket(&packR, true));            //5
 
    ASSERT(test.size() == 5);
    ASSERT(test.getFirst() == &pack1);
    ASSERT(test.getFirst()->next == &pack2);
    ASSERT(test.getFirst()->next->next == &pack3);                
    ASSERT(test.getFirst()->next->next->next == &pack4);         //10
    ASSERT(test.getLast() == &pack5);

    ASSERT(pack1.repeat() == 0);
    ASSERT(pack1.size() == 3);                             
    ASSERT(pack1.dcc_data[0] == 0x23);
    ASSERT(pack1.dcc_data[1] == (DCC_MF_KIND3_F0_F4 | 0x01));     //15

    ASSERT(pack2.repeat() == 0);
    ASSERT(pack2.size() == 4);
    ASSERT(pack2.dcc_data[0] == DCC_ADDRESS_LONG_MIN);
    ASSERT(pack2.dcc_data[1] == 0x23);
    ASSERT(pack2.dcc_data[2] == (DCC_MF_KIND4_F5_F8 | 0x02));    //20

    ASSERT(pack3.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(pack3.size() == 3);                             
    ASSERT(pack3.dcc_data[0] == 0x23);
    ASSERT(pack3.dcc_data[1] == (DCC_MF_KIND4_F9_F12 | 0x03));

    ASSERT(pack4.repeat() == DCC_REPEAT_FUNCTION);               //25
    ASSERT(pack4.size() == 5);
    ASSERT(pack4.dcc_data[0] == DCC_ADDRESS_LONG_MIN);
    ASSERT(pack4.dcc_data[1] == 0x23);
    ASSERT(pack4.dcc_data[2] == DCC_MF_KIND8_F13_F20);
    ASSERT(pack4.dcc_data[3] == 0x45);                           //30

    ASSERT(pack5.repeat() == 0);
    ASSERT(pack5.size() == 4);
    ASSERT(pack5.dcc_data[0] == 0x23);
    ASSERT(pack5.dcc_data[1] == DCC_MF_KIND8_F21_F28);
    ASSERT(pack5.dcc_data[2] == 0x67);                           //35
    
    packR.mfBroadcast().functionF0_F4(0x02);
    ASSERT(test.replaceSameKindPacket(&packR, true));
    packR.mfBroadcast().functionF5_F8(0x03);
    ASSERT(test.replaceSameKindPacket(&packR, true));
    packR.mfBroadcast().functionF9_F12(0x04);
    ASSERT(test.replaceSameKindPacket(&packR, false));
    packR.mfBroadcast().functionF13_F20(0x56);
    ASSERT(test.replaceSameKindPacket(&packR, false));
    packR.mfBroadcast().functionF21_F28(0x78);
    ASSERT(test.replaceSameKindPacket(&packR, true));            //40

    ASSERT(test.size() == 5);
    ASSERT(test.getFirst() == &pack1);
    ASSERT(test.getFirst()->next == &pack2);
    ASSERT(test.getFirst()->next->next == &pack3);                
    ASSERT(test.getFirst()->next->next->next == &pack4);         //45
    ASSERT(test.getLast() == &pack5);

    ASSERT(pack1.repeat() == 0);
    ASSERT(pack1.size() == 3);                             
    ASSERT(pack1.dcc_data[0] == 0x23);
    ASSERT(pack1.dcc_data[1] == (DCC_MF_KIND3_F0_F4 | 0x02));     //50

    ASSERT(pack2.repeat() == 0);
    ASSERT(pack2.size() == 4);
    ASSERT(pack2.dcc_data[0] == DCC_ADDRESS_LONG_MIN);
    ASSERT(pack2.dcc_data[1] == 0x23);
    ASSERT(pack2.dcc_data[2] == (DCC_MF_KIND4_F5_F8 | 0x03));    //55

    ASSERT(pack3.repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(pack3.size() == 3);                             
    ASSERT(pack3.dcc_data[0] == 0x23);
    ASSERT(pack3.dcc_data[1] == (DCC_MF_KIND4_F9_F12 | 0x04));

    ASSERT(pack4.repeat() == DCC_REPEAT_FUNCTION);               //60
    ASSERT(pack4.size() == 5);
    ASSERT(pack4.dcc_data[0] == DCC_ADDRESS_LONG_MIN);
    ASSERT(pack4.dcc_data[1] == 0x23);
    ASSERT(pack4.dcc_data[2] == DCC_MF_KIND8_F13_F20);
    ASSERT(pack4.dcc_data[3] == 0x56);                           //65

    ASSERT(pack5.repeat() == 0);
    ASSERT(pack5.size() == 4);
    ASSERT(pack5.dcc_data[0] == 0x23);
    ASSERT(pack5.dcc_data[1] == DCC_MF_KIND8_F21_F28);
    ASSERT(pack5.dcc_data[2] == 0x78);                           //70
}

void DccQueueTest::testReplaceAccessoryKindPacket() {
    UnitTest::start();
    
    DccQueue  test;
    DccPacket pack1;
    DccPacket pack2;
    DccPacket pack3;
    pack1.baAddress(0x23, 1, 0).activate(false);
    pack2.baAddress(0x23, 3, 1).activate(true);
    pack3.eaAddress(0x23).state(0x12);
    test.add(&pack1);
    test.add(&pack2);
    test.add(&pack3);
    
    DccPacket packR;
    packR.baAddress(0x23, 1, 1).activate(true);
    ASSERT(test.replaceSameKindPacket(&packR, true));
    packR.baAddress(0x23, 3, 0).activate(false);
    ASSERT(test.replaceSameKindPacket(&packR, false));
    packR.eaAddress(0x23).state(0x13);
    ASSERT(test.replaceSameKindPacket(&packR, true));

    
    ASSERT(test.size() == 3);
    ASSERT(test.getFirst() == &pack1);                //5
    ASSERT(test.getFirst()->next == &pack2);
    ASSERT(test.getLast() == &pack3);

    ASSERT( pack1.size() == 3);
    ASSERT( pack1.repeat() == 0);
    ASSERT(!pack1.hasAcknowledge());                  //10
    ASSERT( pack1.dcc_data[0] == 0xA3);                    
    ASSERT( pack1.dcc_data[1] == 0xFB);
    
    ASSERT( pack2.size() == 3);
    ASSERT( pack2.repeat() == DCC_REPEAT_ACCESSORY);
    ASSERT(!pack2.hasAcknowledge());                 //15
    ASSERT( pack2.dcc_data[0] == 0xA3);                    
    ASSERT( pack2.dcc_data[1] == 0xF6);
    
    ASSERT( pack3.size() == 4);
    ASSERT( pack3.repeat() == 0);
    ASSERT(!pack3.hasAcknowledge());                //20
    ASSERT( pack3.dcc_data[0] == 0xA3);                    
    ASSERT( pack3.dcc_data[1] == 0x71);
    ASSERT( pack3.dcc_data[2] == 0x13);
    
    packR.baBroadcast(3, 1).activate(false);
    ASSERT(test.replaceSameKindPacket(&packR, false));

    packR.eaBroadcast().state(0x04);
    ASSERT(test.replaceSameKindPacket(&packR, true));    //25

    ASSERT( pack1.size() == 3);
    ASSERT( pack1.repeat() == 0);
    ASSERT(!pack1.hasAcknowledge());
    ASSERT( pack1.dcc_data[0] == 0xA3);                    
    ASSERT( pack1.dcc_data[1] == 0xFB);                //30
    
    ASSERT( pack2.size() == 3);
    ASSERT( pack2.repeat() == DCC_REPEAT_ACCESSORY);
    ASSERT(!pack2.hasAcknowledge());
    ASSERT( pack2.dcc_data[0] == 0xA3);                    
    ASSERT( pack2.dcc_data[1] == 0xF7);                //35

    ASSERT( pack3.size() == 4);
    ASSERT( pack3.repeat() == 0);
    ASSERT(!pack3.hasAcknowledge()); 
    ASSERT( pack3.dcc_data[0] == 0xA3);                    
    ASSERT( pack3.dcc_data[1] == 0x71);                //40
    ASSERT( pack3.dcc_data[2] == 0x04);

    
}    

boolean DccQueueTest::testAll() {
    UnitTest::suite("DccQueue");
  
    testConstructor();
    testAdd();
    testPush();
    testNext();
    testSize();
    testIsEmpty();
    
    testReplaceSpeedKindPacket();
    testReplaceFunctionKindPacket();
    testReplaceAccessoryKindPacket();
    
    return UnitTest::report();
}

