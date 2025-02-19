#include <MKRNB.h>
#include "Modem.h"
#include "NBModem.h"
 
NB nbAccess;

NBModem modem;
 
void setup() {

    Serial.begin(115200);

    while (!Serial);
 
    Serial.println("Initializing modem...");

    if (!nbAccess.begin()) {

        Serial.println("Failed to connect to the modem!");

        return;

    }
 
    Serial.println("Retrieving IMSI...");

    MODEM.send("AT+CIMI");  // Send IMSI request command

    String imsi;

    MODEM.waitForResponse(1000, &imsi);  // Capture response
 
    Serial.print("IMSI: ");

    Serial.println(imsi);

}
 
void loop() {

    // Nothing here

}

 