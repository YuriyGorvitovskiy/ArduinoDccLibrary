#ifndef __UNIT_TEST_H__
#define __UNIT_TEST_H__

class UnitTest {
  
private: 

    static const char* suite_name;
    static int         test_counter;
    static int         assert_counter;
    static int         failure_counter;
  
public:  

    static void suite(const char* name);
    static void start();
    
    static void nextAssert();
    static void report_failure();
    
    static boolean report();
    
    static boolean bits_in_mask(byte bits, byte mask);
    static boolean unique_bits(byte values[], byte count);
    static boolean unique_values(byte values[], byte count);
    static boolean unique_values(word values[], byte count);
    static boolean ordered_values(byte values[], byte count);
};

#define ASSERT(condition)  {UnitTest::nextAssert(); if(!(condition)) {UnitTest::report_failure(); return;}}

inline void UnitTest::suite(const char* name) {
    suite_name = name;
    test_counter = 0;
    assert_counter = 0;
    failure_counter = 0;
}

inline void UnitTest::start() {
    ++test_counter;
    assert_counter = 0;
}

inline void UnitTest::nextAssert() {
    ++assert_counter;
}

#endif //__UNIT_TEST_H__
