#include <I2S.h>

#define SAMPLE_RATE 16000   // 16 kHz
#define RECORDING_TIME 50    // 5 seconds
#define TOTAL_SAMPLES (SAMPLE_RATE * RECORDING_TIME)

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Starting I2S microphone test...");
  if (!I2S.begin(I2S_PHILIPS_MODE, SAMPLE_RATE, 32)) {
    Serial.println("Failed to initialize I2S!");
    while (1);
  }
}

void loop() {
  Serial.println("Recording started...");

  for (int i = 0; i < TOTAL_SAMPLES; i++) {
    int sample = I2S.read();
    if (sample) {
      Serial.print(sample);
      Serial.print(",");
    }
  }

  Serial.println("Recording complete. Stopping...");
  I2S.end();
  while (1);
}




/*
#define SAMPLES 128        
#define SIGNAL_FREQUENCY 1000
#define SAMPLING_FREQUENCY 5000
#define AMPLITUDE 100  
#define HPF_ALPHA 0.9

double vReal[SAMPLES];
double vImag[SAMPLES];

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SIGNAL_FREQUENCY);
int32_t prevInput = 0;
int32_t prevOutput = 0;

void highpassFilter() {
  int32_t i2sData[256];
  size_t bytesRead;
  
  I2S.read((char *)i2sData, sizeof(i2sData));

  int numSamples = bytesRead / sizeof(int32_t);

  for (int i = 0; i < numSamples; i++) {
    int32_t input = i2sData[i];

    int32_t output = (HPF_ALPHA * (prevOutput + input - prevInput));
    prevInput = input;
    prevOutput = output;
    Serial.println(output);
  }
}



void loop() {
  size_t bytesRead;
  int32_t sample;

  while (collectedSamples < BUFFER_SIZE) {
    sample = I2S.read();
    if (sample) {
      i2sBuffer[collectedSamples++] = sample;
    }
  }

  Serial.println("Data collection complete. Processing...");

  int32_t prevInput = 0, prevOutput = 0;
  double HPF_ALPHA = 0.95;

  for (size_t i = 0; i < BUFFER_SIZE; i++) {
    int32_t input = i2sBuffer[i];
    int32_t output = HPF_ALPHA * (prevOutput + input - prevInput);
    prevInput = input;
    prevOutput = output;
    
    Serial.println(output);
  }

  Serial.println("Processing complete. Stopping...");
  I2S.end();
  while (1);
}
*/
