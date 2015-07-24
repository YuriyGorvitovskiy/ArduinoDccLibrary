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
#include <DccCommander.h>

void processSerialInput() {
    if (!Serial.available())
        return;
        
    char buffer[21];
    if (!Serial.readBytesUntil('\n', buffer, 20))
        return;    
    
    const char* result = DccCmd.handleTextCommand(buffer); // skip first slash
    Serial.println(result);
}

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(500); // half a second

    Serial.println("Initializing...");
    DccCmd.begin();
    Serial.println("Ready");
}


void loop() {
    processSerialInput();
    DccCmd.loop();
    delay(10); 
}



