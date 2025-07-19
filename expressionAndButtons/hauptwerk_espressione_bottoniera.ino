/*
       ---PROGETTO HAUPTWERK---
       Titolo: buttonsAndExpression
       Descrizione: Codice necessario al funzionamento delle staffe e della bottoniera
       Nota: Bottoniera per input PULLUP
       Autore: Francesco Patti
       Versione: 1.0.0
       Arduino: Arduino Pro Micro con 32U4
*/

/*
PEDALETTI DA IMPLEMENTARE

CICCIO:
+
-
10 aggiustabili
P->I, P->II, P->III
I->III
II->I, II->III

TUTTI
Ann gen
Fiss

Totale: 21 (Capire distribuzione tra tastiere e pedaletti)

FABIO:
+
-
10 aggiustabili
P->I, P->II, P->III, P->IV
I->III, I->IV
II->I, II->III, II->IV
III->IV

TUTTI
Ann gen
Fiss

Totale: 25 (Capire distribuizione tra tastiere e pedaletti)


*/

#include "MIDIUSB.h"

/*---Sezione Bottoniera---*/
#define CLOCK_B_PIN 8
#define LATCH_B_PIN 9
#define DATA_B_PIN 10
#define P_CHANNEL 4

#define NUM_CHIPS 4
#define BASENOTE 36 //"Nota" di base per riferimento

const int NUM_BUTTONS = 8 * NUM_CHIPS;
bool lastButtonState[NUM_BUTTONS];
byte buttonValues[NUM_CHIPS];
/*---Fine Sezione Bottoniera---*/



/*---Sezione Espressione---*/
#define EXPRESSION_PIN_1 A0 //Crescendo
#define EXPRESSION_PIN_2 A1 //I (III)
#define EXPRESSION_PIN_3 A2 //III (IV)

int lastValue[3] = {-1, -1, -1};

#define SMOOTHING 4  
#define MIDI_CONTROLLER 11  //Controller

//SW
#define LATCH_C_PIN 14 //ST_CP
#define DATA_C_PIN 15 //DS
#define CLOCK_C_PIN 16 //SH_CP
#define C_CHANNEL 1
#define C_CONTROLLER 19

//I(III)
#define LATCH_E1_PIN 2 
#define DATA_E1_PIN 3
#define CLOCK_E1_PIN 4
#define E1_CHANNEL 2
#define E1_CONTROLLER 4

//III(IV)
#define LATCH_E2_PIN 5
#define DATA_E2_PIN 6
#define CLOCK_E2_PIN 7
#define E2_CHANNEL 3
#define E2_CONTROLLER 11

/*---Fine Sezione Espressione---*/


/*---Commons---*/

/*---Fine Commons---*/

void setup() {
  /*COMMON*/
  
  /*END COMMON*/

  /*BUTTON*/
  pinMode(LATCH_B_PIN, OUTPUT);
  pinMode(CLOCK_B_PIN, OUTPUT);
  pinMode(DATA_B_PIN, INPUT);

  for(int i=0; i<NUM_CHIPS; i++)
  {
    lastButtonState[i]=false;
  }
  for(int i=0; i<NUM_CHIPS; i++)
  {
    buttonValues[i]=255;
  }
  /*END BUTTON*/

  /*EXPRESSION*/
  pinMode(EXPRESSION_PIN_1, INPUT);
  pinMode(EXPRESSION_PIN_2, INPUT);
  pinMode(EXPRESSION_PIN_3, INPUT);

  pinMode(CLOCK_C_PIN, OUTPUT);
  pinMode(LATCH_C_PIN, OUTPUT);
  pinMode(DATA_C_PIN, OUTPUT);
  /*END EXPRESSION*/
}

void loop() {
  updateStaffa(EXPRESSION_PIN_1, CLOCK_C_PIN, LATCH_C_PIN, DATA_C_PIN, C_CHANNEL, C_CONTROLLER);
  updateStaffa(EXPRESSION_PIN_2, CLOCK_E1_PIN, LATCH_E1_PIN, DATA_E1_PIN, E1_CHANNEL, E1_CONTROLLER);
  updateStaffa(EXPRESSION_PIN_3, CLOCK_E2_PIN, LATCH_E2_PIN, DATA_E2_PIN, E2_CHANNEL, E2_CONTROLLER);
  //updateBottoniera(CLOCK_B_PIN, LATCH_B_PIN, DATA_B_PIN, P_CHANNEL);

}

//---------------------------------------------BUTTON-------------------------------------------------------
void updateBottoniera(int clockPin, int latchPin, int dataPin, int midiChannel)
{
  digitalWrite(latchPin, LOW);
  delayMicroseconds(5);
  digitalWrite(latchPin, HIGH);

  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttonValues[i] = shiftIn(dataPin, clockPin, MSBFIRST);
  }

  for (int i = 0; i < NUM_BUTTONS; i++) {
    int chipIndex = i / 8;
    int bitIndex = 7 - (i % 8);

    bool current = bitRead(buttonValues[chipIndex], bitIndex) == 0;

    if (current != lastButtonState[i]) {
      lastButtonState[i] = current;

      if (current) {
        noteOn(midiChannel, BASENOTE + i, 100);
      } else {
        noteOff(midiChannel, BASENOTE + i, 0);
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
//----------------------------------------------------------------------------------------------------------


//---------------------------------------------EXPRESSION---------------------------------------------------
void updateStaffa(int analogPin, int clockPin, int latchPin, int dataPin, int midiChannel, int controller) {
  int index;
  switch (analogPin) {
    case A0:
      index = 0;
      break;
    case A1:
      index = 1;
      break;
    case A2:
      index = 2;
      break;
  }

  int analogValue = analogRead(analogPin);
  int midiValue = map(analogValue, 0, 1023, 0, 127);

  if (abs(midiValue - lastValue[index]) >= SMOOTHING) {
    lastValue[index] = midiValue;
    
    
    sendControlChange(controller, midiValue, midiChannel);
    updateLuci(clockPin, latchPin, dataPin, midiValue);
  }
}

void updateLuci(int clockPin, int latchPin, int dataPin, int value) {
  byte binaryValue = getBinaryValuePerStaffa(value);

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, binaryValue);
  digitalWrite(latchPin, HIGH);
}

byte getBinaryValuePerStaffa(int value) {
  if (value >= 0 && value <= 14) {
    return B00000000;
  } else if (value >= 15 && value <= 29) {
    return B00000001;
  } else if (value >= 30 && value <= 44) {
    return B00000011;
  } else if (value >= 45 && value <= 59) {
    return B00000111;
  } else if (value >= 60 && value <= 74) {
    return B00001111;
  } else if (value >= 75 && value <= 89) {
    return B00011111;
  } else if (value >= 90 && value <= 104) {
    return B00111111;
  } else if (value >= 105 && value <= 112) {
    return B01111111;
  } else {
    return B11111111;
  }
}

void sendControlChange(byte control, byte value, byte channel) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};

  MidiUSB.sendMIDI(event);
  MidiUSB.flush();
}


//----------------------------------------------------------------------------------------------------------

