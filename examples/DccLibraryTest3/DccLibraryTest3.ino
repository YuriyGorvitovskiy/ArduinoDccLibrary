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
#include <DccPacket.h>
#include <DccCollection.h>
#include <DccSteteKeeper.h>

#include "DccPacketTest.h"


#define LED (13)

boolean success = true;

void setup() {
   Serial.begin(115200);

   success = (DccPacketTest::testAll() && success);
   
   pinMode(LED, OUTPUT);
}

void loop() {
    digitalWrite(LED, HIGH);
    delay(success ? 1000 : 200);
    digitalWrite(LED, LOW);
    delay(success ? 1000 : 200);
}
