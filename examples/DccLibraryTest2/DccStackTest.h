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
 
#ifndef __DCC_STACK_TEST_H__
#define __DCC_STACK_TEST_H__

class DccStackTest  {

public:  
    static void testConstructor();
    static void testPush();
    static void testPop();
    static void testSize();
    static void testIsEmpty();
    
    static boolean testAll();
};


#endif //__DCC_STACK_TEST_H__

