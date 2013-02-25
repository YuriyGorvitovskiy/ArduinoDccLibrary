#include <Arduino.h>
#include "UnitTest.h"

const char* UnitTest::suite_name;
int         UnitTest::test_counter = 0;
int         UnitTest::assert_counter = 0;
int         UnitTest::failure_counter = 0;


void UnitTest::report_failure() {
    ++failure_counter;
    Serial.print("Test Failed: "); 
    Serial.print(suite_name); 
    Serial.print(":");
    Serial.print(test_counter); 
    Serial.print(":");
    Serial.println(assert_counter);
}

boolean UnitTest::report() {
    Serial.print("Test "); 
    Serial.print(suite_name); 
    Serial.print(" completed. Total: "); 
    Serial.print(test_counter); 
    Serial.print(". Failed: "); 
    Serial.print(failure_counter); 
    Serial.println("."); 
    return failure_counter == 0;
}

boolean UnitTest::bits_in_mask(byte bits, byte mask) {
    return ((bits & mask) == bits);
}

boolean UnitTest::unique_bits(byte values[], byte count) {
    for (int i = 0; i < count; ++i) {
        if (values[i] == 0)
            return false;
            
        for (int j = i+1; j < count; ++j) 
            if ((values[i] & values[j]) != 0)
                return false;
    }
    return true;
}

boolean UnitTest::unique_values(byte values[], byte count) {
    for (int i = 0; i < count; ++i)
        for (int j = i+1; j < count; ++j) 
            if (values[i] == values[j])
                return false;
                
    return true;
}

boolean UnitTest::unique_values(word values[], byte count) {
    for (int i = 0; i < count; ++i)
        for (int j = i+1; j < count; ++j) 
            if (values[i] == values[j])
                return false;
                
    return true;
}

boolean UnitTest::ordered_values(byte values[], byte count) {
    for (int i = 1; i < count; ++i) {
        if (values[i-1] > values[i])
            return false;
    }
    return true;
}


