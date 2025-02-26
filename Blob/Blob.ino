// libraries
#include <MKRNB.h>

// PIN Number
const char PINNUMBER[]  = "";

// initialize the library instance
NBClient client;
GPRS gprs;
NB nbAccess;

// URL, path and port (for example: example.org)
const char server[] = "129.241.153.67";  // Change this to your server
const char path[] = "/upload-audio";         // Change this to your API endpoint
const int port = 443; 

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Starting Arduino web client.");
  // connection state
  boolean connected = false;

  // After starting the modem with NB.begin()
  // attach to the GPRS network with the APN, login and password
  while (!connected) {
    if ((nbAccess.begin(PINNUMBER) == NB_READY) &&
        (gprs.attachGPRS() == GPRS_READY)) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");

  auto resp = client.connect(server, port);
  Serial.println(resp);

  // if you get a connection, report back via serial:
  if (resp) {
        Serial.println("Connected, sending BLOB...");

        // Send HTTP POST request
        client.print("POST ");
        client.print(path);
        client.println(" HTTP/1.1");

        // Use the actual host name instead of an IP if possible
        client.print("Host: ");
        client.println(server);  // Change to actual domain if available

        client.println("User-Agent: Arduino-MKR-NB-1500");
        client.println("Accept: */*");
        client.println("Connection: close");
        client.println("Content-Type: application/octet-stream");

        uint8_t data[] = {0x42, 0x4C, 0x4F, 0x42, 0x44, 0x41, 0x54, 0x41};

        // Send Content-Length properly
        client.print("Content-Length: ");
        client.println(sizeof(data));

        // **Critical Fix**: Ensure there is a double newline to separate headers and body
        client.println();

        // Send binary data
        client.write(data, sizeof(data));

        // **Ensure proper termination**
        client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) {
    Serial.print((char)client.read());
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for (;;)
      ;
  }
}