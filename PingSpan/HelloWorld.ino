#include <MKRNB.h>
#include "Modem.h"
#include "NBModem.h"
#include "NBUdp.h"

GPRS gprs;
NB nbAccess;
NBUDP Udp;
unsigned int localPort = 4200;

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  
  Serial.println("Connecting...."); 
  boolean connected = false;
  while (!connected) {
    if ((nbAccess.begin(NULL, "mda.lab5e") == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected, retrying...");
      delay(1000);
    }
  }

  Udp.begin(localPort);
}


IPAddress spanAddress(172, 16, 15, 14); 
unsigned int spanPort = 1234;

#define PACKET_SIZE 128
byte packetBuffer[PACKET_SIZE];
bool sent = false;

void loop()
{
  if (sent) {
    delay(1000);
    return;
  }
  Serial.println("Sending packet");
  memset(packetBuffer, 0, PACKET_SIZE);
  sprintf((char*)packetBuffer, "Hello world");
  
  Udp.beginPacket(spanAddress, spanPort); 
  Udp.write(packetBuffer, strlen((const char*)packetBuffer));
  Udp.endPacket();
  sent = true;
}