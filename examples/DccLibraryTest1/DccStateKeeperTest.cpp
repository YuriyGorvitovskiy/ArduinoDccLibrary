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
#include <EEPROM.h>

#include <DccConfig.h>
#include <DccPacket.h>
#include <DccStateKeeper.h>
#include <UnitTest.h>

#include "DccStateKeeperTest.h"

DccPacket testHeap[6];
DccStack  recycle(testHeap, 6);
DccQueue  queue;

void DccStateKeeperTest::startTest() {
    UnitTest::start();
    DccState.resetAll();
    
     while(!queue.isEmpty()) {
        recycle.push(queue.next());
     }
}

void DccStateKeeperTest::testSpeed() {
    startTest();
    
    DccPacket TEST;

    TEST.mfAddress7(0x12).speed28(true, 2);
    DccState.saveState(&TEST);    
    
    TEST.mfAddress14(0x2345).speed128(false, 0);
    DccState.saveState(&TEST);    
    
    //Overwrite the privious value
    TEST.mfAddress14(0x2345).speed128(false, 0x15);
    DccState.saveState(&TEST);  
    
    DccState.readNextState(queue, recycle);
    DccPacket* p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 3);
    ASSERT( p->repeat() == DCC_REPEAT_STOP);
    ASSERT(!p->hasAcknowledge());                       
    ASSERT( p->dcc_data[0] == 0x12);
    ASSERT( p->dcc_data[1] == 0x61);                    //05
    ASSERT( p->dcc_data[2] == 0x73);

    p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 3);
    ASSERT( p->repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!p->hasAcknowledge());                               
    ASSERT( p->dcc_data[0] == 0x12);                    //10
    ASSERT( p->dcc_data[1] == 0x80);                    
    ASSERT( p->dcc_data[2] == 0x92);

    ASSERT( queue.isEmpty());

    DccState.readNextState(queue, recycle);
    p = queue.next();
    recycle.push(p);

    ASSERT( p->size() == 5);                           
    ASSERT( p->repeat() == DCC_REPEAT_SPEED);            //15     
    ASSERT(!p->hasAcknowledge());
    ASSERT( p->dcc_data[0] == 0xE3);
    ASSERT( p->dcc_data[1] == 0x45);                    
    ASSERT( p->dcc_data[2] == 0x3F);                   
    ASSERT( p->dcc_data[3] == 0x15);                     //20              
    ASSERT( p->dcc_data[4] == 0x8C);                    

    p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 4);
    ASSERT( p->repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!p->hasAcknowledge());                               
    ASSERT( p->dcc_data[0] == 0xE3);                    //25
    ASSERT( p->dcc_data[1] == 0x45);                    
    ASSERT( p->dcc_data[2] == 0x80);                    
    ASSERT( p->dcc_data[3] == 0x26);

    ASSERT( queue.isEmpty());                           
    
    DccState.readNextState(queue, recycle);
    p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 3);                            //30
    ASSERT( p->repeat() == DCC_REPEAT_STOP);
    ASSERT(!p->hasAcknowledge());                       
    ASSERT( p->dcc_data[0] == 0x12);
    ASSERT( p->dcc_data[1] == 0x61);                    
    ASSERT( p->dcc_data[2] == 0x73);
}


void DccStateKeeperTest::testFunctions() {
    startTest();
    
    DccPacket TEST;

    TEST.mfAddress7(0x12).speed28(true, 2);
    DccState.saveState(&TEST);    
    
    TEST.mfAddress7(0x12).functionF0_F4(true, false, false, true, true);
    DccState.saveState(&TEST);    
    
    TEST.mfAddress7(0x12).functionF5_F8(true, false, false, true);
    DccState.saveState(&TEST);    
    
    TEST.mfAddress14(0x2345).speed128(false, 0x15);
    DccState.saveState(&TEST);    
      
    TEST.mfAddress14(0x2345).functionF0_F4(true, true, true, true, true);
    DccState.saveState(&TEST);    

    TEST.mfAddress14(0x2345).functionF5_F8(false, false, true, true);
    DccState.saveState(&TEST);    

    TEST.mfAddress14(0x2345).functionF9_F12(true, true, false, false);
    DccState.saveState(&TEST);    

    //Overwrite the privious value
    TEST.mfAddress14(0x2345).functionF0_F4(false, true, false, false, true);
    DccState.saveState(&TEST);    

    //Overwrite the privious value
    TEST.mfAddress14(0x2345).functionF5_F8(true, false, true, false);
    DccState.saveState(&TEST);    

    //Overwrite the privious value
    TEST.mfAddress14(0x2345).functionF9_F12(false, false, true, true);
    DccState.saveState(&TEST);    

    TEST.mfAddress14(0x2345).functionF13_F20(false, true, false, true, false, true, false, true);
    DccState.saveState(&TEST);    
    
    
    DccState.readNextState(queue, recycle);
    DccPacket* p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 3);
    ASSERT( p->repeat() == DCC_REPEAT_STOP);
    ASSERT(!p->hasAcknowledge());                       
    ASSERT( p->dcc_data[0] == 0x12);
    ASSERT( p->dcc_data[1] == 0x61);                    //05
    ASSERT( p->dcc_data[2] == 0x73);

    p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 3);
    ASSERT( p->repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!p->hasAcknowledge());                               
    ASSERT( p->dcc_data[0] == 0x12);                    //10
    ASSERT( p->dcc_data[1] == 0x9C);                    
    ASSERT( p->dcc_data[2] == 0x8E);

    p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 3);
    ASSERT( p->repeat() == DCC_REPEAT_FUNCTION);
    ASSERT(!p->hasAcknowledge());                       //15                            
    ASSERT( p->dcc_data[0] == 0x12);
    ASSERT( p->dcc_data[1] == 0xB9);                    
    ASSERT( p->dcc_data[2] == 0xAB);

    ASSERT( queue.isEmpty());

    DccState.readNextState(queue, recycle);
    p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 5);                           //20                    
    ASSERT( p->repeat() == DCC_REPEAT_SPEED);       
    ASSERT(!p->hasAcknowledge());
    ASSERT( p->dcc_data[0] == 0xE3);
    ASSERT( p->dcc_data[1] == 0x45);                    
    ASSERT( p->dcc_data[2] == 0x3F);                  //25                 
    ASSERT( p->dcc_data[3] == 0x15);                            
    ASSERT( p->dcc_data[4] == 0x8C);                    

    p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 4);                           
    ASSERT( p->repeat() == DCC_REPEAT_SPEED);       
    ASSERT(!p->hasAcknowledge());                      //30
    ASSERT( p->dcc_data[0] == 0xE3);
    ASSERT( p->dcc_data[1] == 0x45);                    
    ASSERT( p->dcc_data[2] == 0x89);                   
    ASSERT( p->dcc_data[3] == 0x2F);                    
    
    p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 4);                            //35                         
    ASSERT( p->repeat() == DCC_REPEAT_SPEED);      
    ASSERT(!p->hasAcknowledge());
    ASSERT( p->dcc_data[0] == 0xE3);
    ASSERT( p->dcc_data[1] == 0x45);                    
    ASSERT( p->dcc_data[2] == 0xB5);                    //40
    ASSERT( p->dcc_data[3] == 0x13);                  
    
    p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 4);                           
    ASSERT( p->repeat() == DCC_REPEAT_SPEED);       
    ASSERT(!p->hasAcknowledge());
    ASSERT( p->dcc_data[0] == 0xE3);                    //45
    ASSERT( p->dcc_data[1] == 0x45);                    
    ASSERT( p->dcc_data[2] == 0xAC);                   
    ASSERT( p->dcc_data[3] == 0x0A);    

    ASSERT( queue.isEmpty());    
}


void DccStateKeeperTest::testGeneration() {
    startTest();
    
    DccPacket TEST;
    //State Capacity + 1, The last one has to replace the first one
    for(int i = 0; i<= DCC_STATE_MAX_COUNT; ++i) {
        TEST.mfAddress14(i).speed128(false, 0x15);
        DccState.saveState(&TEST);    
    }
    byte address0 = TEST.dcc_data[0];
    byte address1 = TEST.dcc_data[1];
    
    DccState.readNextState(queue, recycle);
    DccPacket* p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 5);                                           
    ASSERT( p->repeat() == DCC_REPEAT_SPEED);       
    ASSERT(!p->hasAcknowledge());
    ASSERT( p->dcc_data[0] == address0);
    ASSERT( p->dcc_data[1] == address1);                        //5
    ASSERT( p->dcc_data[2] == 0x3F);                   
    ASSERT( p->dcc_data[3] == 0x15);                            
    ASSERT( p->dcc_data[4] == (0x2A ^ address0 ^ address1));                    

    p = queue.next();
    recycle.push(p);
    ASSERT( p->size() == 4);                           
    ASSERT( p->repeat() == DCC_REPEAT_SPEED);                   //10
    ASSERT(!p->hasAcknowledge());
    ASSERT( p->dcc_data[0] == address0);
    ASSERT( p->dcc_data[1] == address1);                    
    ASSERT( p->dcc_data[2] == 0x80);                   
    ASSERT( p->dcc_data[3] == (0x80 ^ address0 ^ address1));    //15                  
   
    ASSERT( queue.isEmpty()); 
}


boolean DccStateKeeperTest::testAll() {
    UnitTest::suite("DccStateKeeper");
  
    testSpeed();
    testFunctions();
    testGeneration();
    
    DccState.resetAll();
    
    return UnitTest::report();
}


