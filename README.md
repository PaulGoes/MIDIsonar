# MIDIsonar
## An Arduino based MIDI Gesture Controller 
The MidiSonar is a MIDI Gesture Controller based on an Arduino UNO with two HC-SR04 ultrasonic sensors.

![MIDIsonar Welcome](https://github.com/PaulGoes/MIDIsonar/blob/master/Images/MIDIsonar%20Usage.png)

With MIDIsonar you have ***two independent sonar based gesture controllers*** that let you create music by waving your hands up and down above the controllers.

Each gesture controller uses a ultrasonic sensor that measures the distance between itself and the hand above it using sonar waves (much like bats do to orientate themselves). 

By varying the distance between your hands and the ultrasonic sensors you ***generate musical information*** that can be send over the MIDI Out to any MIDI capable device like your synthesizer, drum machine or DAW.

### MIDI information generated

MIDISonar lets you generate the following MIDI information:
- **MIDI Control Changes - Change the sound characteristics:**
    - Selectable MIDI CC number.
    - Selectable value range (min and max).
- **MIDI Notes - Play melodies and scales:**
    - Selectable root note.
    - Selectable scale:
      - Chromatic.
      - Major.
      - Minor.
      - Melodic Minor.
      - Harmonic Minor.
      - Whole Tone.
      - Major Blues.
      - Minor Blues.
      - Major Pentatonic.
      - Minor Pentatonic.
      - Octatonic Whole Half.
      - Octatonic Half Whole.
- **MIDI Chords - Play harmonies and progressions:**
    - Selectable root note.
    - Selectable chord progression:
      - Major.
      - Minor.
      - Major Blues 7th.
      - Minor Blues 7th.
      - Pop.
      - Jazz.
      - Modal.
      - Gospel.
      - Pachelbel.  
- **MIDI Pitchbend - Change the tone height of the sound:**
    - Selectable direction (up, down, up&down).
    - Selectable value range (min and max) and neutral zone .

### Controller Configuration

For both controllers the following settings are available:
- **Active status:**
    - Set the controller active or not so you can use both or either one of the controllers.
- **Operational range:**
    - Set the range (minimum and maximum) in cm's in which the controller is operational (from 5 cm to 80 cm).
- **Polarity:**
    - Set the polarity of the controller, e.g. CC values increase or decrease when your hand is raised.
- **MIDI Channel:**
    - Set the MIDI channel of the controller so you can use both on the same or on different MIDI channels.

### Musical usage examples

If set to two different MIDI channels the MIDIsonar can e.g. be used to:
- Play chord progressions with the left hand controlling a pad sound and play note melodies with the right hand controlling a lead sound.
- Control the filter of a running sequence sound with the left hand and play note melodies with the right hand controlling a lead sound.

If set to the same MIDI channel the MIDIsonar can e.g. be used to:
- Control the filter of a sound with the left hand and control the resonance of the same sound with the right hand.
- Control the volume of a sound with the left hand and control the pitch of the same sound to play the sound like a theremin.
- Trigger a single bass drum note with the left hand and trigger another single snare drum note with the right hand to play air drums.

## Build it Yourself

You can build the MIDIsonar yourself. Just build it up like shown below and load the MIDIsonar.ino into the Arduino UNO. 

### Schematic

Connect the various components as shown in this schematic diagram.

![MIDIsonar Schematic](https://github.com/PaulGoes/MIDIsonar/blob/master/MIDIsonar%20Schematic.jpg)

### Bill Of Materials

To build the MIDIsonar you need the following components.

| Component  | Quantity |
| ------------- | ------------- |
| Arduino UNO Microcontroller | 1x  |
| HC-RC04 ULtrasonic sensor  | 2x  |
| HD44780 16x2 LCD screen | 1x |
| MIDI Din Connector | 1x |
| Push buttons | 5x |
| LED diode | 2x |
| Potentiometer 10kOhm | 1x |
| Resistor 220Ohm | 5x |
| Resistor 10kOhm | 5x |
| Breadboard & Wires | 1x |

If you want to build a more permanent solution you might want to solder the components and you additionally need protyping PCB boards and soldering equipment.

If you want to build it into a case you might additionally need wood, paint and screws (wooden case) or 3D printing equipment (3D printed plastic case).

### GPIO Pin definitions

The MIDIsonar defines the GPIO pins of the Arduino UNO as follows.

| GPIO | Usage | GPIO | Usage |
| ------------- | ------------- | ------------- | ------------- |
| TXD | Serial MIDI Out | D10 | Activity LED B |
| D2 | LCD Data bit 7 |  D11 | LCD Clock |
| D3 | LCD Data bit 6 | D12 | LCD Register Select |
| D4 | LCD Data bit 5 | D13 | Activity LED A |
| D5 | LCD Data bit 4 | A1 | Button MODE |
| D6 | Sensor B Trigger | A2 | Button PREV |
| D7 | Sensor B Echo | A3 | Button NEXT |
| D8 | Sensor A Trigger | A4 | Button INC |
| D9 | Sensor A Echo | A5 | Button DEC |

In the software the pins are defined at the beginning of the code. If you use another type of Arduino than an Arduino UNO, you might have to change the pin numbers to the corresponding ones for the type that you use.

### Software

The software uses the following libraries:
- LiquidCrystal.h - to interface with the LCD screen
- NewPing.h - to interface with the ultrasonic sensors

The sketch uses 15010 bytes (46%) of program storage space. Global variables use 1894 bytes (92%) of dynamic memory.

## User Manual

When you power on the MIDI Sonar it starts up with the welcome screen that displays the software version.

![MIDIsonar Welcome](https://github.com/PaulGoes/MIDIsonar/blob/master/LCD%20Screen%20Images/Welcome%20Screen.png)

After a few seconds the MIDI Sonar switches to the Standby Mode.

![MIDIsonar Standby](https://github.com/PaulGoes/MIDIsonar/blob/master/LCD%20Screen%20Images/MIDIsonar%20Standby.png)

By pressing the MODE button you can cycle trought the three modes Standby, Setup and Play.

![MIDIsonar Mode Select](https://github.com/PaulGoes/MIDIsonar/blob/master/Images/MIDIsonar%20Mode%20Select.png)
