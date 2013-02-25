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
 
#ifndef __DCC_PACKET_TEST_H__
#define __DCC_PACKET_TEST_H__

class DccPacketTest  {

public:  
    static void testDataSizeBits();
    static void testAcknowledgeBits();
    static void testRepeatBits();
    static void testAddressBits();
    
    static void testMultiFunctionBuilds();
    static void testMultiFunctionParsing();
    static void testAccessoryBuilds();
    static void testAccessoryParsing();
    
    static void testMultiFunctionBits();
    
    static boolean testAll();
    
};


#endif //__DCC_PACKET_TEST_H__

