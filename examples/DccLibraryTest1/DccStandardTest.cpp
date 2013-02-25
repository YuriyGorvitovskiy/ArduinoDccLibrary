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

#include "DccStandardTest.h"

void DccStandardTest::testSize() {
    UnitTest::start();
    
    byte values[] = {
        DCC_DATA_SIZE_MIN,
        DCC_DATA_SIZE_MAX
    };
    ASSERT(UnitTest::unique_values(values, sizeof(values)));
    ASSERT(UnitTest::ordered_values(values, sizeof(values)));
}

void DccStandardTest::testAddress() {
    UnitTest::start();

    byte values[] = {
        DCC_ADDRESS_BROADCAST,
        DCC_ADDRESS_SHORT_MIN,
        DCC_ADDRESS_SHORT_MAX,
        DCC_ADDRESS_ACCESSORY_MIN,
        DCC_ADDRESS_ACCESSORY_MAX,
        DCC_ADDRESS_LONG_MIN,
        DCC_ADDRESS_LONG_MAX,
        DCC_ADDRESS_RESERVED_MIN,
        DCC_ADDRESS_RESERVED_MAX,
        DCC_ADDRESS_IDLE
    };
    ASSERT(UnitTest::unique_values(values, sizeof(values)));
    ASSERT(UnitTest::ordered_values(values, sizeof(values)));
    
    ASSERT(UnitTest::bits_in_mask(DCC_ADDRESS_BROADCAST, DCC_ADDRESS_SHORT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_ADDRESS_SHORT_MIN, DCC_ADDRESS_SHORT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_ADDRESS_SHORT_MAX, DCC_ADDRESS_SHORT_MASK));
}

void DccStandardTest::testMFKind() {
    UnitTest::start();

    byte kinds[] = {
        DCC_MF_KIND2_MASK,
        DCC_MF_KIND3_MASK,
        DCC_MF_KIND4_MASK,
        DCC_MF_KIND8_MASK
    };
    ASSERT(UnitTest::unique_values(kinds, sizeof(kinds)));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND2_SPEED_OPERATION, DCC_MF_KIND2_MASK));
    
    ASSERT((DCC_MF_KIND3_REVERSE_OPERATION & DCC_MF_KIND2_MASK) == DCC_MF_KIND2_SPEED_OPERATION);
    ASSERT((DCC_MF_KIND3_FORWARD_OPERATION & DCC_MF_KIND2_MASK) == DCC_MF_KIND2_SPEED_OPERATION);

    byte kind3[] = {
        DCC_MF_KIND3_CONTROL,
        DCC_MF_KIND3_ADVANCED_OPERATION,
        DCC_MF_KIND3_REVERSE_OPERATION,
        DCC_MF_KIND3_FORWARD_OPERATION,
        DCC_MF_KIND3_F0_F4,
        DCC_MF_KIND3_F5_F12,
        DCC_MF_KIND3_FUTURE_EXPANSION,
        DCC_MF_KIND3_CONFIG_VARIABLE_ACCESS
    };
    ASSERT(UnitTest::unique_values(kind3, sizeof(kind3)));                                                //5
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND3_CONTROL, DCC_MF_KIND3_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND3_ADVANCED_OPERATION, DCC_MF_KIND3_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND3_REVERSE_OPERATION, DCC_MF_KIND3_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND3_FORWARD_OPERATION, DCC_MF_KIND3_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND3_F0_F4, DCC_MF_KIND3_MASK));                                //10
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND3_F5_F12, DCC_MF_KIND3_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND3_FUTURE_EXPANSION, DCC_MF_KIND3_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND3_CONFIG_VARIABLE_ACCESS, DCC_MF_KIND3_MASK));

    ASSERT((DCC_MF_KIND4_DECODER_CONTROL & DCC_MF_KIND3_MASK) == DCC_MF_KIND3_CONTROL);
    ASSERT((DCC_MF_KIND4_CONSIST_CONTROL & DCC_MF_KIND3_MASK) == DCC_MF_KIND3_CONTROL);                   //15
    ASSERT((DCC_MF_KIND8_SPEED_128 & DCC_MF_KIND3_MASK) == DCC_MF_KIND3_ADVANCED_OPERATION);
    ASSERT((DCC_MF_KIND8_SPEED_LIMIT & DCC_MF_KIND3_MASK) == DCC_MF_KIND3_ADVANCED_OPERATION);
    ASSERT((DCC_MF_KIND4_F5_F8 & DCC_MF_KIND3_MASK) == DCC_MF_KIND3_F5_F12);
    ASSERT((DCC_MF_KIND4_F9_F12 & DCC_MF_KIND3_MASK) == DCC_MF_KIND3_F5_F12);
    ASSERT((DCC_MF_KIND8_LONG_STATE_CONTROL & DCC_MF_KIND3_MASK) == DCC_MF_KIND3_FUTURE_EXPANSION);       //20
    ASSERT((DCC_MF_KIND8_SHORT_STATE_CONTROL & DCC_MF_KIND3_MASK) == DCC_MF_KIND3_FUTURE_EXPANSION);
    ASSERT((DCC_MF_KIND8_F13_F20 & DCC_MF_KIND3_MASK) == DCC_MF_KIND3_FUTURE_EXPANSION);
    ASSERT((DCC_MF_KIND8_F21_F28 & DCC_MF_KIND3_MASK) == DCC_MF_KIND3_FUTURE_EXPANSION);
    ASSERT((DCC_MF_KIND4_CV_SHORT_ACCESS & DCC_MF_KIND3_MASK) == DCC_MF_KIND3_CONFIG_VARIABLE_ACCESS);
    ASSERT((DCC_MF_KIND4_CV_LONG_ACCESS & DCC_MF_KIND3_MASK) == DCC_MF_KIND3_CONFIG_VARIABLE_ACCESS);    //25
    

    byte kind4[] = {
        DCC_MF_KIND4_DECODER_CONTROL,
        DCC_MF_KIND4_CONSIST_CONTROL,
        DCC_MF_KIND4_F5_F8,
        DCC_MF_KIND4_F9_F12,
        DCC_MF_KIND4_CV_SHORT_ACCESS,
        DCC_MF_KIND4_CV_LONG_ACCESS
    };
    ASSERT(UnitTest::unique_values(kind4, sizeof(kind4)));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND4_DECODER_CONTROL, DCC_MF_KIND4_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND4_CONSIST_CONTROL, DCC_MF_KIND4_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND4_F5_F8, DCC_MF_KIND4_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND4_F9_F12, DCC_MF_KIND4_MASK));                               //30
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND4_CV_SHORT_ACCESS, DCC_MF_KIND4_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND4_CV_LONG_ACCESS, DCC_MF_KIND4_MASK));
    
    ASSERT(DCC_BA_KIND4_CV_LONG_ACCESS == DCC_MF_KIND4_CV_LONG_ACCESS);
    ASSERT(DCC_EA_KIND4_CV_LONG_ACCESS == DCC_MF_KIND4_CV_LONG_ACCESS);

    byte kind8[] = {
        DCC_MF_KIND8_SPEED_128,
        DCC_MF_KIND8_SPEED_LIMIT,
        DCC_MF_KIND8_LONG_STATE_CONTROL,
        DCC_MF_KIND8_SHORT_STATE_CONTROL,
        DCC_MF_KIND8_F13_F20,
        DCC_MF_KIND8_F21_F28
    };    
    ASSERT(UnitTest::unique_values(kind8, sizeof(kind8)));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND8_SPEED_128, DCC_MF_KIND8_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND8_SPEED_LIMIT, DCC_MF_KIND8_MASK));                          //35
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND8_LONG_STATE_CONTROL, DCC_MF_KIND8_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND8_SHORT_STATE_CONTROL, DCC_MF_KIND8_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND8_F13_F20, DCC_MF_KIND8_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_KIND8_F21_F28, DCC_MF_KIND8_MASK));
}

void DccStandardTest::testBasicSpeed() {
    UnitTest::start();

     byte speed14[] = {
        DCC_MF_SPEED_14_STOP,
        DCC_MF_SPEED_14_EMERGENCY_STOP,
        DCC_MF_SPEED_14_MIN,
        DCC_MF_SPEED_14_MAX
    };    
    
    ASSERT(UnitTest::ordered_values(speed14, sizeof(speed14)));
    ASSERT(UnitTest::unique_values(speed14, sizeof(speed14)));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_SPEED_14_STOP, DCC_MF_SPEED_14_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_SPEED_14_EMERGENCY_STOP, DCC_MF_SPEED_14_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_SPEED_14_MIN, DCC_MF_SPEED_14_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_SPEED_14_MAX, DCC_MF_SPEED_14_MASK));

    byte speed28[] = {
        DCC_MF_SPEED_28_STOP,
        DCC_MF_SPEED_28_EMERGENCY_STOP,
        DCC_MF_SPEED_28_MIN,
        DCC_MF_SPEED_28_MAX
    };    
    ASSERT(UnitTest::ordered_values(speed28, sizeof(speed28)));
    ASSERT(UnitTest::unique_values(speed28, sizeof(speed28)));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_SPEED_28_STOP, DCC_MF_SPEED_28_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_SPEED_28_EMERGENCY_STOP, DCC_MF_SPEED_28_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_SPEED_28_MIN, DCC_MF_SPEED_28_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_SPEED_28_MAX, DCC_MF_SPEED_28_MASK));
}

void DccStandardTest::testFunctionF0_F4() {
    UnitTest::start();

     byte functions[] = {
        DCC_MF_FUNCTION_F0,
        DCC_MF_FUNCTION_F1,
        DCC_MF_FUNCTION_F2,
        DCC_MF_FUNCTION_F3,
        DCC_MF_FUNCTION_F4
    };    
    ASSERT(UnitTest::unique_values(functions, sizeof(functions)));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F0, DCC_MF_FUNCTION_F0_F4_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F1, DCC_MF_FUNCTION_F0_F4_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F2, DCC_MF_FUNCTION_F0_F4_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F3, DCC_MF_FUNCTION_F0_F4_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F4, DCC_MF_FUNCTION_F0_F4_MASK));
}

void DccStandardTest::testFunctionF5_F8() {
    UnitTest::start();

     byte functions[] = {
        DCC_MF_FUNCTION_F5,
        DCC_MF_FUNCTION_F6,
        DCC_MF_FUNCTION_F7,
        DCC_MF_FUNCTION_F8
    };    
    ASSERT(UnitTest::unique_values(functions, sizeof(functions)));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F5, DCC_MF_FUNCTION_F5_F8_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F6, DCC_MF_FUNCTION_F5_F8_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F7, DCC_MF_FUNCTION_F5_F8_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F8, DCC_MF_FUNCTION_F5_F8_MASK));
}

void DccStandardTest::testFunctionF9_F12() {
    UnitTest::start();

     byte functions[] = {
        DCC_MF_FUNCTION_F9,
        DCC_MF_FUNCTION_F10,
        DCC_MF_FUNCTION_F11,
        DCC_MF_FUNCTION_F12
    };    
    ASSERT(UnitTest::unique_values(functions, sizeof(functions)));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F9, DCC_MF_FUNCTION_F9_F12_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F10, DCC_MF_FUNCTION_F9_F12_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F11, DCC_MF_FUNCTION_F9_F12_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_FUNCTION_F12, DCC_MF_FUNCTION_F9_F12_MASK));
}

void DccStandardTest::testMF_CV_Short() {
    UnitTest::start();

     byte values[] = {
        DCC_MF_CV_SHORT_ACCELERATION,
        DCC_MF_CV_SHORT_DECELERATION
    };    
    ASSERT(UnitTest::unique_values(values, sizeof(values)));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_CV_SHORT_ACCELERATION, DCC_MF_CV_SHORT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_MF_CV_SHORT_DECELERATION, DCC_MF_CV_SHORT_MASK));
}

void DccStandardTest::testMF_CV_Long() {
    UnitTest::start();

     byte operations[] = {
        DCC_CV_VERIFY,
        DCC_CV_WRITE,
        DCC_CV_BIT_OP
    };    
    ASSERT(UnitTest::unique_values(operations, sizeof(operations)));
    ASSERT(UnitTest::bits_in_mask(DCC_CV_VERIFY, DCC_CV_LONG_OP_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_CV_WRITE, DCC_CV_LONG_OP_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_CV_BIT_OP, DCC_CV_LONG_OP_MASK));
    
    ASSERT(DCC_CV_MASK_1 != 0);
    ASSERT(DCC_CV_MASK_2 != 0);
    
     byte bitop[] = {
        DCC_CV_BIT_VERIFY,
        DCC_CV_BIT_WRITE
    };    
    ASSERT(UnitTest::unique_values(operations, sizeof(operations)));
    ASSERT(UnitTest::bits_in_mask(DCC_CV_BIT_VERIFY, DCC_CV_BIT_OP_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_CV_BIT_WRITE, DCC_CV_BIT_OP_MASK));
    
    ASSERT(DCC_CV_BIT_VALUE_0 != DCC_CV_BIT_VALUE_1);

     byte bits[] = {
        DCC_CV_BIT_0,
        DCC_CV_BIT_1,
        DCC_CV_BIT_2,
        DCC_CV_BIT_3,
        DCC_CV_BIT_4,
        DCC_CV_BIT_5,
        DCC_CV_BIT_6,
        DCC_CV_BIT_7
    };    
    ASSERT(UnitTest::ordered_values(bits, sizeof(bits)));
    ASSERT(UnitTest::unique_values(bits, sizeof(bits)));
    
    ASSERT(UnitTest::bits_in_mask(DCC_CV_BIT_0, DCC_CV_BIT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_CV_BIT_1, DCC_CV_BIT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_CV_BIT_2, DCC_CV_BIT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_CV_BIT_3, DCC_CV_BIT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_CV_BIT_4, DCC_CV_BIT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_CV_BIT_5, DCC_CV_BIT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_CV_BIT_6, DCC_CV_BIT_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_CV_BIT_7, DCC_CV_BIT_MASK));
}

void DccStandardTest::testAccessoryKind() {
    UnitTest::start();

     byte kinds[] = {
        DCC_ACCESSORY_KIND_BASIC,
        DCC_ACCESSORY_KIND_EXTENDED
    };    
    ASSERT(UnitTest::unique_values(kinds, sizeof(kinds)));
    ASSERT(UnitTest::bits_in_mask(DCC_ACCESSORY_KIND_BASIC,    DCC_ACCESSORY_KIND_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_ACCESSORY_KIND_EXTENDED, DCC_ACCESSORY_KIND_MASK));
    ASSERT(DCC_ACCESSORY_KIND_EXTENDED != DCC_ACCESSORY_EXTENDED);
}

void DccStandardTest::testAccessoryAddress() {
    UnitTest::start();

    ASSERT(DCC_BA_ADDRESS_MASK_1 != 0);
    ASSERT(DCC_BA_ADDRESS_MASK_2 != 0);
    ASSERT(DCC_BA_ADDRESS_SHIFT != 0);
    ASSERT(DCC_BA_ADDRESS_BROADCAST != 0);
    ASSERT((((DCC_BA_ADDRESS_MASK_2) << DCC_BA_ADDRESS_SHIFT) | DCC_BA_ADDRESS_MASK_1) == DCC_BA_ADDRESS_BROADCAST);
    ASSERT((((DCC_BA_ADDRESS_MASK_2) << DCC_BA_ADDRESS_SHIFT) & DCC_BA_ADDRESS_MASK_1) == 0);
    ASSERT(DCC_BA_ADDRESS_BROADCAST_1 == DCC_BA_ADDRESS_MASK_1);
    ASSERT(DCC_BA_ADDRESS_BROADCAST_2 == 0);    

     byte pairs[] = {
        DCC_BA_ADDRESS_PAIR_0,
        DCC_BA_ADDRESS_PAIR_1,
        DCC_BA_ADDRESS_PAIR_2,
        DCC_BA_ADDRESS_PAIR_3
    };    
    ASSERT( UnitTest::unique_values(pairs, sizeof(pairs)));
    ASSERT( UnitTest::bits_in_mask(DCC_BA_ADDRESS_PAIR_0, DCC_BA_ADDRESS_PAIR_MASK));
    ASSERT( UnitTest::bits_in_mask(DCC_BA_ADDRESS_PAIR_1, DCC_BA_ADDRESS_PAIR_MASK));
    ASSERT( UnitTest::bits_in_mask(DCC_BA_ADDRESS_PAIR_2, DCC_BA_ADDRESS_PAIR_MASK));
    ASSERT( UnitTest::bits_in_mask(DCC_BA_ADDRESS_PAIR_3, DCC_BA_ADDRESS_PAIR_MASK));
    ASSERT( UnitTest::bits_in_mask(1 << DCC_BA_ADDRESS_PAIR_SHIFT, DCC_BA_ADDRESS_PAIR_MASK));
    ASSERT(!UnitTest::bits_in_mask(1 << (DCC_BA_ADDRESS_PAIR_SHIFT-1), DCC_BA_ADDRESS_PAIR_MASK));

     byte output[] = {
        DCC_BA_ADDRESS_OUTPUT_0,
        DCC_BA_ADDRESS_OUTPUT_1
    };    
    ASSERT( UnitTest::unique_values(output, sizeof(output)));
    ASSERT( UnitTest::bits_in_mask(DCC_BA_ADDRESS_OUTPUT_0, DCC_BA_ADDRESS_OUTPUT_MASK));
    ASSERT( UnitTest::bits_in_mask(DCC_BA_ADDRESS_OUTPUT_1, DCC_BA_ADDRESS_OUTPUT_MASK));

     byte activate[] = {
        DCC_BA_ACTIVATE,
        DCC_BA_DEACTIVATE
    };  
    
    ASSERT(UnitTest::unique_values(activate, sizeof(activate)));
    ASSERT(UnitTest::bits_in_mask(DCC_BA_ACTIVATE, DCC_BA_ACTIVATE_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_BA_DEACTIVATE, DCC_BA_ACTIVATE_MASK));
    

    ASSERT(DCC_EA_ADDRESS_MASK_1  != 0);
    ASSERT(DCC_EA_ADDRESS_MASK_2  != 0);
    ASSERT(DCC_EA_ADDRESS_MASK_3  != 0);
    ASSERT(DCC_EA_ADDRESS_SHIFT_2 != 0);
    ASSERT(DCC_EA_ADDRESS_SHIFT_3 != 0);
    ASSERT((((DCC_EA_ADDRESS_MASK_3) << DCC_EA_ADDRESS_SHIFT_3) | ((DCC_EA_ADDRESS_MASK_2) << DCC_EA_ADDRESS_SHIFT_2) | DCC_EA_ADDRESS_MASK_1) == DCC_EA_ADDRESS_BROADCAST);
    ASSERT((((DCC_EA_ADDRESS_MASK_3) << DCC_EA_ADDRESS_SHIFT_3) & ((DCC_EA_ADDRESS_MASK_2) << DCC_EA_ADDRESS_SHIFT_2) & DCC_EA_ADDRESS_MASK_1) == 0);
    ASSERT(DCC_EA_ADDRESS_BROADCAST_1 == DCC_EA_ADDRESS_MASK_1);
    ASSERT(DCC_EA_ADDRESS_BROADCAST_2 == 0);    
    ASSERT(DCC_EA_ADDRESS_BROADCAST_3 == DCC_EA_ADDRESS_MASK_3);
    
    byte cvlocation[] = {
        DCC_BA_CV_ADDRESS_DECODER,
        DCC_BA_CV_ADDRESS_P0_O0,
        DCC_BA_CV_ADDRESS_P0_O1,
        DCC_BA_CV_ADDRESS_P1_O0,
        DCC_BA_CV_ADDRESS_P1_O1,
        DCC_BA_CV_ADDRESS_P2_O0,
        DCC_BA_CV_ADDRESS_P2_O1,
        DCC_BA_CV_ADDRESS_P3_O0,
        DCC_BA_CV_ADDRESS_P3_O1
    };  
    ASSERT(UnitTest::unique_values(cvlocation, sizeof(cvlocation)));
    ASSERT(UnitTest::bits_in_mask(DCC_BA_CV_ADDRESS_P0_O0, DCC_BA_CV_ADDRESS_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_BA_CV_ADDRESS_P0_O1, DCC_BA_CV_ADDRESS_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_BA_CV_ADDRESS_P1_O0, DCC_BA_CV_ADDRESS_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_BA_CV_ADDRESS_P1_O1, DCC_BA_CV_ADDRESS_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_BA_CV_ADDRESS_P2_O0, DCC_BA_CV_ADDRESS_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_BA_CV_ADDRESS_P2_O1, DCC_BA_CV_ADDRESS_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_BA_CV_ADDRESS_P3_O0, DCC_BA_CV_ADDRESS_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_BA_CV_ADDRESS_P3_O1, DCC_BA_CV_ADDRESS_MASK));
    
    ASSERT((DCC_BA_CV_ADDRESS_OUTPUT & DCC_BA_CV_ADDRESS_DECODER) == 0);
    ASSERT((DCC_BA_CV_ADDRESS_OUTPUT & DCC_BA_CV_ADDRESS_P0_O0) == DCC_BA_CV_ADDRESS_OUTPUT);
    ASSERT((DCC_BA_CV_ADDRESS_OUTPUT & DCC_BA_CV_ADDRESS_P0_O1) == DCC_BA_CV_ADDRESS_OUTPUT);
    ASSERT((DCC_BA_CV_ADDRESS_OUTPUT & DCC_BA_CV_ADDRESS_P1_O0) == DCC_BA_CV_ADDRESS_OUTPUT);
    ASSERT((DCC_BA_CV_ADDRESS_OUTPUT & DCC_BA_CV_ADDRESS_P1_O1) == DCC_BA_CV_ADDRESS_OUTPUT);
    ASSERT((DCC_BA_CV_ADDRESS_OUTPUT & DCC_BA_CV_ADDRESS_P2_O0) == DCC_BA_CV_ADDRESS_OUTPUT);
    ASSERT((DCC_BA_CV_ADDRESS_OUTPUT & DCC_BA_CV_ADDRESS_P2_O1) == DCC_BA_CV_ADDRESS_OUTPUT);
    ASSERT((DCC_BA_CV_ADDRESS_OUTPUT & DCC_BA_CV_ADDRESS_P3_O0) == DCC_BA_CV_ADDRESS_OUTPUT);
    ASSERT((DCC_BA_CV_ADDRESS_OUTPUT & DCC_BA_CV_ADDRESS_P3_O1) == DCC_BA_CV_ADDRESS_OUTPUT);
}

void DccStandardTest::testAccessoryState() {
    UnitTest::start();
    
     byte states[] = {
        DCC_EA_STATE_STOP,
        DCC_EA_STATE_MIN,
        DCC_EA_STATE_MAX
    };  
    ASSERT(UnitTest::ordered_values(states, sizeof(states)));
    ASSERT(UnitTest::unique_values(states, sizeof(states)));
    ASSERT(UnitTest::bits_in_mask(DCC_EA_STATE_STOP, DCC_EA_STATE_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_EA_STATE_MIN, DCC_EA_STATE_MASK));
    ASSERT(UnitTest::bits_in_mask(DCC_EA_STATE_MAX, DCC_EA_STATE_MASK));
}

boolean DccStandardTest::testAll() {
    UnitTest::suite("DccStandard");
  
    testSize();
    testAddress();
    testMFKind();
    testBasicSpeed();
    testFunctionF0_F4();
    testFunctionF5_F8();
    testFunctionF9_F12();
    testMF_CV_Short();
    testMF_CV_Long();
    testAccessoryKind();
    testAccessoryAddress();
    testAccessoryState();
    
    return UnitTest::report();
}

