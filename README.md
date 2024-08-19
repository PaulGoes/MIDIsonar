# MIDIsonar
## An Arduino based MIDI Gesture Controller 
The MIDIsonar is a MIDI Gesture Controller based on an Arduino UNO with two HC-SR04 ultrasonic sensors.

![MIDIsonar Welcome](https://github.com/PaulGoes/MIDIsonar/blob/master/Images/MIDIsonar%20Usage.png)

With MIDIsonar you have ***two independent sonar based gesture controllers*** that let you create music by waving your hands up and down above the controllers.

Each gesture controller uses a ultrasonic sensor that measures the distance between itself and the hand above it using sonar waves (much like bats do to orientate themselves). 

By varying the distance between your hands and the ultrasonic sensors you ***generate musical information*** that can be send over the MIDI Out to any MIDI capable device like your synthesizer, drum machine or DAW.

<br/>

### MIDI information generated

MIDISonar lets you generate the following MIDI information:
- **MIDI Control Changes - Change the sound characteristics:**
    - Selectable MIDI CC number.
    - Selectable value range (min and max).
- **MIDI Notes - Play melodies and scales:**
    - Selectable root note.
    - Selectable scale (12 different available)
- **MIDI Chords - Play harmonies and progressions:**
    - Selectable root note.
    - Selectable chord progression (9 different available)
- **MIDI Pitchbend - Change the tone height of the sound:**
    - Selectable direction (up, down, up&down).
    - Selectable value range (min and max) and neutral zone .

<br/>

### Controller Configuration

For both controllers the following settings are available:
- **Active status:**
    - Set the controller active or not so you can use both or either one of the controllers.
- **Operational range:**
    - Set the range (minimum and maximum) in cm's in which the controller is operational (from 5cm to 80cm).
- **Polarity:**
    - Set the polarity of the controller, e.g. CC values increase or decrease when your hand is raised.
- **MIDI Channel:**
    - Set the MIDI channel of the controller so you can use both on the same or on different MIDI channels.

<br/>

### Musical usage examples

If set to two different MIDI channels the MIDIsonar can e.g. be used to:
- Play chord progressions with the left hand controlling a pad sound and play note melodies with the right hand controlling a lead sound.
- Control the filter of a running sequence sound with the left hand and play note melodies with the right hand controlling a lead sound.

If set to the same MIDI channel the MIDIsonar can e.g. be used to:
- Control the filter of a sound with the left hand and control the resonance of the same sound with the right hand.
- Control the volume of a sound with the left hand and control the pitch of the same sound to play the sound like a theremin.
- Trigger a single bass drum note with the left hand and trigger another single snare drum note with the right hand to play air drums.

<br/>

## Build it Yourself

You can build the MIDIsonar yourself. Just build it up like shown below and load the MIDIsonar.ino into the Arduino UNO. 

<br/>

### Schematic

Connect the various components as shown in this schematic diagram.

![MIDIsonar Schematic](https://github.com/PaulGoes/MIDIsonar/blob/master/MIDIsonar%20Schematic.jpg)

<br/>

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

If you want to build it into an enclosure you might additionally need wood, paint and screws (wooden enclosure or 3D printing equipment (3D printed plastic enclosure).

<br/>

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

<br/>

### Software

The software uses the following libraries:
- ``LiquidCrystal.h`` - to interface with the LCD screen
- ``NewPing.h`` - to interface with the ultrasonic sensors

The software consists of a ``setup()`` that initializes the MIDIsonar and displays the welcome message and a ``loop()`` that cycles through the three operational modes Standby, Setup and Play.

The software contains the following functions/subroutines:

- ``MODEstandby()`` - implements the Standby mode
- ``MODEsetup()`` - implements the Setup mode
- ``MODEplay()`` - implements the Play mode
- ``value2string()`` - converts values to strings for displaying purposes
- ``MIDIchord()`` - play chords by decomposing to individual notes
- ``MIDIpitchbend()`` - play pitchbend
- ``MIDImessage()`` - send individual MIDI message

The sketch uses 15010 bytes (46%) of program storage space. Global variables use 1894 bytes (92%) of dynamic memory.

<br/>

### Change the default controller setup

The MIDIsonar uses a default setup for the controllers. This default setup is loaded when the MIDIsonar is turned on. You can change this defaults if you want the MIDIsonar to start up with a different setup.

The default setup for the two controllers is defined at [line 137](https://github.com/PaulGoes/MIDIsonar/blob/b069583abb0d174fe831ef4dbfc4291921db8744/MIDIsonar.ino#L137) of the code.

```
/* initialize the data array with default values */
int value[2][18] = {
  { 1, 1, 1, 5, 50, 1, 7, 0, 127, 1, 60, 24, 1, 60, 14, 3, 3, 1 },	/* controller A: Active | CC 7 Volume over full range 0-127               */
  { 1, 2, 1, 5, 50, 1, 7, 0, 127, 1, 60, 24, 1, 60, 14, 3, 3, 1 }  	/* controller B: Active | NOTE Chromatic scale with root C3 with 24 steps */
```

The settings map to the following: 
| Controller| ACTV | TYPE | CHNL | LDST | HDST | POLR | CNTR | LVAL | HVAL | SCAL | ROOT | RANG | PROG | ROOT | RANG | TYPE | NTRL | PBFX |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
|  |  |  |  |  |  |  | CC | CC | CC | NOT | NOT | NOT | CHD | CHD | CHD | PB | PB | PB |
| A | <code style="color : blue">1</code> | 1 | 1 | 5 | 50 | 1 | 7 | 0 | 127 | 1 | 60 | 24 | 1 | 60 | 14 | 3 | 3 | 1 |
| B | 1 | 2 | 1 | 5 | 50 | 1 | 7 | 0 | 127 | 1 | 60 | 24 | 1 | 60 | 14 | 3 | 3 | 1 |

The defaults of both controllers are basically the same:

- Both send on MIDI channel 1 (CHNL) with a working range from 5 cm (LDST) to 50 cm (HDST).
- Controller A is set to TYPE 1 which means to send MIDI CC information. So it uses the config for the CC information: sending CC 7 (CC CNTR) over the full range of 0 (CC LVAL) to 127 (CC HVAL).
- Controller B is set to type 2 which means to send MIDI NOTE information. So it uses the config for the NOT information: using the chromatic scale (NOT SCAL) with root note C3 (NOT ROOT) and a range of 24 notes (NOT RANG).

The file [MIDIsonar datastructure pdf](https://github.com/PaulGoes/MIDIsonar/blob/master/MIDIsonar%20Datastructure.pdf) defines the settings and the values that can be used.

<br/>

## Photo's

Here are some photo's of my MIDIsonar to give you inspiration to design you own enclosure.

![MIDIsonar Photos](https://github.com/PaulGoes/MIDIsonar/blob/master/Images/MIDISonar%20Photos.png)

<br/>

## User Manual

When you power on the MIDIsonar it starts up with the welcome screen that displays the software version.

![MIDIsonar Welcome](https://github.com/PaulGoes/MIDIsonar/blob/master/LCD%20Screen%20Images/Welcome%20Screen.png)

After a few seconds the MIDIsonar switches to the Standby Mode.

![MIDIsonar Standby](https://github.com/PaulGoes/MIDIsonar/blob/master/LCD%20Screen%20Images/MIDIsonar%20Standby.png)

<br/>

### Select Modes

By pressing the :red_square:**MODE** button you can cycle trought the three modes Standby, Setup and Play.

![MIDIsonar Mode Select](https://github.com/PaulGoes/MIDIsonar/blob/master/Images/MIDIsonar%20Mode%20Select.png)

<br/>

### Play Mode

The MIDIsonar has a default setup and you can immediately use it by going to the Play mode. The default setup is:

MIDI controller A: 
- Active with a range from 5cm to 50cm, positive polarity.
- MIDI channel 1.
- Sending MIDI CC 7 (Volume) over the full range of 0 to 127.
  
MIDI controller B: 
- Active with a range from 5cm to 50cm, positive polarity.
- MIDI channel 1.
- Sending MIDI NOTES using a chromatic scale with root note C3 divided in 24 steps.

> [!NOTE]
> When in Play mode the activity LED's for controller A and controller B indicate whether the controller is active (lit) or not (dimmed).

When in Play mode the LCD screen gives visual feedback on the controls. For the default setup it shows the following information.

![MIDIsonar Play Default](https://github.com/PaulGoes/MIDIsonar/blob/master/LCD%20Screen%20Images/Play%20Default.png)

You can read from this screen that controller A is now sending CC 7 with a value 32 on MIDI channel 1 and controller B is now sending note G#3 also on MIDI channel 1.

<br/>

### Setup Mode

In Setup mode you can configure the settings for both controllers.

When you go into Setup Mode for the first time you are presented the first setup screen for controller A.

![MIDIsonar Setup 1](https://github.com/PaulGoes/MIDIsonar/blob/master/LCD%20Screen%20Images/MIDIsonar%20Setup%201.png)

The cursor (a thin line at the bottom) indicates which setting is currently selected. 

You can select different settings by pressing the :blue_square:**PREV** button (previous) and :blue_square:**NEXT** button (next). By pressing these buttons you cycle through the settings of controller A and controller B: page A1 -> page A2 -> page A3 -> page B1 -> page B2 -> page B3.

> [!NOTE]
> When you press the :blue_square:**NEXT** button at the last setting of controller B you will go to the first setting of controller A.<br/>
>
> When you press the :blue_square:**PREV** button at the first setting of controller A you will go to the last setting of controller B.

You can change the value of the selected setting by pressing the :black_large_square:**DEC** button (decrease) and :black_large_square:**INC** button (increase). 

> [!TIP]
> If you keep either :black_large_square:**DEC** or :black_large_square:**INC** button pressed it will scroll faster through the values.

> [!NOTE]
> When you return to Setup Mode after visiting Play mode you will return to the most recently edited setting.

<br/>

### Setup Screen organization

Each controller is configured by 9 settings arranged in 3 pages of 3 settings each. The third page has settings that are dependent of the chosen type of operation on page 1.

<br/>

### Settings of the first Setup screen

![MIDIsonar Setup 1](https://github.com/PaulGoes/MIDIsonar/blob/master/LCD%20Screen%20Images/MIDIsonar%20Setup%201.png)

- **ACTV:** Active
  - ON = Active
  - OFF = Inactive
- **TYPE:** Type of operation
  - CC = Send MIDI CC changes
  - NOT = Send MIDI Notes
  - CHD = Send MIDI Chords
  - PB = Send MIDI Pitchbend
- **CHNL:** MIDI Channel
  - 1-16 = MIDI Channel number

<br/>

### Settings of the second Setup screen

![MIDIsonar Setup 2](https://github.com/PaulGoes/MIDIsonar/blob/master/LCD%20Screen%20Images/MIDIsonar%20Setup%202.png)

- **LDST:** Low Distance
  - 5-80 = Low end of the operating range in cm (in steps of 5)
- **HDST:** High Distance
  - 5-80 = High end of the operating range in cm (in steps of 5)
- **POLR:** Polarity
  - POS = Positive mapping
  - NEG = Negative mapping
    
> [!NOTE]
> The HDST must be higher than the LDST. This is enforced when setting the values. 

The following image illustrates the effect of using positive and negative polarity.

![MIDIsonar Polarity](https://github.com/PaulGoes/MIDIsonar/blob/master/Images/MIDIsonar%20Polarity.png)

<br/>

### Settings of the third Setup screen - MIDI CC Changes

![MIDIsonar Setup 3 CC](https://github.com/PaulGoes/MIDIsonar/blob/master/LCD%20Screen%20Images/MIDIsonar%20Setup%203-CC.png)

- **CNTR:** Controller number
  - 0-127 = MIDI CC number
- **LVAL:** Low Value
  - 0-127 = Lowest value generated
- **HVAL:** High Value
  - 0-127 = Highest value generated

> [!NOTE]
> The HVAL must be higher than the LVAL. This is enforced when setting the values. 

> [!TIP]
> The use of MIDI CC is synthesizer specific. So the manufacturer of the synthesizer determines which CC value controls what parameter of the sound engine. So take a look at the manual of the synthesizer for a mapping of the CC values. Most likely there will be an appendix with a name of MIDI controller list (or equivalent).

The following image illustrates controlling the filter cutoff (controller A) and filter resonance (controller B) on a Novation KS-rack synthesizer module. 

![MIDIsonar Control Chnage](https://github.com/PaulGoes/MIDIsonar/blob/master/Images/MIDIsonar%20Control%20Change.png)

<br/>

### Settings of the third Setup screen - MIDI Notes

![MIDIsonar Setup 3 NOT](https://github.com/PaulGoes/MIDIsonar/blob/master/LCD%20Screen%20Images/MIDIsonar%20Setup%203-NOTE.png)

- **SCAL:** Musicial note scale
  - CHR = Chromatic scale
  - MAJ = Major scale
  - MIN = Minor scale
  - MMI = Melodic Minor scale
  - HMI = Harmonic Minor scale
  - WHL = Whole Tone scale
  - MAB = Major Blues scale
  - MIB = Minor Blues scale
  - MA5 = Major Pentatonic scale
  - MI5 = Minor Pentatonic scale
  - 8WH = Octatonic Whole Half scale
  - 8HW = Octatonic Half Whole scale
- **ROOT:** Root note
  -  C0-B8 - Root note for the scale
-  **RANG:** Number of scale notes to be played

The following image shows the notes of the different scales for the root note C.

![MIDISonar Scales](https://github.com/PaulGoes/MIDIsonar/blob/master/Images/MIDIsonar%20Scales.png)

<br/>

### Settings of the third Setup screen - MIDI Chords

![MIDIsonar Setup 3 CHD](https://github.com/PaulGoes/MIDIsonar/blob/master/LCD%20Screen%20Images/MIDIsonar%20Setup%203-CHRD.png)

- **PROG:** Musical chord progression
  - MAJ = Major chord progression
  - MIN = Minor chord progression
  - MAB = Major Blues 7th chord progession
  - MIB = Minor Blues 7th chord progression
  - POP = Pop chord progression
  - JAZ = Jazz chord progression (undocumented)
  - MOD = Modal chord progression (undocumented)
  - GSP = Gospel chord progression (undocumented)
  - PBL = Pachelbel chord progression (undocumented)
- **ROOT:** Root note
  - C0-B8 - Root note for the chord progression
- **RANG:** Number of progression chords to be played 

The following image shows the chords of the different progressions for the root note C.

![MIDIsonar Progressions](https://github.com/PaulGoes/MIDIsonar/blob/master/Images/MIDIsonar%20Chord%20Progressions.png)

<br/>

### Settings of the third Setup screen - MIDI Pitchbend

![MIDIsonar Setup 3 PB](https://github.com/PaulGoes/MIDIsonar/blob/master/LCD%20Screen%20Images/MIDIsonar%20Setup%203-PB.png)

- **TYPE:** Pitchbend type
  - UP = Bend the pitch up
  - DWN = Bend the pitch down
  - U&D = Bend the pitch up and down  
- **NTRL:** Neutral zone size
  - SML = Small
  - MED = Medium
  - LRG = Large
- **PBFX:** Pitchbend effects
  - FLW = Flowing pitch bend
  - STP = Stepping pitch bend  

The following image shows the effect of using the pitchbend types and neutral zone sizes.

![MIDIsonar Pitchbend](https://github.com/PaulGoes/MIDIsonar/blob/master/Images/MIDIsonar%20Pitchbend.png)

> [!NOTE]
> The neutral zone is comparable to a pitchbend wheel in its neutral/center position. The pitchbend maximum is comparable to a pitchbend wheel completely up or down. As with the pitchbend wheel this doesn't say anything about the actual pitchbend in semitones. The semitone range of the pitchbend is determined by the setup of the synthesizer. On most synthesizers it is even possible to setup individual semitone ranges for pitchbend up and pitchbend down. 
