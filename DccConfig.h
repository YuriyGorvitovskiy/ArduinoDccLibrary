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

#ifndef __DCC_CONFIG_H__
#define __DCC_CONFIG_H__

// DccLibrary is using Timer1
//
// Pin A and B has to be connected to the DCC booster, and can't be applyed directly to the rails.
// DCC Rail Output pin A
#define DCC_PIN_OUT_A  (5)

// DCC Rail Output pin B
#define DCC_PIN_OUT_B  (6)


// DCC set it minimum to 14
#define DCC_PREAMBULE_SIZE (15)

// State Keeper configuration
#define DCC_STATE_EEPROM_ADDR (128)

#define DCC_STATE_MAX_COUNT   (40)


// Commander configuration
#define DCC_QUEUE_MAX_COUNT   (20)

// Repeat
#define DCC_REPEAT_STOP    		(5)
#define DCC_REPEAT_SPEED   		(3)
#define DCC_REPEAT_FUNCTION		(3)
#define DCC_REPEAT_ACCESSORY    (2)

#endif //__DCC_CONFIG_H__

