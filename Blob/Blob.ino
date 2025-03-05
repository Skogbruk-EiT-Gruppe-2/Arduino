/*
  SSL Web client - POST BLOB

  This sketch connects to a website using SSL through a MKR NB 1500 board.
  Instead of using GET, it sends a binary blob via HTTP POST.
*/


// Libraries
#include <MKRNB.h>
#include "NBModem.h"
#include "Modem.h"
#include "base64.hpp"
#include "arduino_secrets.h" 


// PIN Number 
const char PINNUMBER[] = SECRET_PINNUMBER;


// Initialize the library instances
NBSSLClient client;
NBModem modem;
NB nbAccess;
GPRS gprs;


// Server details
const char SERVER[] = SERVER_IP;  
const char PATH[]   = SERVER_PATH;         
const int  S_PORT   = SERVER_PORT;                  


// Sample binary data (BLOB)
uint8_t blobData[42] = {
  0x42, 0x4C, 0x4F, 0x42, 0x44, 0x41, 0x54, 0x41, 
  0x10, 0x2A, 0x33, 0x45, 0x56, 0x67, 0x78, 0x89, 
  0x90, 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xFA, 0x1B,
  0x2C, 0x3D, 0x4E, 0x5F, 0x6A, 0x7B, 0x8C, 0x9D,
  0xAE, 0xBF, 0xC0, 0xD1, 0xE2, 0xF3, 0x04, 0x15 
  // EOF_1, EOF_2
};  


// Modem ID and unique file ID
byte      IMSI[15];     // IMSI number of the SIM-card.
uint8_t   UUID[16];     // UUID of the file.
uint16_t  seq_num = 0;  // Sequence number of the BLOB partition.
String    imsiString;   // IMSI number as a String.
String    uuidString;   // UUID number as a String.



/**
 * Main function - Setup.
 */
void setup() {
  initModem();
  connect();
  connectBackend();
  sendFile();
}



/**
 * Initializes the modem and logs status and numbers to the serial monitor.
 */
void initModem() {
  Serial.begin(BAUD_RATE);

  while (!Serial) {
    ; 
  }

  if (!nbAccess.begin()) {
    Serial.println("   x\tFailed to connect to the modem.");
    exit(1);
  }

  Serial.println("Initializing Modem");
  Serial.print("   ✓\tStatus code:\t\t");
  Serial.println(PM->RCAUSE.reg, HEX);
  Serial.print("   ✓\tGetting IMSI:\t\t");
  getIMSI(IMSI, IMSI_LENGTH);
  Serial.println(imsiString);
  Serial.print("   ✓\tGenerating UUID:\t");
  generateUUID(UUID);
  setUUIDString();
  Serial.println(uuidString);
}



/**
 * Connects the SIM-card to the web.
 */
void connect() {
  Serial.println("Starting Arduino web client");
  boolean connected = false;

  // Connect to the network
  while (!connected) {
    Serial.print("   ✓\tAttached GPRS:\t\t");
    boolean gprs_ready = gprs.attachGPRS() == GPRS_READY;
    String gprs_ok = gprs_ready ? "ready" : "failed";
    Serial.println(gprs_ok);

    Serial.print("   ✓\tAccessed SIM-card:\t\t");
    boolean sim_ready = nbAccess.begin(PINNUMBER) == NB_READY;
    String sim_ok = sim_ready ? "ready" : "failed";
    Serial.println(sim_ok);

    if (sim_ready && gprs_ready) {
        connected = true;
    } else {
        Serial.println("   !\tNot connected, retrying...\n");
        delay(1000);
    }
  }

  Serial.println("   ✓\tConnected to network.");
}



/**
 * Establishes connection with the FastAPI server.
 */
void connectBackend() {
  Serial.println("Connecting to server");
  if (client.connect(SERVER, S_PORT)) {
    Serial.print("   ✓\tHost:\t\t");
    Serial.println(SERVER_IP);
    Serial.print("   ✓\tPort:\t\t");
    Serial.print(S_PORT);
    Serial.print("   ✓\tPath:\t\t");
    Serial.print(SERVER_PATH);
    Serial.println("   ✓\tConnected.");
  } else {
    Serial.println("   x\tConnection failed.");
    exit(1);
  }
}



/**
 * Splits the BLOB object into several packages numbered by a sequence number.
 * Sends these packages to the backend.
 */
void sendFile() {
  int startIndex = seq_num * MAX_BLOB_SIZE;
  int endIndex = min((seq_num + 1) * MAX_BLOB_SIZE, sizeof(blobData));
  int counter = 0;
  bool eof = false;

  while (startIndex < sizeof(blobData)) {
    uint8_t arr[endIndex - startIndex - 1];

    for (int i = startIndex; i < endIndex; i++) {
      arr[i-startIndex] = blobData[i];
    }

    if (seq_num * MAX_BLOB_SIZE >= sizeof(blobData)) {
      eof = true;
    }

    sendBlob(arr, sizeof(arr), eof);

    seq_num++;
    startIndex = seq_num * MAX_BLOB_SIZE;
    endIndex = min((seq_num + 1) * MAX_BLOB_SIZE, sizeof(blobData));
    Serial.println("   ✓\tPackage " + String(counter) + " sent.");
  }

  Serial.println("   ✓\tAll packages sent");

  Serial.println("\nDisconnecting.");
  client.stop();
  Serial.println("   ✓\tDisconnected.");
  Serial.println("   ✓\tTerminated.");
}



/**
 * Sends BLOB data to the server through HTTP POST at the given endpoint.
 * @param data    BLOB data to send.
 * @param length  Length of the data.
 */
void sendBlob(uint8_t *data, size_t length, bool eof) {
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

  // Request body
  client.beginWrite();
  client.write(data, length);
  client.endWrite();

  // **Ensure proper termination**
  client.println();


  // Read the response from the server.
  if ( PRINT_HTTP_RESPONSE ) {
    Serial.println("Receiving response");
    while (client.available()) {
        char c = client.read();
        Serial.print(c); 
    } 
  }
}



/**
 * Returns a conversion of the UUID uint8_t array to a String.
 */
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



/**
 * Retrieves the SIM-card's IMSI number from the modem.
 */
void getIMSI(byte* imsi, int length) {
  MODEM.send("AT+CIMI");  
  MODEM.waitForResponse(1000, &imsiString);
  imsiString.getBytes(imsi, length);
}



// Not needed but must be declared.
void loop() {}
