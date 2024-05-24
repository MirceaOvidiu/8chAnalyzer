#include <Wire.h>

const int pinCount = 8;
int pins[pinCount] = {2, 3, 4, 5, 6, 7, 8, 9}; // pinii utilizati

#define EEPROM_I2C_ADDRESS 0x50 // adresa I2C AT24C512 EEPROM
#define SEQUENCE_COUNT 64000    // numarul de secvente necesare grupei de lucru
#define SEQUENCE_DELAY 50       // delay intre secvente

unsigned long sequenceCounter = 0; // contor pentru nr. de secvente salvate

void setup() {
  Wire.begin();
  Serial.begin(2000000); // 2Mbits
  
  // initializam pinii ca output-uri
  for (int i = 0; i < pinCount; i++) {
    pinMode(pins[i], OUTPUT);
  }

  randomSeed(analogRead(0));
}

void loop() {
  if (sequenceCounter < SEQUENCE_COUNT) {
    // generam byte aleator
    byte randomByte = random(256); // numar aleator 8bit
    
    // trimis catre pini
    sendByteToPins(randomByte);

    // salvare in EEPROM
    writeEEPROM(sequenceCounter, randomByte);
    
    sequenceCounter++; // incrementam contorul
  } else {
    Serial.println("Toate secventele au fost scrise in EEPROM.");
    while (true); // oprim bucla dupa ce toate au fost salvate
  }

  delay(SEQUENCE_DELAY);   // repetare conform delay-ului
}

// functia ce trimite bitul catre pin
void sendByteToPins(byte b) {
  for (int i = 0; i < pinCount; i++) {
    digitalWrite(pins[i], (b >> i) & 0x01); // scrierea bitului la pin
  }
}

// functie ce scrie in EEPROM
void writeEEPROM(unsigned long address, byte data) {
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((int)(address >> 8)); // MSB
  Wire.write((int)(address & 0xFF)); // LSB
  Wire.write(data);
  Wire.endTransmission();
  delay(5); // ne asiguram ca scrierea a fost efectuata 
            // inainte de a repeta procesul
}

// functie ce citeste din EEPROM
byte readEEPROM(unsigned long address) {
  byte data = 0xFF;

  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((int)(address >> 8)); // MSB
  Wire.write((int)(address & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);

  if (Wire.available()) {
    data = Wire.read();
  }

  return data;
}
