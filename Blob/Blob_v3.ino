#include <I2S.h>
#include <MKRNB.h>
#include "NBModem.h"
#include "Modem.h"
#include "arduino_secrets.h" 
#include <bitset>


// PIN Number 
const char PINNUMBER[] = SECRET_PINNUMBER;


// Initialize the library instances
NBClient client;
NB nbAccess;
NBModem modem;
GPRS gprs;

const char SERVER[] = SERVER_IP;  
const char PATH[]   = SERVER_PATH;         
const int  S_PORT   = SERVER_PORT;

byte      IMSI[15];     // IMSI number of the SIM-card.
uint8_t   UUID[16];     // UUID of the file.
uint16_t  seq_num = 0;  // Sequence number of the BLOB partition.
String    imsiString;   // IMSI number as a String.
String    uuidString;   // UUID number as a String.


#define SAMPLE_RATE 16000   // 16 kHz
#define RECORDING_TIME 0.7    // 0.5 seconds
#define TOTAL_SAMPLES (static_cast<int> (SAMPLE_RATE * RECORDING_TIME))

uint8_t buffer[TOTAL_SAMPLES*2];

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Starting I2S microphone test...");
  if (!I2S.begin(I2S_PHILIPS_MODE, SAMPLE_RATE, BITS_PER_SAMPLE)) {
    Serial.println("Failed to initialize I2S!");
    while (1);
  }

  bool connected = false;
  while (!connected) {
    if ((nbAccess.begin(PINNUMBER) == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  randomSeed(analogRead(A0));

  getIMSI(IMSI, IMSI_LENGTH);
  generateUUID(UUID);
  setUUIDString();
}

void loop() {
  Serial.println("Recording started...");

    for (int i = 0; i < TOTAL_SAMPLES*2; i+=2) {
      int16_t sample = I2S.read();
      if (sample) {
        uint8_t lower = sample & 0xFF;
        uint8_t upper = (sample >> 8) & 0xFF;
        
        buffer[i] = upper;
        buffer[i+1] = lower;


/*         std::bitset<16> bits(sample);
        Serial.print(bits); */
/*         Serial.print(" ");
        Serial.print(upper);
        Serial.print(" ");
        Serial.print(lower); */
        //Serial.print(",");
      }
    }



  Serial.println("Recording complete. Stopping...");
  I2S.end();


  sendBuffer(buffer, TOTAL_SAMPLES, 10);
  while (1);
}


void sendBuffer(uint8_t *buffer, size_t length, size_t segLength) {
  Serial.println("Connecting to backend");
  if (client.connect(SERVER, S_PORT)) {
    sendBlob(buffer, length, "true");
  } else {
    Serial.println("Failed to connect to HTTP server");
  }
  
}

void sendBlob(uint8_t *data, size_t length, char *eof) {
  Serial.println("Sending POST request...");
  // Send HTTP POST request
  client.print("POST ");
  client.print(PATH);
  client.println(" HTTP/1.1");

  // Request headers
  client.print("Host: ");
  client.println(SERVER);  

  client.print("User-Agent: ");
  client.println(USER_AGENT);

  client.println("Accept: */*");
  client.println("Connection: close");

  client.print("Content-Type: ");
  client.println(CONTENT_TYPE);

  client.print("Content-Length: ");
  client.println(length);

  client.print("X-Sample-Rate: ");
  client.println(SAMPLE_RATE);

  client.print("X-Bits-Per-Sample: ");
  client.println(BITS_PER_SAMPLE);

  client.print("X-End-Of-File: ");
  client.println(eof);

  client.print("X-Sequence-Number: ");
  client.println(seq_num);

  client.print("X-File-ID: ");
  client.println(uuidString);

  client.print("X-IMSI: ");
  client.println(imsiString);

  client.println();

  Serial.println("Headers written.");

  // Request body
  client.beginWrite();
  client.write(data, length);
  client.endWrite();

  // **Ensure proper termination**
  client.println();

  Serial.println("Request was sent.");

  Serial.println("Receiving response");
    while (client.available()) {
      char c = client.read();
        Serial.print(c); 
    } 
}


void setUUIDString() {
  uuidString = ""; 
  for (int i = 0; i < sizeof(UUID); i++) {
    char buffer[3]; 
    sprintf(buffer, "%02X", UUID[i]); 
    uuidString += buffer;
  }
}



/**
 * Generates a random UUID for the file to send.
 */
void generateUUID(uint8_t *uuid) {
  for (int i = 0; i < 16; i++) {
    uuid[i] = random(256);
  }
  uuid[6] = (uuid[6] & 0x0F) | 0x40;
  uuid[8] = (uuid[8] & 0x3F) | 0x80;
}


void getIMSI(byte* imsi, int length) {
  MODEM.send("AT+CIMI");  
  MODEM.waitForResponse(1000, &imsiString);
  imsiString.getBytes(imsi, length);
}
