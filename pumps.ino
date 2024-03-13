#include <Wire.h>

#define I2C_LOWDRIVER_ADRESS (0x59) // Default address for Lowdriver 1011001
boolean bPumpState[4];
uint8_t nPumpVoltageByte[4];
extern uint8_t nFrequencyByte;

// Function to select control registers on the Lowdriver device
void selectControlRegisters() {
  Wire.beginTransmission(I2C_LOWDRIVER_ADRESS);
  Wire.write(0xFF);
  Wire.write(0x00);
  Wire.endTransmission();
}

// Function to select memory registers on the Lowdriver device
void selectMemoryRegisters() {
  Wire.beginTransmission(I2C_LOWDRIVER_ADRESS);
  Wire.write(0xFF);
  Wire.write(0x01);
  Wire.endTransmission();
}

// Initialization function for the Lowdriver device
void Lowdriver_init() {
  // Select control registers
  selectControlRegisters();
  Wire.beginTransmission(I2C_LOWDRIVER_ADRESS);
  Wire.write(0x01); // Select (Control) Register 0x01
  Wire.write(0x02); // Set Gain 0-3 (0x00-0x03 25v-100v)
  Wire.write(0x00); // Take device out of standby mode
  Wire.write(0x01); // Set sequencer to play WaveForm ID #1
  Wire.write(0x00); // End of sequence
  Wire.endTransmission();

  // Select memory registers
  selectMemoryRegisters();
  Wire.beginTransmission(I2C_LOWDRIVER_ADRESS);
  Wire.write(0x00); // Select Register 0x00
  Wire.write(0x05); // Header size –1
  Wire.write(0x80); // Start address upper byte (page), also indicates Waveform Synthesis Mode
  Wire.write(0x06); // Start address lower byte (in page address)
  Wire.write(0x00); // Stop address upper byte
  Wire.write(0x09); // Stop address Lower byte
  Wire.write(0x00); // Repeat count, 0 = infinite loop
  Wire.write((bPumpState[2] ? nPumpVoltageByte[2] : 0)); // Amplitude
  Wire.write(0x0C); // Frequency. (100Hz)
  Wire.write(100); // cycles
  Wire.write(0x00); // Envelope
  Wire.endTransmission();
  
  delay(10);

  // Start Waveform playback
  Wire.beginTransmission(I2C_LOWDRIVER_ADRESS);
  Wire.write(0x02); // Set page register to control space
  Wire.write(0x01); // Set GO bit (execute WaveForm sequence)
  Wire.endTransmission();
}

// Function to set pump voltage on the Lowdriver device
void Lowdriver_setvoltage(uint8_t _voltage) {
  nPumpVoltageByte[2] = _voltage;
  
  // Stop Waveform playback
  Wire.beginTransmission(I2C_LOWDRIVER_ADRESS);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.endTransmission();

  // Select memory registers
  selectMemoryRegisters();
  Wire.beginTransmission(I2C_LOWDRIVER_ADRESS);
  Wire.write(0x06); // Set page register to control space
  Wire.write((bPumpState[2] ? nPumpVoltageByte[2] : 0)); // 0-255
  Wire.endTransmission();
  
  delay(10);

  // Select control registers
  selectControlRegisters();
  Wire.beginTransmission(I2C_LOWDRIVER_ADRESS);
  Wire.write(0x02); // Start Waveform playback
  Wire.write(0x01);
  Wire.endTransmission();
}

// Function to set pump frequency on the Lowdriver device
void Lowdriver_setfrequency(uint16_t _frequency) {
  float temp = _frequency;
  temp /= 7.8125;
  nFrequencyByte = temp;
  if (nFrequencyByte == 0) nFrequencyByte = 1;
  
  // Stop Waveform playback
  Wire.beginTransmission(I2C_LOWDRIVER_ADRESS);
  Wire.write(0x02);
  Wire.write(0x00);
  Wire.endTransmission();

  // Select memory registers
  selectMemoryRegisters();
  Wire.beginTransmission(I2C_LOWDRIVER_ADRESS);
  Wire.write(0x07); // Set page register to control space
  Wire.write(nFrequencyByte); // 0-255
  Wire.endTransmission();
  
  delay(10);

  // Select control registers
  selectControlRegisters();
  Wire.beginTransmission(I2C_LOWDRIVER_ADRESS);
  Wire.write(0x02); // Start Waveform playback
  Wire.write(0x01);
  Wire.endTransmission();
}

void setup() {
  // Setup code here
}

void loop() {
    Lowdriver_setvoltage(40);
    Lowdriver_setfrequency(20);
}
