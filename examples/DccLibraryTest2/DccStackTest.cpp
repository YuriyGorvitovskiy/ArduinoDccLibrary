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
#include <DccCollection.h>
#include <UnitTest.h>

#include "DccStackTest.h"

void DccStackTest::testConstructor() {
    UnitTest::start();

    DccPacket pack[3];

    DccStack  test1;
    ASSERT(test1.getTop() == NULL);

    DccStack  test2(pack, 2);
    ASSERT(test2.getTop() == &pack[0]);
    ASSERT(pack[0].next   == &pack[1]);
    ASSERT(pack[1].next   == NULL);

    DccStack  test3(pack, 3);
    ASSERT(test3.getTop() == &pack[0]);
    ASSERT(pack[0].next   == &pack[1]);
    ASSERT(pack[1].next   == &pack[2]);
    ASSERT(pack[2].next   == NULL);
}

void DccStackTest::testPush() {
    UnitTest::start();

    DccStack  test;
    DccPacket pack1;
    DccPacket pack2;
    DccPacket pack3;
    
    test.push(&pack1);    
    ASSERT(test.getTop() == &pack1);
    ASSERT(pack1.next  == NULL);
    
    test.push(&pack2);    
    ASSERT(test.getTop() == &pack2);
    ASSERT(pack2.next  == &pack1);
    ASSERT(pack1.next  == NULL);
    
    test.push(&pack3);    
    ASSERT(test.getTop() == &pack3);
    ASSERT(pack3.next  == &pack2);
    ASSERT(pack2.next  == &pack1);
    ASSERT(pack1.next  == NULL);
}

void DccStackTest::testPop() {
    UnitTest::start();

    DccStack  test;
    DccPacket pack1;
    DccPacket pack2;
    DccPacket pack3;
    
    test.push(&pack1);    
    test.push(&pack2);    
    test.push(&pack3);    

    ASSERT(test.pop()   == &pack3);
    ASSERT(test.getTop() == &pack2);
    ASSERT(pack2.next    == &pack1);
    ASSERT(pack1.next    == NULL);
    
    ASSERT(test.pop()   == &pack2);
    ASSERT(test.getTop() == &pack1);
    ASSERT(pack1.next    == NULL);

    ASSERT(test.pop()   == &pack1);
    ASSERT(test.getTop() == NULL);

    ASSERT(test.pop()   == NULL);
    ASSERT(test.getTop() == NULL);
}

void DccStackTest::testSize() {
    UnitTest::start();
    
    DccStack  test;
    DccPacket pack1;
    DccPacket pack2;
    ASSERT(test.size() == 0);
    
    test.push(&pack1);    
    ASSERT(test.size() == 1);
    
    test.push(&pack2);    
    ASSERT(test.size() == 2);
    
    test.pop();    
    ASSERT(test.size() == 1);
    
    test.pop();    
    ASSERT(test.size() == 0);

    test.pop();    
    ASSERT(test.size() == 0);
}

void DccStackTest::testIsEmpty() {
    UnitTest::start();
    
    DccStack  test;
    DccPacket pack1;
    DccPacket pack2;
    ASSERT(test.isEmpty());
    
    test.push(&pack1);    
    ASSERT(!test.isEmpty());
    
    test.push(&pack2);    
    ASSERT(!test.isEmpty());
    
    test.pop();    
    ASSERT(!test.isEmpty());
    
    test.pop();    
    ASSERT(test.isEmpty());

    test.pop();    
    ASSERT(test.isEmpty());
}

    
boolean DccStackTest::testAll() {
    UnitTest::suite("DccStack");
  
    testConstructor();
    testPush();
    testPop();
    testSize();
    testIsEmpty();
    
    return UnitTest::report();
}

