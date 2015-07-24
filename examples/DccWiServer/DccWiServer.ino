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
 
#include <WiServer.h>
#include <EEPROM.h>
#include <DccCommander.h>


extern "C" {
#include <g2100.h>
}

// Wireless configuration parameters ----------------------------------------
#define SECURITY_TYPE_WEP (1)
#define SECURITY_TYPE_WPA (2)

// WiFi is using pins 9 - LED, 10,11,12,13 (SPI for WiFi communication)
unsigned char local_ip[] = {192,168,1,10};	// IP address of WiShield
unsigned char gateway_ip[] = {192,168,1,1};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,255,0};	// subnet mask for the local network

const prog_char ssid[] PROGMEM = {"SSID"};	        // max 32 bytes
unsigned char security_type = ZG_SECURITY_TYPE_WEP;

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"PASSPHRASE"};	// max 64 characters

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = {	
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00,	// Key 0
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00,	// Key 1
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00,	// Key 2
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	0x00	// Key 3
			        };

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_INFRA;

unsigned char ssid_len;
unsigned char security_passphrase_len;

// Wireless configuration parameters ----------------------------------------

void Serial_printMACAddress() {
    Serial.print("MAC Address: ");
    U8* mac = zg_get_mac();
    for (int i = 0; i< 6; ++i) {
        if (i != 0)
            Serial.print(":");
        Serial.print(mac[i], HEX);
    }    
    Serial.println();
}

void Serial_printConnectionState() {
    if(zg_get_conn_state())
        Serial.println("WiFi: Connected");
    else           
        Serial.println("WiFi: Disconnected");
}

// This is our page serving function that generates web pages
boolean processRequest(char* URL) {
    const char* result = DccCmd.handleTextCommand(URL+1); // skip first slash
    if (result == DccCmd.UNKNOWN)
        return false;
        
    WiServer.print(result);
    return true;
}

void processSerialInput() {
    if (!Serial.available())
        return;
        
    char buffer[21];
    if (!Serial.readBytesUntil('\n', buffer, 20))
        return;    
    
    const char* result = DccCmd.handleTextCommand(buffer); // skip first slash
    Serial.println(result);
}


// DCC WiServer app -----------------------------


void setup() {
    Serial.begin(115200);
    Serial.setTimeout(500); // half a second

    Serial.println("Initializing...");
    // Initialize WiServer and have it use the sendMyPage function to serve pages
    WiServer.enableVerboseMode(true);
    WiServer.init(processRequest);    
   
    Serial_printMACAddress();
    Serial_printConnectionState();

    DccCmd.begin();
    Serial.println("Ready");
}


void loop() {
    WiServer.server_task();
    delay(10); // Don't know why but WiServer Works better.
    processSerialInput();
    DccCmd.loop();

}



