# HauptwerkProject

**STATUS: on going**

Progetto open source per la gestione di una pedaliera MIDI, staffe di espressione e bottoniera compatibili con [Hauptwerk](https://www.hauptwerk.com/), basato su Arduino Pro Micro (ATmega32U4). Il sistema invia messaggi MIDI tramite USB grazie alla libreria `MIDIUSB.h`.

## Contenuto del Progetto

- pedaliera a 30/32 note tramite shift register 74HC165
- **3 Staffe di espressione** analogiche con LED indicatori a segmenti (shift register 74HC595)
- Bottoniera per pedaletti e pulsantini sulle tastiere
- **Output MIDI-USB** compatibile con Hauptwerk, GrandOrgue, o qualsiasi DAW

## Requisiti Hardware

- Arduino Pro Micro (ATmega32U4) – obbligatorio per la libreria MIDIUSB X2
- Shift register 74HC165 (input bottoniera) X8
- Shift register 74HC595 (output LED staffe) X3
- potenziometri lineari o sensori analogici per staffe X3
- contattiera per pedaliera di tipo reed switch o sensori hall X30/x32
- LED per visualizzazione livelli staffa (opzionale) X24

### Librerie Arduino
Assicurati di avere installato:

- [`MIDIUSB`](https://github.com/arduino-libraries/MIDIUSB) (via Library Manager)



## 👤 Autore

**Francesco Patti**  
[GitHub](https://github.com/Alpha004game)


🔗 *Compatibile con Hauptwerk™, GrandOrgue, Reaper, Kontakt, e qualsiasi DAW/MIDI host.*

