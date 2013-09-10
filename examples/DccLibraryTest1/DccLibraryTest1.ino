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
#include <EEPROM.h>

#include <DccConfig.h>
#include <DccStandard.h>
#include <DccProtocol.h>

#include "DccStandardTest.h"
#include "DccProtocolTest.h"
#include "DccStateKeeperTest.h"

#define LED (13)

boolean success = true;

void setup() {
   Serial.begin(115200);
   
   //Teensy 3.0 required some time before Serial become functional.
   delay(500);
   
   success = (DccStandardTest::testAll() && success);
   //success = (DccProtocolTest::testAll() && success);
   success = (DccStateKeeperTest::testAll() && success);

   pinMode(LED, OUTPUT);
}

void loop() {
    digitalWrite(LED, HIGH);
    delay(success ? 1000 : 200);
    digitalWrite(LED, LOW);
    delay(success ? 1000 : 200);
   }
