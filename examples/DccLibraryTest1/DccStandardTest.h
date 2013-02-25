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
 
#ifndef __DCC_STANDARD_TEST_H__
#define __DCC_STANDARD_TEST_H__

class DccStandardTest  {

public:  
    static void testSize();
    static void testAddress();
    static void testMFKind();
    static void testBasicSpeed();
    static void testFunctionF0_F4();
    static void testFunctionF5_F8();
    static void testFunctionF9_F12();
    static void testMF_CV_Short();
    static void testMF_CV_Long();
    static void testAccessoryKind();
    static void testAccessoryAddress();
    static void testAccessoryState();

    static boolean testAll();
};


#endif //__DCC_STANDARD_TEST_H__

