/*
 SMS sender

 This sketch, for the MKR NB 1500 board, sends an SMS message
 you enter in the serial monitor. Connect your Arduino with the
 SIM card, open the serial monitor, and wait for
 the "READY" message to appear in the monitor. Next, type a
 message to send and press "return". Make sure the serial
 monitor is set to send a newline when you press return.

 Circuit:
 * MKR NB 1500 board
 * Antenna
 * SIM card that can send SMS

 created 25 Feb 2012
 by Tom Igoe
*/

// Include the NB library
#include <MKRNB.h>

#include "arduino_secrets.h" 
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// initialize the library instance
NB nbAccess;
NB_SMS sms;

int counter = 0;

char* numbers[] = {"+4791704251", "91704251", "0091704251"};
char* texts[]   = {"fra +47", "fra blank", "fra 00"};

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("SMS Messages Sender");

  // connection state
  bool connected = false;

  // Start NB module
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (!connected) {
    Serial.println("Enters PIN...");
    NB_NetworkStatus_t e = nbAccess.begin();
    Serial.println("connecting...");
    if (e == NB_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("NB initialized");
}

void loop() {

  Serial.print("Entering mobile number: ");
  //char remoteNum[20];  // telephone number to send SMS
  //readSerial(remoteNum);
  Serial.println(numbers[counter]);

  // SMS text
  //Serial.print("Now, enter SMS content: ");
  //char txtMsg[200];
  //readSerial(txtMsg);
  Serial.println("SENDING");
  //Serial.println();
  Serial.println("Message: ");
  Serial.println(texts[counter]);

  // send the message
  sms.beginSMS(numbers[counter]);
  sms.print(texts[counter]);
  // send the message
  int status = sms.endSMS();  // Capture return value

  // Check if sending was successful
  if (status == 1) {
    Serial.println("\nSMS sent successfully!");
  } else {
    Serial.println("\nFailed to send SMS.");
  }
  Serial.println("\nCOMPLETE!\n");
  counter++;

  if (counter == 3) {
    counter = 0;
  }
}

/*
  Read input serial
 */
int readSerial(char result[]) {
  int i = 0;
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if (inChar != '\r') {
        result[i] = inChar;
        i++;
      }
    }
  }
}
