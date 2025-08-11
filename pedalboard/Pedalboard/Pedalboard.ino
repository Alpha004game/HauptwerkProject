/*
       ---PROGETTO HAUPTWERK---
       Titolo: pedalboardToArduinoToMidi
       Descrizione: Codice necessario al funzionamento della pedaliera (30/32 note)
       Nota: 
       Autore: Francesco Patti
       Versione: 1.5.0
       Arduino: Arduino Pro Micro con 32U4
*/
#include "MIDIUSB.h"

/*   ---MIDI SETTINGS---   */
#define MIDI_CHANNEL 0
#define BASENOTE 36 // Do2 (Calibrare a seconda del funzionamento di hauptwerk)

/*   ----Pedalboard config---   */
#define NUM_CHIPS 4
#define LATCHPIN 6
#define CLOCKPIN 5
#define DATAPIN 4
const int NUM_KEYS = 8 * NUM_CHIPS;

/* ---State memorizer---*/
bool lastState[NUM_KEYS];
byte values[NUM_CHIPS];


void setup() {
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);
  pinMode(DATAPIN, INPUT);

  for (int i = 0; i < NUM_KEYS; i++) {
    lastState[i] = false;
  }

  for(int i=0; i<NUM_CHIPS; i++)
  {
    values[i]=255; //Logica invertita per il PULLUP
  }
}

void loop() {
  digitalWrite(LATCHPIN, LOW);   // Blocca input
  delayMicroseconds(10);
  digitalWrite(LATCHPIN, HIGH);  // Carica i dati
  delayMicroseconds(10);

  // Leggo tutti i chip
  for (int chip = 0; chip < NUM_CHIPS; chip++) {
    //values[chip] = shiftIn(DATAPIN, CLOCKPIN, MSBFIRST);
    values[chip]=readShiftRegister();
  }

  // Ora scorri ogni bit per determinare il singolo stato di ogni pedale
  for (int i = 0; i < NUM_KEYS; i++) {
    int chipIndex = i / 8;
    int bitIndex = 7 - (i % 8);


    bool current = bitRead(values[chipIndex], bitIndex) == 0;

    if (current != lastState[i]) {
      lastState[i] = current;

      int reversedIndex = (NUM_KEYS - 1) - i;

      if (current) {
        noteOn(MIDI_CHANNEL, BASENOTE + reversedIndex, 100);
      } else {
        noteOff(MIDI_CHANNEL, BASENOTE + reversedIndex, 0);
      }

      MidiUSB.flush();
    }
  }
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOnMsg = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOnMsg);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOffMsg = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOffMsg);
}

byte readShiftRegister() {
  byte data = 0;
  for (int i = 0; i < 8; i++) {
    data <<= 1;
    data |= digitalRead(DATAPIN);
    digitalWrite(CLOCKPIN, HIGH);
    delayMicroseconds(1);
    digitalWrite(CLOCKPIN, LOW);
    delayMicroseconds(1);
  }
  return data;
}
