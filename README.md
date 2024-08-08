# MIDIsonar
## An Arduino based MIDI Gesture Controller 
The MidiSonar is a MIDI Gesture Controller based on an Arduino UNO with two HC-SR04 sonar sensors.

![MIDIsonar Welcome](https://github.com/PaulGoes/MIDIsonar/blob/master/Images/MIDIsonar%20Usage.png)

With MIDIsonar you have two independent sonar based gesture controllers that let you create music by waving your hands up and down above the controllers.

Each gesture controller uses a sonar sensor that measures the distance between itself and the hand above it using sonar waves (much like bats do to orientate themselves). By varying the distance between your hands and the sonar sensors you generate musical information that can be send over the MIDI Out to any MIDI capable device like your synthesizer, drum machine or DAW.

MIDISonar lets you send the following MIDI information:

- **MIDI Control Changes using a selectable CC number**
- **MIDI Notes using a selectable root note and scale**
- **MIDI Chords using a selectable root chord and progression**
- **MIDI Pitchbend in either up, down or up&down direction**
