// SIM-CARD
#define SECRET_PINNUMBER     ""

// Server
#define SERVER_IP   "129.241.153.67"
#define SERVER_PATH "/upload-audio"
#define SERVER_PORT  80

// Configs
#define MAX_BLOB_SIZE        12                         // Maximum size of the BLOB partition that can be sent.
#define USER_AGENT          "Arduino-MKR-NB-1500"
#define CONTENT_TYPE        "application/octet-stream"
#define PRINT_HTTP_RESPONSE  false                      // When true, the HTTP response is printed to Serial Monitor.
#define BAUD_RATE            9600
#define EOF_1                0xFF
#define EOF_2                0xD9
#define SAMPLE_RATE          16000
#define BITS_PER_SAMPLE      24
#define IMSI_LENGTH          15