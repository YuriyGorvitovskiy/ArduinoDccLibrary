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
 
#ifndef __DCC_PROTOCOL_TEST_H__
#define __DCC_PROTOCOL_TEST_H__

class DccProtocolTest  {
private:
    static DccPacket TEST;

    static byte readState();
    static void execute(DccPacket* p);

    static boolean checkBit(boolean expected, byte statistic);
    static boolean checkPreambule(byte*& from);
    static boolean checkData(byte*& from, DccPacket* p);
    static boolean checkAcknowledge(byte*& from, boolean isShort);

public:  
    static void test3BytesCommand();
    static void test4BytesCommand();
    static void test5BytesCommand();
    static void test6BytesCommand();
    static void test1BytesAcknowledge();
    static void test2BytesAcknowledge();
    
    static boolean testAll();
    
};


#endif //__DCC_PROTOCOL_TEST_H__

