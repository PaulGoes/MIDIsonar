# MIDIsonar
## An Arduino based MIDI Gesture Controller 
The MidiSonar is a MIDI Gesture Controller based on an Arduino UNO with two HC-SR04 sonar sensors.

![MIDIsonar Welcome](https://github.com/PaulGoes/MIDIsonar/blob/master/Images/MIDIsonar%20Usage.png)

With MIDIsonar you have ***two independent sonar based gesture controllers*** that let you create music by waving your hands up and down above the controllers.

Each gesture controller uses a sonar sensor that measures the distance between itself and the hand above it using sonar waves (much like bats do to orientate themselves). 

By varying the distance between your hands and the sonar sensors you ***generate musical information*** that can be send over the MIDI Out to any MIDI capable device like your synthesizer, drum machine or DAW.

MIDISonar lets you generate the following MIDI information:

- **MIDI Control Changes - Change the sound characteristics:**
    - Selectable MIDI CC number
    - Selectable value range (min and max)
- **MIDI Notes - Play melodies and scales:**
    - Selectable root note
    - Selectable scale
      - Chromatic
      - Major
      - Minor
      - Melodic Minor
      - Harmonic Minor
      - Whole Tone
      - Major Blues
      - Minor Blues
      - Major Pentatonic
      - Minor Pentatonic
      - Octatonic Whole Half
      - Octatonic Half Whole
- **MIDI Chords - Play harmonies and progressions:**
    - Selectable root note
    - Selectable chord progression
      - Major
      - Minor
      - Major Blues 7th
      - Minor Blues 7th
      - Pop
      - Jazz
      - Modal
      - Gospel
      - Pachelbel  
- **MIDI Pitchbend - Change the tone height of the sound:**
    - Selectable direction (up, down, up&down)
    - Selectable value range (min and max) and neutral zone 

For both controllers the operating range can be set to operate on their own MIDI channel. 

If set to two different MIDI channels this can e.g. be used to:
- Play chord progressions with the left hand controlling a pad sound and play note melodies with the right hand controlling a lead sound.
- Control the filter of a running sequence sound with the left hand and play note melodies with the right hand controlling a lead sound.

If set to the same MIDI channel this can e.g. be used to:
- Control the filter of a sound with the left hand and control the resonance of the same sound with the right hand.
- Control the volume of a sound with the left hand and control the pitch of the same sound to play the sound like a theremin.
- Trigger a single bass drum note with the left hand and trigger another single snare drum note with the right hand to play air drums.
