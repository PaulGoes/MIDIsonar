/* MIDIsonar                                                                    */
/* ---------------------------------------------------------------------------- */
/* Author:  Paul Goes                                                           */
/* Version: 3.1                                                                 */
/* Date:    19-03-2024                                                          */
/* ---------------------------------------------------------------------------- */ 
/* Revision history:                                                            */
/*                                                                              */
/* v0.1    : Initial version. Only contains simple LCD functionality that       */
/*           enables to scroll through the different modes: STANDBY, SETUP and  */
/*           PLAY.                                                              */
/*                                                                              */
/* v0.2    : Added data array that holds the setup values. Initialized the      */
/*           data array with the default values. Implemented temporary PLAY     */
/*           mode functionality that shows the contents of the array on the     */
/*           LCD display.                                                       */
/*                                                                              */
/* v0.3    : Added function that converts integer value to string in order to   */
/*           display meaningful values on the LCD.                              */
/*                                                                              */
/* v0.4    : Added setup mode. Navigation through the six setup screens. Setup  */
/*           screens display the initalizaton data from the data array.         */
/*           Changing the data values is not yet implemented.                   */
/*                                                                              */
/* v0.5    : Added the functionality to change values in setup mode.  Added     */
/*           extensive in-code comments to document the logic.                  */
/*                                                                              */
/* v0.6    : Solved bug #1 'LDST and HDST displays wrong values when LDST>HDST  */
/*           and HDST<LDST' and solved bug #2 'Jitter at certain positions of   */
/*           the value dial'.                                                   */
/*                                                                              */
/* v0.7    : Implemented play mode. Implemented is only sonar controller A.     */
/*           For this controller the complete functionality is implemented.     */
/*           Both controller types (MIDI notes and MIDI CC) are implemented.    */
/*                                                                              */
/* v0.8    : Solved bug #3 'Alternating adjacent notes when playing notes' and  */
/*           solved bug 'No NoteOff Midi message sent when hand is out of       */
/*           range'.                                                            */
/*                                                                              */
/* v0.9    : Implemented play mode for sonar controller B.                      */
/*                                                                              */
/* v1.0    : Implemented setting values by two new buttons for INC and DEC.     */
/*           The value potentiometer is removed and no longer functional.       */
/*                                                                              */
/* v1.1    : Switched pins for sonar controller A and sonar controller B to     */
/*           fix the build error in which the controllers are build into the    */
/*           wrong physical slots.                                              */ 	
/*                                                                              */
/* v2.0    : Changed implementation of the MIDI notes functionality. It is now  */
/*           possible to select a scale to use when playing the notes. The      */
/*           scale is played starting from a specified root note which          */
/*           determines the key that the scale is played in.                    */
/*                                                                              */
/* v2.1    : Added implementation of the MIDI chords functionality. It is now   */
/*           possible to select a progression to use when playing the chords.   */
/*           The progression is played starting from a specified root note      */
/*           which determines the key the progression is played in.             */
/*                                                                              */
/* v2.2    : Added implementation of additional MIDI chord progressions.        */
/*                                                                              */
/* v2.3    : Added implementation of the MIDI pitchbend functionality. It is    */
/*           possible to select a whether bending up only, bending down only    */
/*           or bending up and down. In all three settings there is a neutral   */
/*           zone of configurable size that corresponds to the central pitch.   */
/*                                                                              */
/* v2.4    : Enhanced menu handling. When returning to setup it returns to the  */
/*           previously selected menu item. When selecting NEXT at the last     */
/*           menu item it selects the first menu item and when selecting PREV   */
/*           at the first menu item it selects the last menu item.              */ 
/*                                                                              */
/* v2.5    : Solved issue with the MIDI chord progressions. The new chord was   */
/*           played with the wrong chord type and the note off was played with  */
/*           the correct chord type resulting in notes sustaining.              */
/*                                                                              */
/* v3.0    : Added MIDI reset when leaving play mode. Notes that are sounding   */
/*           are released and the pitch bend is reset to neutral position.      */
/*                                                                              */
/* v3.1    : Enhanced playing of note scales and chord progressions. In         */
/*           previous versions it played one note/chord outside of the range.   */
/*           With a range of 1 it played 2 notes/chords, with the range set     */
/*           to maximum it tried to play a note outside of the arrays.          */
/*           With range of a single note it is now possible to trigger single   */
/*           notes/chords with more precision.                                  */
/* ---------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/* Arduino Pin connections:                                                     */
/*    TXD - MIDI OUT Serial               D10 - Sonar B Led                     */
/*    D2  - LCD Bit 7 (pin 14)            D11 - LCD Clock (pin 6)               */
/*    D3  - LCD Bit 6 (pin 13)            D12 - LCD Reg Select (pin 4)          */    
/*    D4  - LCD Bit 5 (pin 12)            D13 - Sonar A Led                     */
/*    D5  - LCD Bit 4 (pin 11)            A0  - Potmeter Value (deprecated)     */
/*    D8  - Sonar A Trigger               A1  - Button MODE SELECT              */
/*    D9  - Sonar A Echo                  A2  - Button MENU PREV                */
/*    D6  - Sonar B Trigger               A3  - Button MENU NEXT                */
/*    D7  - Sonar B Echo                  A4  - Button VALUE INC                */
/*                                        A5  - Button VALUE DEC                */
/* ---------------------------------------------------------------------------- */


/* include the library code */
#include <LiquidCrystal.h>
#include <NewPing.h>

/* initialize the library with the numbers of the interface pins */
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/* initialize the push buttons */
const int buttonPin1 = 15;   /* button 1 connected to pin 15 (analog A1) MODE SELECT */
const int buttonPin2 = 16;   /* button 2 connected to pin 16 (analog A2) MENU PREV   */
const int buttonPin3 = 17;   /* button 3 connected to pin 17 (analog A3) MENU NEXT   */
const int buttonPin4 = 18;   /* button 4 connected to pin 18 (analog A4) VALUE INC   */
const int buttonPin5 = 19;   /* button 5 connected to pin 19 (analog A5) VALUE DEC   */

/* initialize the LEDs */
const int ledPinA = 13;      /* activity LED used for controller A connected to pin D13 */
const int ledPinB = 10;      /* activity LED used for controller B connected to pin D10 */

/* initialize the potentiometer */
const int potPin1 = 0;       /* potentiometer 1 connected to pin 1 (analog A0) VALUE */

int buttonState = 0;

/* initialize the start menu position */
int menu_id = 0;        /* menu id of the selected menu item */
int menu_id_old = 3;    /* menu id of the previous menu item */

/* initialize the Sonars */
#define triggerPinA   8      /* trigger pin sonar sensor A connected to pin D8 */
#define echoPinA      9      /* echo pin sonar sensor A connected to pin D9    */
#define triggerPinB   6      /* trigger pin sonar sensor B connected to pin D6 */
#define echoPinB      7      /* echo pin sonar sensor B connected to pin D7    */

NewPing sonarA(triggerPinA, echoPinA, 100);   /* NewPing setup of sonar A pins and maximum distance */
NewPing sonarB(triggerPinB, echoPinB, 100);   /* NewPing setup of sonar A pins and maximum distance */

/* initialize the data array with default values */
int value[2][18] = {
  { 1, 1, 2, 5, 50, 1, 7, 0, 127, 1, 60, 24, 1, 60, 14, 3, 3, 1 },	/* controller A: Active | CC 7 Volume over full range 0-127               */
  { 1, 2, 2, 5, 50, 1, 7, 0, 127, 1, 60, 24, 1, 60, 14, 3, 3, 1 }  	/* controller B: Active | NOTE Chromatic scale with root C3 with 24 steps */
};

/* initialize the data array with valuetypes */
int type[18] = { 2, 3, 1, 1, 1, 4, 1, 1, 1, 5, 6, 1, 7, 6, 1, 8, 9, 10 };

/* initialize the string array with menu names */
char* menu[] = { "ACTV", "TYPE", "CHNL", "LDST", "HDST", "POLR", "CNTR", "LVAL", "HVAL", "SCAL", "ROOT", "RANG", "PROG", "ROOT", "RANG", "TYPE", "NTRL", "PBFX" };

/* initialize the data array with ping medians */
int pingmedian[4] = { 2, 3, 3, 2 };

/* initalize the data array with wiggle room */
int wiggleroom[4] = { 10, 10, 10, 0 };

/* initialize the data array with scales */
int scales[12][24] = {
  {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 },   /* Chromatic            */
  {  0,  2,  4,  5,  7,  9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28, 29, 31, 33, 35, 36, 38, 40 },   /* Major                */
  {  0,  2,  3,  5,  7,  8, 10, 12, 14, 15, 17, 19, 20, 22, 24, 26, 27, 29, 31, 32, 34, 36, 38, 39 },   /* Minor                */
  {  0,  2,  3,  5,  7,  9, 11, 12, 14, 15, 17, 19, 21, 23, 24, 26, 27, 29, 31, 33, 35, 36, 38, 39 },   /* Melodic Minor        */
  {  0,  2,  3,  5,  7,  8, 11, 12, 14, 15, 17, 19, 20, 23, 24, 26, 27, 29, 31, 32, 35, 36, 38, 39 },   /* Harmonic Minor       */
  {  0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46 },   /* Whole Tone           */
  {  0,  2,  3,  4,  7,  9, 12, 14, 15, 16, 19, 21, 24, 26, 27, 28, 31, 33, 36, 38, 39, 40, 43, 45 },   /* Major Blues          */
  {  0,  3,  5,  6,  7, 10, 12, 15, 17, 18, 19, 22, 24, 27, 29, 30, 31, 34, 36, 39, 41, 42, 43, 46 },   /* Minor Blues          */
  {  0,  2,  4,  7,  9, 12, 14, 16, 19, 21, 24, 26, 28, 31, 33, 36, 38, 40, 43, 45, 48, 50, 52, 55 },   /* Major Pentatonic     */ 
  {  0,  3,  5,  7, 10, 12, 15, 17, 19, 22, 24, 27, 29, 31, 34, 36, 39, 41, 43, 46, 48, 51, 53, 55 },   /* Minor Pentatonic     */
  {  0,  2,  3,  5,  6,  8,  9, 11, 12, 14, 15, 17, 18, 20, 21, 23, 24, 26, 27, 29, 30, 32, 33, 35 },   /* Octatonic Whole Half */
  {  0,  1,  3,  4,  6,  7,  9, 10, 12, 13, 15, 16, 18, 19, 21, 22, 24, 25, 27, 28, 30, 31, 33, 34 }    /* Octatonic Half Whole */
};

/* initialize the data array with progressions */
int progs[9][14][2] = {
  { {0,2} , {2,3} , {4,3} , {5,2} , {7,2} , {9,3} , {11,5} , {12,2} , {14,3} , {16,3} , {17,2} , {19,2} , {21,3} , {23,5} },                    /* Major           */
  { {0,2} , {2,5} , {3,2} , {5,3} , {7,3} , {8,2} , {10,2} , {12,2} , {14,5} , {15,2} , {17,3} , {19,3} , {20,2} , {22,2} },                    /* Minor           */
  { {0,8} , {5,8} , {7,8} , {12,8} , {17,8} , {19,8} , {24,8} , {29,8} , {31,8} , {36,8} , {41,8} , {43,8} , {48,8} , {53,8} },                 /* Major Blues 7th */
  { {0,10} , {5,10} , {7,10} , {12,10} , {17,10} , {19,10} , {24,10} , {29,10} , {31,10} , {36,10} , {41,10} , {43,10} , {48,10} , {53,10} } ,  /* Minor Blues 7th */
  { {0,2} , {5,2} , {7,2} , {9,3} , {12,2} , {17,2} , {19,2} , {21,3} , {24,2} , {29,2} , {31,2} , {33,3} , {36,2} , {41,2} } ,                 /* Pop             */
  { {0,9} , {2,10} , {7,8} , {12,9} , {14,10} , {19,8} , {24,9} , {26,10} , {31,8} , {36,9} , {38,10} , {43,8} , {48,9} , {50,10} } ,           /* Jazz            */
  { {0,2} , {5,2} , {10,2} , {12,2} , {17,2} , {22,2} , {24,2} , {29,2} , {34,2} , {36,2} , {41,2} , {46,2} , {48,2} , {53,2} } ,               /* Modal           */
  { {0,2} , {2,3} , {4,3} , {5,2} , {7,2} , {9,3} , {11,5} , {12,2} , {14,3} , {16,3} , {17,2} , {19,2} , {21,3} , {23,5} } ,                   /* Gospel          */
  { {0,2} , {4,3} , {5,3} , {7,3} , {9,2} , {12,2} , {16,3} , {17,3} , {19,3} , {21,2} , {24,2} , {28,3} , {29,3} , {31,3} }                    /* Pachelbel       */
};

/* ---------------------------------------------------------------------------- */
/* Setup portion of the Arduino program.                                        */
/* ---------------------------------------------------------------------------- */

void setup() 
{
  /* initialize serial MIDI OUT interface with the correct baud rate */
  Serial.begin(31250);
  
  /* set up the LCD's number of columns and rows */
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(ledPinA, OUTPUT);
  pinMode(ledPinB, OUTPUT);
  
  /* set up the LCD screen as 2 lines of 16 characters */
  lcd.begin(16, 2);

  /* clear the LCD display */
  lcd.clear();

  /* display productname and version */
  lcd.setCursor(0, 0);
  lcd.print("MIDIsonar   v3.1");

  delay(1000);

  /* display the welcome message */
  for(int counter=0; counter<3; counter++)
  {
      lcd.setCursor(0,1); lcd.print("by Paul Goes");
      delay(750);

      lcd.setCursor(0,1); lcd.print("            ");
      delay(750);
  }

  lcd.clear();

}


/* ---------------------------------------------------------------------------- */
/* Loop portion of the Arduino program.                                         */
/* ---------------------------------------------------------------------------- */

void loop() 
{
    /* cycle through the three modes */
    MODEstandby();     
    MODEsetup();      
    MODEplay();       
}


/* ---------------------------------------------------------------------------- */
/* MODEstandby():                                                               */
/* Function:     Implements the standby mode.                                   */
/* Arguments:    None.                                                          */
/* ---------------------------------------------------------------------------- */
void MODEstandby()
{
  /* display the mode message */
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("MIDIsonar");
  lcd.setCursor(0,1); lcd.print("Standby");
  delay(1000);

  /* check if MODE SELECT button is pressed */
  do 
  {
    if(digitalRead(buttonPin1) == HIGH) 
      {
        /* wait until MODE SELECT button is released */
        do
          delay(20);
        while(digitalRead(buttonPin1) == HIGH);
        return;
      }
    delay(20);
  } while (true);

} /* End of MODEstandby */


/* ---------------------------------------------------------------------------- */
/* MODEsetup():                                                                 */
/* Function:     Implements the setup mode.                                     */
/* Arguments:    None.                                                          */
/* ---------------------------------------------------------------------------- */
void MODEsetup()
{
  /* display the mode message */
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("MIDIsonar");
  lcd.setCursor(0,1); lcd.print("Setup");
  delay(1000);
  lcd.clear();

  /* initialize variables */
  /*int menu_id = 0;        /* menu id of the selected menu item */
  /*int menu_id_old = 3;    /* menu id of the previous menu item */
  int screen_id;          /* screen id of the selected menu item */
  int screen_id_old;      /* screen id of the previous menu item */

  int controller_id;      /* controller id of the selected menu item */
  int array_pos;          /* position of the selected menu item in the value array */

  int init_potval;        /* intial value read from the potentiometer at the selection of a new menu item [0-1023] */
  int read_potval;        /* current value read from the potentiometer [0-1023] */
  int prev_potval;        /* previous value read from the potentiometer [0-1023] */
  int prev_val;           /* previous value of the selected menu item as stored in the value array [type specific] */
  int temp_val;           /* temporary value for the selected menu item [type specific] */
  int treshold;           /* indicates whether value knob is turned past treshold (0=FALSE, 1=TRUE) */
  bool buttonhold;        /* indicates whether value button is hold down */

  char* valstr = "   ";   /* temporary string used to construct formatted string values */

  do 
  {
    /* determine screen id's of the selected menu item and the previous menu item */
    screen_id = menu_id/3;
    screen_id_old = menu_id_old/3;

    /* determine controller id and array position of the selected menu item */
    controller_id = menu_id/9;
    array_pos = menu_id%9;

    /* array positions of menu-items [6-8] depend on selected TYPE [if TYPE=1 increase three positions] */
    if(array_pos>5) array_pos=array_pos+3*(value[controller_id][1]-1);

    /* redraw the complete screen if the selected menu item is on a new setup screen */
    /*if(screen_id != screen_id_old)*/
    if(0==0)
       {
         lcd.noCursor();
         lcd.clear();
         lcd.setCursor(0,0);
         if(controller_id==0) lcd.print("A:"); else lcd.print("B:");

         for(int counter=0; counter<3; counter++)
         {
            lcd.setCursor(2+counter*5,0); lcd.print(menu[(array_pos/3)*3+counter]);
            value2string(value[controller_id][(array_pos/3)*3+counter], valstr, type[(array_pos/3)*3+counter]);
            lcd.setCursor(3+counter*5,1); lcd.print(valstr);
         }
       }

    /* draw the cursor at the place of the selected menu item */
    lcd.noCursor();
    lcd.setCursor(5+(menu_id%3)*5,1); lcd.cursor();

    /* read the initial value for the selected menu item */
    init_potval = analogRead(potPin1);
    prev_potval = init_potval;
    
    /* read the stored value for the selected menu item from the value array */
    prev_val = value[controller_id][array_pos];
    
    /* reset the treshold */
    treshold = 0;

    /* check for setup actions */
    do
    {
      /* check if MODE SELECT button is pressed */
      if(digitalRead(buttonPin1) == HIGH)
      {
        /* wait until MODE SELECT button is released */
        do
            delay(20);
        while(digitalRead(buttonPin1) == HIGH);
        lcd.noCursor();
        return;
      }

      /* check if MENU PREV button is pressed */
      if(digitalRead(buttonPin2) == HIGH) 
      {
        /* decrease the menu_id */
        menu_id_old=menu_id; 
        menu_id--;
        if(menu_id==-1) menu_id=17;
        
        /* wait until MENU PREV button is released */
        do
          delay(20);
        while(digitalRead(buttonPin2) == HIGH);
         
        break;
      }

      /* check if MENU NEXT button is pressed */
      if(digitalRead(buttonPin3) == HIGH) 
      {
        /* increase the menu_id */
        menu_id_old=menu_id; 
        menu_id++;
        if(menu_id==18) menu_id=0;

        /* wait until MENU NEXT button is released */
        do
          delay(20);
        while(digitalRead(buttonPin3) == HIGH);

        break;
      }

      /* check if VALUE INC button is pressed */
      if(digitalRead(buttonPin4) == HIGH)
      {
        /* reset the button hold indicator */
        buttonhold=false;

        /* wait a little to prevent jitter */
        delay(20);

        /* check if VALUE INC button is still pressed */
        while(digitalRead(buttonPin4) == HIGH)
        {
            /* get value from the value array */
            temp_val = value[controller_id][array_pos]; 

            /* increase the value value */
            switch(array_pos)
            {
              /* ACTV */
              case  0: temp_val = temp_val + 1;
                      if(temp_val == 2) temp_val = 0; /* wrap around */
                      break;
              /* TYPE */
              case  1: temp_val = temp_val + 1;
                       if(temp_val == 5) temp_val = 1; /* wrap around */
                       break;
              /* CHNL */         
              case  2: temp_val = temp_val + 1;
                       if(temp_val == 17) temp_val = 1; /* wrap around */
                       break;
              /* LDST */
              case  3: temp_val = temp_val + 5;
                       if(temp_val==value[controller_id][array_pos+1]) temp_val=value[controller_id][array_pos+1]-5; /* value of LDST must be lower then value of HDST */
                       break;
              /* HDST */
              case  4: temp_val = temp_val + 5;
                       if(temp_val == 85) temp_val = 80; /* no wrap around */
                       break;
              /* POLR */
              case  5: temp_val = temp_val + 1;
                       if(temp_val == 2) temp_val = 0; /* wrap around */
                       break;
              /* CTRL NUMBER */
              case  6: temp_val = temp_val + 1;
                       if(temp_val == 128) temp_val = 0; /* wrap around */
                       break;
              /* CTRL LVAL */
              case  7: temp_val = temp_val + 1;
                       if(temp_val==value[controller_id][array_pos+1]) temp_val=value[controller_id][array_pos+1]-1; /* value of LVAL must be lower then value of HVAL */
                       break;
              /* CTRL HVAL */
              case  8: temp_val = temp_val + 1; 
                       if(temp_val == 128) temp_val = 127; /* no wrap around */
                       break;
              /* NOTE SCALE */
              case  9: temp_val = temp_val + 1;
                       if(temp_val == 13) temp_val = 1; /* wrap around */
                       break;
              /* NOTE ROOT */
              case 10: temp_val = temp_val + 1;
                       if(temp_val == 120) temp_val = 119; /* no wrap around */
                       break;
              /* NOTE RANG */
              case 11: temp_val = temp_val + 1;
                       if(temp_val == 25) temp_val = 24; /* no wrap around */
                       break;
			        /* CHORD PROGRESSION */
              case 12: temp_val = temp_val + 1;
                       if(temp_val == 10) temp_val = 1; /* wrap around */
                       break;
              /* CHORD ROOT */
              case 13: temp_val = temp_val + 1;
                       if(temp_val == 120) temp_val = 119; /* no wrap around */
                       break;
              /* NOTE RANG */
              case 14: temp_val = temp_val + 1;
                       if(temp_val == 15) temp_val = 14; /* no wrap around */
                       break;
              /* PBND TYPE */
              case 15: temp_val = temp_val + 1;
                       if(temp_val == 4) temp_val = 1; /* wrap around */
                       break;
              /* PBND NTRL */
              case 16: temp_val = temp_val + 1;
                       if(temp_val == 4) temp_val = 1; /* wrap around */
                       break;
              /* PBND PBFX */
              case 17: temp_val = temp_val + 1;
                       if(temp_val == 3) temp_val = 1; /* wrap around */
                       break;
            }

            /* store the changed value in the value array */
            value[controller_id][array_pos] = temp_val; 

            /* display the changed value and redraw cursor */
            value2string(value[controller_id][array_pos], valstr, type[array_pos]);
            lcd.setCursor(3+(menu_id%3)*5,1); lcd.print(valstr);
            lcd.setCursor(5+(menu_id%3)*5,1); lcd.cursor();

            /* check buttonhold and wait appropriate time for next button check */
            if(buttonhold == false)
            {
              delay(300);
              buttonhold=true;
            }
            else
            {
              delay(70);
            }
        }
      }   

      /* check if VALUE DEC button is pressed */
      if(digitalRead(buttonPin5) == HIGH)
      {
        /* reset the button hold indicator */
        buttonhold=false;

        /* wait a little to prevent jitter */
        delay(20);

        /* check if VALUE DEC button is still pressed */
        while(digitalRead(buttonPin5) == HIGH)
        {
            /* get value from the value array */
            temp_val = value[controller_id][array_pos]; 

            /* increase the value value */
            switch(array_pos)
            {
              /* ACTV */
              case  0: temp_val = temp_val - 1;
                      if(temp_val == -1) temp_val = 1; /* wrap around */
                      break;
              /* TYPE */
              case  1: temp_val = temp_val - 1;
                       if(temp_val == 0) temp_val = 4; /* wrap around */
                       break;
              /* CHNL */         
              case  2: temp_val = temp_val - 1;
                       if(temp_val == 0) temp_val = 16; /* wrap around */
                       break;
              /* LDST */
              case  3: temp_val = temp_val - 5;
                       if(temp_val == 0) temp_val = 5; /* no wrap around */
                       break;
              /* HDST */
              case  4: temp_val = temp_val - 5;
                       if(temp_val==value[controller_id][array_pos-1]) temp_val=value[controller_id][array_pos-1]+5; /* value of HDST must be higher then value of LDST */
                       break;
              /* POLR */
              case  5: temp_val = temp_val - 1;
                       if(temp_val == -1) temp_val = 1; /* wrap around */
                       break;
              /* CTRL NUMBER */
              case  6: temp_val = temp_val - 1;
                       if(temp_val == -1) temp_val = 127; /* wrap around */
                       break;
              /* CTRL LVAL */
              case  7: temp_val = temp_val - 1;
                       if(temp_val == -1) temp_val = 0; /* no wrap around */
                       break;
              /* CTRL HVAL */	
              case  8: temp_val = temp_val - 1; 
                       if(temp_val==value[controller_id][array_pos-1]) temp_val=value[controller_id][array_pos-1]+1; /* value of HVAL must be higher then value of LVAL */
                       break;
              /* NOTE SCALE */
              case  9: temp_val = temp_val - 1;
                       if(temp_val == 0) temp_val = 12; /* wrap around */
                       break;
              /* NOTE ROOT */
              case 10: temp_val = temp_val - 1;
                       if(temp_val == 11) temp_val = 12; /* no wrap around */
                       break;
              /* NOTE RANG */
              case 11: temp_val = temp_val - 1;
                       if(temp_val == 0) temp_val = 1; /* no wrap around */
                       break;
			        /* CHORD PROGRESSION */
              case 12: temp_val = temp_val - 1;
                       if(temp_val == 0) temp_val = 9; /* wrap around */
                       break;
              /* CHORD ROOT */
              case 13: temp_val = temp_val - 1;
                       if(temp_val == 11) temp_val = 12; /* no wrap around */
                       break;
              /* CHORD RANG */
              case 14: temp_val = temp_val - 1;
                       if(temp_val == 0) temp_val = 1; /* no wrap around */
                       break;
              /* PBND TYPE */
              case 15: temp_val = temp_val - 1;
                       if(temp_val == 0) temp_val = 3; /* wrap around */
                       break;
              /* PBND NTRL */
              case 16: temp_val = temp_val - 1;
                       if(temp_val == 0) temp_val = 3; /* wrap around */
                       break;
              /* PBND PBFX */
              case 17: temp_val = temp_val - 1;
                       if(temp_val == 0) temp_val = 2; /* wrap around */
                       break;
            }

            /* store the changed value in the value array */
            value[controller_id][array_pos] = temp_val; 

            /* display the changed value and redraw cursor */
            value2string(value[controller_id][array_pos], valstr, type[array_pos]);
            lcd.setCursor(3+(menu_id%3)*5,1); lcd.print(valstr);
            lcd.setCursor(5+(menu_id%3)*5,1); lcd.cursor();     

            /* check buttonhold and wait appropriate time for next button check */
            if(buttonhold == false)
            {
              delay(300);
              buttonhold=true;
            }
            else
            {
              delay(70);
            }
        }
      }   

    } while (true);
          
  } while (true);

} /* End of MODEsetup */


/* ---------------------------------------------------------------------------- */
/* MODEplay():                                                                  */
/* Function:     Implements the play mode.                                      */
/* Arguments:    None.                                                          */
/* ---------------------------------------------------------------------------- */
void MODEplay()
{
  /* display the mode message */
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("MIDIsonar");
  lcd.setCursor(0,1); lcd.print("Play");
  delay(1000);
  lcd.clear();

  /* initialize variables */
  int pingTimeOldA = 0;   /* previous ping time for controller A */
  int lowRangeA;          /* low range ping controller A in microseconds */
  int highRangeA;         /* high range ping controller A in microseconds */
  int rangePolA;          /* polarity controller A */
  int lowValA;            /* low value for controller A*/
  int highValA;           /* high value for controller A*/
  int newValA = 0;        /* current value measured for controller A */
  int oldValA = 999;      /* previous value measured for controller A */
  int noteStateA = 0;     /* state of the notes when type=NOTE: 0=off, 1=on */
  int newNoteA = 0;       /* current note for controller A */
  int oldNoteA = 0;       /* previous note for controller A */
  int newChordA = 0;	    /* current chord for controller A */
  int oldChordA = 0;	    /* previous chord for controller A */
  int pbValA = 0;         /* calculated pitchbend value for controller A */
  int pbRangeA = 0;       /* calculated pitchbend range for controller A */
  int pbDeltaA = 0;       /* calculated pitchbend delta for controller A */
  int pbDirA = 0;         /* calculated pitchbend direction for controller A */
  int pbStateA = 0;       /* state of the pitchbend when type=PBND: 0=neutral, 1=bend */

  int pingTimeOldB = 0;   /* previous ping time for controller B */
  int lowRangeB;          /* low range ping controller B in microseconds */
  int highRangeB;         /* high range ping controller B in microseconds */
  int rangePolB;          /* polarity controller B */
  int lowValB;            /* low value for controller B*/
  int highValB;           /* high value for controller B*/
  int newValB = 0;        /* current value measured for controller B */
  int oldValB = 999;      /* previous value measured for controller B */
  int noteStateB = 0;     /* State of the notes when type=NOTE: 0=off, 1=on */
  int newNoteB = 0;       /* current note for controller B */
  int oldNoteB = 0;       /* previous note for controller B */
  int newChordB = 0;	    /* current chord for controller B */
  int oldChordB = 0;	    /* previous chord for controller B */
  int pbValB = 0;         /* calculated pitchbend value for controller B */
  int pbRangeB = 0;       /* calculated pitchbend range for controller B */
  int pbDeltaB = 0;       /* calculated pitchbend delta for controller B */
  int pbDirB = 0;         /* calculated pitchbend direction for controller B */
  int pbStateB = 0;       /* state of the pitchbend when type=PBND: 0=neutral, 1=bend */

  int pingTime;           /* measured ping time between trigger and echo */
  int displayTime = 0;    /* display update timer to avoid value flickering */

  char* valstr = "   ";   /* temporary string used to construct formatted string values */
  char* pbstr = "   ";    /* temporary string used to construct formatted string values */

  /* display the static part of the mode play screen and switch on LED's */
  for(int counter=0; counter<2; counter++)
  {
    /* display the label for the controller */
    if(counter==0) {lcd.setCursor(0,counter); lcd.print("A:");} else {lcd.setCursor(0,counter); lcd.print("B:");}

    /* determine whether the controller is active */
    if(value[counter][0]==0)
    {
      /* controller is not active */
      /* display OFF */
      lcd.setCursor(2,counter); lcd.print("OFF");
    }
    else
    {
      /* controller is active */
      /* switch on the controller activity LED */
      if(counter==0) digitalWrite(ledPinA, HIGH); else digitalWrite(ledPinB, HIGH);

      /* display the controller TYPE */
      value2string(value[counter][1], valstr, type[1]);
      lcd.setCursor(2,counter); lcd.print(valstr);

      /* display the controller CTRL NUMBER, NOTE SCALE OR CHORD PROGRESSION */
      if(value[counter][1]==1) value2string(value[counter][6], valstr, type[6]);
      if(value[counter][1]==2) value2string(value[counter][9], valstr, type[9]);
      if(value[counter][1]==3) value2string(value[counter][12], valstr, type[12]);
      if(value[counter][1]==4) value2string(value[counter][15], valstr, type[15]);
      lcd.setCursor(6,counter); lcd.print(valstr);

      /* display the controller CHNL */
      value2string(value[counter][2], valstr, type[2]);
      lcd.setCursor(9,counter); lcd.print(valstr);

      /* display three dashes as placeholder for the controller value */
      lcd.setCursor(13, counter); lcd.print("---");
    }
  }

  /* determine lowrange and highrange as pingtimes in microseconds */
  lowRangeA = value[0][3]*58; highRangeA = value[0][4]*58;
  lowRangeB = value[1][3]*58; highRangeB = value[1][4]*58;
  
  /* determine the polarity */
  rangePolA = value[0][5]; 
  rangePolB = value[1][5];

  /* determine lowval and highval depending on TYPE */
  if(value[0][1]==1) {lowValA=value[0][7]; highValA=value[0][8];}                       /* controller A - CTRL        */
  if(value[0][1]==2) {lowValA=0; highValA=value[0][11];}                                /* controller A - NOTE        */
  if(value[0][1]==3) {lowValA=0; highValA=value[0][14];}                                /* controller A - CHRD        */
  if(value[0][1]==4 && value[0][15]==1) {lowValA=0; highValA=10000+value[0][16]*819;}   /* controller A - PBND UP     */
  if(value[0][1]==4 && value[0][15]==2) {lowValA=0; highValA=10000+value[0][16]*819;}   /* controller A - PBND DWN    */
  if(value[0][1]==4 && value[0][15]==3) {lowValA=0; highValA=20000+value[0][16]*819;}   /* controller A - PBND UP&DWN */
  if(value[1][1]==1) {lowValB=value[1][7]; highValB=value[1][8];}                       /* controller B - CTRL        */
  if(value[1][1]==2) {lowValB=0; highValB=value[1][11];}                                /* controller B - NOTE        */
  if(value[1][1]==3) {lowValB=0; highValB=value[1][14];}                                /* controller B - CTRL        */
  if(value[1][1]==4 && value[1][15]==1) {lowValB=0; highValB=10000+value[0][16]*819;}   /* controller B - PBND UP     */
  if(value[1][1]==4 && value[1][15]==2) {lowValB=0; highValB=10000+value[0][16]*819;}   /* controller B - PBND DWN    */
  if(value[1][1]==4 && value[1][15]==3) {lowValB=0; highValB=20000+value[0][16]*819;}   /* controller B - PBND UP&DWN */

  do 
  {
      /* if active, process controller A */
      if(value[0][0]==1)
      {
        /* send ping and get ping time in microseconds based on configured average depending on the TYPE */
        pingTime = sonarA.ping_median(pingmedian[value[0][1]-1]);

        /* process the results when pingTime is within range */
        if (pingTime>lowRangeA && pingTime<highRangeA)
        {
          /* determine new value */
          if(value[0][1]==4) /* TYPE = PBND */
          {
            newValA = int ( (pingTime-lowRangeA) * ( (highValA-lowValA+1) / (highRangeA-lowRangeA) ) );
          }
          else /* TYPE =  CNTRL or NOTE or CHORD */
          {
            newValA = int ( (pingTime-lowRangeA) / ( (highRangeA-lowRangeA) / (highValA-lowValA+1) ) );
          }

          /* adjust value for polarity */
          if (rangePolA == 1) newValA = lowValA + newValA; else newValA = highValA - newValA;

          /* keep value within range in case of rounding errors */
          if (newValA<lowValA) newValA=lowValA; if (newValA>highValA) newValA=highValA;

          /* adjust value to array index */
          if((value[0][1]==2) || (value[0][1]==3)) if(newValA==highValA) newValA--;
          
          /* send MIDI controller message if value has changed */
          if ( (newValA!=oldValA) && (abs(pingTime-pingTimeOldA) > wiggleroom[value[0][1]-1] ) )
          {
            /* set previous value of the pingTime */ 
            pingTimeOldA = pingTime;
            
            /* send CC message if TYPE is CTRL */
            if(value[0][1]==1)
            {
              /* CC: statusbyte=144+channel, databyte1=controller, databyte2=value */
              MIDImessage(176+value[0][2]-1, value[0][6], newValA);

              /* display value on the LCD screen */
              if(displayTime==0)
              {
                 value2string(newValA, valstr, 1);
                 lcd.setCursor(13,0); lcd.print(valstr);
              }
            }

            /* send Note messages if TYPE is NOTE */
            if(value[0][1]==2)
            {
              /* Send noteOff message for the previous note */
              if(oldValA != 999)
              {
                oldNoteA=value[0][10]+scales[value[0][9]-1][oldValA];
			          MIDIchord(value[0][2], 0, oldNoteA, 1);
              }
 
              /* Send noteOn messages for the new chord */
              newNoteA=value[0][10]+scales[value[0][9]-1][newValA];
			        MIDIchord(value[0][2], 1, newNoteA, 1);

              /* Switch noteState to On: notes are sounding */
              noteStateA = 1;

              /* display value on the LCD screen */
              value2string(newNoteA, valstr, 6);
              lcd.setCursor(13,0); lcd.print(valstr);
            }
             
			      /* send Note messages if TYPE is CHORD */
            if(value[0][1]==3)
            {
              if(oldValA != 999)
              {
                /* Send noteOff message for the previous chord */
                oldNoteA=value[0][13]+progs[value[0][12]-1][oldValA][0];
			          oldChordA=progs[value[0][12]-1][oldValA][1];
			          MIDIchord(value[0][2], 0, oldNoteA, oldChordA);
              }
              
              /* Send noteOn messages for the new chord */
              newNoteA=value[0][13]+progs[value[0][12]-1][newValA][0];
			        newChordA=progs[value[0][12]-1][newValA][1];
			        MIDIchord(value[0][2], 1, newNoteA, newChordA);

              /* Switch noteState to On: notes are sounding */
              noteStateA = 1;

              /* display value on the LCD screen */
              value2string(newNoteA, valstr, 6);
              lcd.setCursor(13,0); lcd.print(valstr);
            }

            /* send Pitch Bend messages if TYPE is PBND */
            if(value[0][1]==4)
            {
              /* adjust for neutral zones */
              if(value[0][15]==1) /* pitchbend UP */
              {
                pbRangeA = 819 * value[0][16]; pbDirA = 1;
                if(newValA <= pbRangeA) pbDeltaA = 0; else pbDeltaA = newValA - pbRangeA;
              }
              if(value[0][15]==2) /* pitchbend DOWN */
              {
                pbRangeA = 819 * value[0][16]; pbDirA = -1;
                if(newValA <= pbRangeA) pbDeltaA = 0; else pbDeltaA = newValA - pbRangeA;
              }
              if(value[0][15]==3) /* pitchbend UP&DOWN */
              {
                pbRangeA = 819 * value[0][16];
                if((newValA >= 10900-pbRangeA) && (newValA <= 10900+pbRangeA)) {pbDeltaA = 0; pbDirA=1;} 
                if(newValA < 10900-pbRangeA) {pbDeltaA = 10900-pbRangeA-newValA; pbDirA=-1;}
                if(newValA > 10900+pbRangeA) {pbDeltaA = newValA-pbRangeA-10900; pbDirA=1;}
              }

              /* process pitchbend effects */
              if(value[0][17] == 2) /* pitchbend in discrete steps */
              {
                pbDeltaA = (pbDeltaA / 256) * 256;
              }
              
              /* finalize pitchbend value */
              pbValA = 8192 + pbDirA * pbDeltaA;
              if(pbValA < 0) pbValA = 0;
              if(pbValA > 16383) pbValA = 16383;

              /* Send PB message */
			        MIDIpitchbend(value[0][2], pbValA);

              /* Set pbState to 0 if neutral and 1 if bend */
              if(pbValA == 8192) pbStateA=0; else pbStateA=1;

              /* display value on the LCD screen */
              if(pbValA<2730) pbstr = "---";
              else if(pbValA<5460) pbstr = " --";
              else if(pbValA<8192) pbstr = "  -";
              else if(pbValA>13650) pbstr = "+++";
              else if(pbValA>10920) pbstr = "++ ";
              else if(pbValA>8192) pbstr = "+  ";
              else pbstr = "000";
              lcd.setCursor(13,0); lcd.print(pbstr);
            }  
          }

          /* remember the current value for next cycle */
          oldValA = newValA;
        } 
        
        /* process the results when pingTime is out of range */
        else
        {
          /* send Note Off message if TYPE is NOTE and notes are sounding */
          if( (value[0][1]==2) && (noteStateA==1))
            {
              /* Send noteOff messages for the previous note */
              oldNoteA=value[0][10]+scales[value[0][9]-1][oldValA];
			        MIDIchord(value[0][2], 0, oldNoteA, 1);

              /* Switch noteState to Off and reset old values: notes are no longer sounding */
              noteStateA = 0; oldValA = 999; pingTimeOldA = pingTime;

              /* display value on the LCD screen */
              lcd.setCursor(13, 0);
              lcd.print("---");
            }
			
		      /* send Note Off message if TYPE is CHORD and notes are sounding */
          if((value[0][1]==3) && (noteStateA==1))
            {
              /* Send noteOff messages for the previous chord */
              oldNoteA=value[0][13]+progs[value[0][12]-1][oldValA][0];
			        oldChordA=progs[value[0][12]-1][oldValA][1];
			        MIDIchord(value[0][2], 0, oldNoteA, oldChordA);

              /* Switch noteState to Off and reset old values: notes are no longer sounding */
              noteStateA = 0; oldValA = 999; pingTimeOldA = pingTime;

              /* display value on the LCD screen */
              lcd.setCursor(13, 0);
              lcd.print("---");
            }
        } 
      }

      /* if active, process controller B */
      if(value[1][0]==1)
      {
        /* send ping and get ping time in microseconds based on configured average depending on the TYPE */
        pingTime = sonarB.ping_median(pingmedian[value[1][1]-1]);   

        /* process the results when pingTime is within range */
        if (pingTime>lowRangeB && pingTime<highRangeB)
        {
          /* determine new value */
          if(value[1][1]==4) /* TYPE = PBND */
          {
            newValB = int ( (pingTime-lowRangeB) * ( (highValB-lowValB+1) / (highRangeB-lowRangeB) ) );
          }
          else /* TYPE =  CNTRL or NOTE or CHORD */
          {
            newValB = int ( (pingTime-lowRangeB) / ( (highRangeB-lowRangeB) / (highValB-lowValB+1) ) );
          }

          /* adjust value for polarity */
          if (rangePolB == 1) newValB = lowValB + newValB; else newValB = highValB - newValB;

          /* keep value within range in case of rounding errors */
          if (newValB<lowValB) newValB=lowValB; if (newValB>highValB) newValB=highValB;

          /* adjust value to array index */
          if((value[1][1]==2) || (value[1][1]==3)) if(newValB==highValB) newValB--;

          /* send MIDI controller message if value has changed */
          if ( (newValB!=oldValB) && (abs(pingTime-pingTimeOldB) > wiggleroom[value[1][1]-1] ) )
          {
            /* set previous value of the pingTime */ 
            pingTimeOldB = pingTime;
            
            /* send CC message if TYPE is CTRL */
            if(value[1][1]==1)
            {
              /* CC: statusbyte=144+channel, databyte1=controller, databyte2=value */
              MIDImessage(176+value[1][2]-1, value[1][6], newValB);

              /* display value on the LCD screen */
              if(displayTime==0)
              {
                value2string(newValB, valstr, 1);
                lcd.setCursor(13,1); lcd.print(valstr);
              }
            }

            /* send Note messages if TYPE is NOTE */
            if(value[1][1]==2)
            {
              if(oldValB != 999)
              {
                /* Send noteOff messages for the previous note */
                oldNoteB=value[1][10]+scales[value[1][9]-1][oldValB];
                MIDIchord(value[1][2], 0, oldNoteB, 1);
              }

              /* Send noteOn messages for the new note */
              newNoteB=value[1][10]+scales[value[1][9]-1][newValB];
              MIDIchord(value[1][2], 1, newNoteB, 1);

              /* Switch noteState to On: notes are sounding */
              noteStateB = 1;

              /* display value on the LCD screen */
              value2string(newNoteB, valstr, 6);
              lcd.setCursor(13,1); lcd.print(valstr);
            }
			
			      /* send Note messages if TYPE is CHORD */
            if(value[1][1]==3)
            {
              /* Send noteOff message for the previous chord */
              if(oldValB != 999)
              {
                oldNoteB=value[1][13]+progs[value[1][12]-1][oldValB][0];
			          oldChordB=progs[value[1][12]-1][oldValB][1];
			          MIDIchord(value[1][2], 0, oldNoteB, oldChordB);
              }

              /* Send noteOn messages for the new chord */
              newNoteB=value[1][13]+progs[value[1][12]-1][newValB][0];
			        newChordB=progs[value[1][12]-1][newValB][1];
			        MIDIchord(value[1][2], 1, newNoteB, newChordB);

              /* Switch noteState to On: notes are sounding */
              noteStateB = 1;

              /* display value on the LCD screen */
              value2string(newNoteB, valstr, 6);
              lcd.setCursor(13,1); lcd.print(valstr);
			      }

            /* send Pitch Bend messages if TYPE is PBND */
            if(value[1][1]==4)
            {
              /* adjust for neutral zones */
              if(value[1][15]==1) /* pitchbend UP */
              {
                pbRangeB = 819 * value[1][16]; pbDirB = 1;
                if(newValB <= pbRangeB) pbDeltaB = 0; else pbDeltaB = newValB - pbRangeB;
              }
              if(value[1][15]==2) /* pitchbend DOWN */
              {
                pbRangeB = 819 * value[1][16]; pbDirB = -1;
                if(newValB <= pbRangeB) pbDeltaB = 0; else pbDeltaB = newValB - pbRangeB;
              }
              if(value[1][15]==3) /* pitchbend UP&DOWN */
              {
                pbRangeB = 819 * value[1][16];
                if((newValB >= 10900-pbRangeB) && (newValB <= 10900+pbRangeB)) {pbDeltaB = 0; pbDirB=1;} 
                if(newValB < 10900-pbRangeB) {pbDeltaB = 10900-pbRangeB-newValB; pbDirB=-1;}
                if(newValB > 10900+pbRangeB) {pbDeltaB = newValB-pbRangeB-10900; pbDirB=1;}
              }

              /* process pitchbend effects */
              if(value[1][17] == 2) /* pitchbend in discrete steps */
              {
                pbDeltaB = (pbDeltaB / 1024) * 1024;
              }
              
              /* finalize pitchbend value */
              pbValB = 8192 + pbDirB * pbDeltaB;
              if(pbValB < 0) pbValB = 0;
              if(pbValB > 16383) pbValB = 16383;

              /* Send PB message */
			        MIDIpitchbend(value[1][2], pbValB);

              /* Set pbState to 0 if neutral and 1 if bend */
              if(pbValB == 8192) pbStateB=0; else pbStateB=1;

              /* display value on the LCD screen */
              if(pbValB<2730) pbstr = "---";
              else if(pbValB<5460) pbstr = " --";
              else if(pbValB<8192) pbstr = "  -";
              else if(pbValB>13650) pbstr = "+++";
              else if(pbValB>10920) pbstr = "++ ";
              else if(pbValB>8192) pbstr = "+  ";
              else pbstr = "000";
              lcd.setCursor(13,1); lcd.print(pbstr);
            } 
          }

          /* remember the current value for next cycle */
          oldValB = newValB;
        } 
        
        /* process the results when pingTime is out of range */
        else
        {
          /* send Note Off message if TYPE is NOT and notes are sounding */
          if( (value[1][1]==2) && (noteStateB==1))
            {
              /* Send noteOff messages for the previous chord */
              oldNoteB=value[1][10]+scales[value[1][9]-1][oldValB];
              MIDIchord(value[1][2], 0, oldNoteB, 1);

              /* Switch noteState to Off and reset old values: notes are no longer sounding */
              noteStateB = 0; oldValB = 999; pingTimeOldB = pingTime;

              /* display value on the LCD screen */
              lcd.setCursor(13, 1);
              lcd.print("---");
            }
			
		      /* send Note Off message if TYPE is CHORD and notes are sounding */
          if( (value[1][1]==3) && (noteStateB==1))
            {
              /* Send noteOff messages for the previous chord */
              oldNoteB=value[1][13]+progs[value[1][12]-1][oldValB][0];
			        oldChordB=progs[value[1][12]-1][oldValB][1];
			        MIDIchord(value[1][2], 0, oldNoteB, oldChordB);

              /* Switch noteState to Off and reset old values: notes are no longer sounding */
              noteStateB = 0; oldValB = 999; pingTimeOldB = pingTime;

              /* display value on the LCD screen */
              lcd.setCursor(13, 1);
              lcd.print("---");
            }
        }
      }

      /* check if MODE SELECT button is pressed */
      if(digitalRead(buttonPin1) == HIGH)
      {
        /* wait until MODE SELECT button is released */
        do
            delay(20);
        while(digitalRead(buttonPin1) == HIGH);
        
        /* switch off the controller activity LEDs */
        digitalWrite(ledPinA, LOW); digitalWrite(ledPinB, LOW);

        /* send noteOff messages for the previous note if type = NOTE */
        if(value[0][1]==2) {oldNoteA=value[0][10]+scales[value[0][9]-1][oldValA]; MIDIchord(value[0][2], 0, oldNoteA, 1);}
        if(value[1][1]==2) {oldNoteB=value[1][10]+scales[value[1][9]-1][oldValB]; MIDIchord(value[1][2], 0, oldNoteB, 1);}   
			        
        /* send noteOff messages for the previous chord if type = CHORD */
        if(value[0][1]==3) 
        { oldNoteA=value[0][13]+progs[value[0][12]-1][oldValA][0];
          oldChordA=progs[value[0][12]-1][oldValA][1];
			    MIDIchord(value[0][2], 0, oldNoteA, oldChordA);
        }
        if(value[1][1]==3)
        {
          oldNoteB=value[1][13]+progs[value[1][12]-1][oldValB][0];
			    oldChordB=progs[value[1][12]-1][oldValB][1];
			    MIDIchord(value[1][2], 0, oldNoteB, oldChordB);
        }

			  /* reset pitchbend to neutral */
        if(value[0][1]==4) {MIDIpitchbend(value[0][2], 8192);}
        if(value[1][1]==4) {MIDIpitchbend(value[1][2], 8192);}

        return;
      }

      /* increase displayTime */
      displayTime++;
      if(displayTime==2)
      {
        displayTime=0;
      }

  } while (true);

} /* End of MODEplay */


/* ---------------------------------------------------------------------------- */
/* value2string():                                                              */
/* Function:     Converts a value to a formatted string depending on the type.  */
/* Arguments:    value      : integer value to be converted                     */
/*               valstr     : pointer to string to store the converted value    */
/*               convtype   : conversion type                                   */
/* ---------------------------------------------------------------------------- */
void value2string(int value, char *valstr, int convtype)
{
  if(convtype == 1)
  {
    sprintf(valstr, "%3d", value);
  }
  
  if(convtype == 2)
  {
    if(value==0) sprintf(valstr, "OFF");
    if(value==1) sprintf(valstr, " ON");
  }
  
  if(convtype == 3)
  {
    if(value==1) sprintf(valstr, " CC");
    if(value==2) sprintf(valstr, "NOT");
    if(value==3) sprintf(valstr, "CHD");
    if(value==4) sprintf(valstr, " PB");
  }
  
  if(convtype == 4)
  {
    if(value==0) sprintf(valstr, "NEG");
    if(value==1) sprintf(valstr, "POS");
  }
  
  if(convtype == 5)
  {
    if(value==1) sprintf(valstr, "CHR");
    if(value==2) sprintf(valstr, "MAJ");
    if(value==3) sprintf(valstr, "MIN");
    if(value==4) sprintf(valstr, "MMI");
    if(value==5) sprintf(valstr, "HMI");
    if(value==6) sprintf(valstr, "WHL");
    if(value==7) sprintf(valstr, "MAB");
    if(value==8) sprintf(valstr, "MIB");
		if(value==9) sprintf(valstr, "MA5");
		if(value==10) sprintf(valstr, "MI5");
		if(value==11) sprintf(valstr, "8WH");
		if(value==12) sprintf(valstr, "8HW");
  }
  
  if(convtype == 6)
  {
    int midiNote = value % 12;
    int midiOct = (value / 12) - 1;
    if(midiNote==0) sprintf(valstr, " C%1d", midiOct);
    if(midiNote==1) sprintf(valstr, "C#%1d", midiOct);
    if(midiNote==2) sprintf(valstr, " D%1d", midiOct);
    if(midiNote==3) sprintf(valstr, "D#%1d", midiOct);
    if(midiNote==4) sprintf(valstr, " E%1d", midiOct);
    if(midiNote==5) sprintf(valstr, " F%1d", midiOct);
    if(midiNote==6) sprintf(valstr, "F#%1d", midiOct);
    if(midiNote==7) sprintf(valstr, " G%1d", midiOct);
    if(midiNote==8) sprintf(valstr, "G#%1d", midiOct);
    if(midiNote==9) sprintf(valstr, " A%1d", midiOct);
    if(midiNote==10) sprintf(valstr, "A#%1d", midiOct);
    if(midiNote==11) sprintf(valstr, " B%1d", midiOct);
  }
  
  if(convtype == 7)
  {
    if(value==1) sprintf(valstr, "MAJ");
    if(value==2) sprintf(valstr, "MIN");
    if(value==3) sprintf(valstr, "MAB");
    if(value==4) sprintf(valstr, "MIB");
    if(value==5) sprintf(valstr, "POP");
    if(value==6) sprintf(valstr, "JAZ");
    if(value==7) sprintf(valstr, "MOD");
    if(value==8) sprintf(valstr, "GSP");
    if(value==9) sprintf(valstr, "PBL");
  }

  if(convtype == 8)
  {
    if(value==1) sprintf(valstr, " UP");
    if(value==2) sprintf(valstr, "DWN");
    if(value==3) sprintf(valstr, "U&D");
  }

  if(convtype == 9)
  {
    if(value==1) sprintf(valstr, "SML");
    if(value==2) sprintf(valstr, "MED");
    if(value==3) sprintf(valstr, "LRG");
  }

  if(convtype == 10)
  {
    if(value==1) sprintf(valstr, "FLW");
    if(value==2) sprintf(valstr, "STP");
  }
  
} /* End of value2string */


/* ---------------------------------------------------------------------------- */
/* MIDIchord():                                                                 */
/* Function:     Creates a MIDI chord                                           */
/* Arguments:    channel       : MIDI channel                                   */
/*               notestate     : 0 = send noteOff, 1 = send noteOn              */
/*               basenote      : basenote for the chord                         */
/*               chordtype     : chordtype as defined in NOTE                   */
/* ---------------------------------------------------------------------------- */
void MIDIchord(int channel, int notestate, int basenote, int chordtype)
{
  /* initalize variables */
  int statusbyte;        /* statusbyte to send to MIDI */

  /* construct statusbyte : noteOff = 128+channel, noteOn= 144+channel */
  if(notestate == 0) statusbyte = 128+channel-1; else statusbyte = 144+channel-1; 

  /* send notes to MIDI */
  if(chordtype == 1) 
    { 
	   /* send single note to MIDI */
	   if(basenote>=0 && basenote <=127) MIDImessage(statusbyte, basenote, 100);
	  }
  else
    {
       /* send chord notes to MIDI if in range */
	   if(basenote>=0 && basenote <=115)
	   {
		   /* send basenote to MIDI */
		   MIDImessage(statusbyte, basenote, 100);

		   /* send additional notes if a CHORD is played */
       if(chordtype==2) {MIDImessage(statusbyte, basenote+4, 100); MIDImessage(statusbyte, basenote+7, 100);}
       if(chordtype==3) {MIDImessage(statusbyte, basenote+3, 100); MIDImessage(statusbyte, basenote+7, 100);}
       if(chordtype==4) {MIDImessage(statusbyte, basenote+4, 100); MIDImessage(statusbyte, basenote+8, 100);}
       if(chordtype==5) {MIDImessage(statusbyte, basenote+3, 100); MIDImessage(statusbyte, basenote+6, 100);}
       if(chordtype==6) {MIDImessage(statusbyte, basenote+5, 100); MIDImessage(statusbyte, basenote+7, 100);}
       if(chordtype==7) {MIDImessage(statusbyte, basenote+12, 100);}
       if(chordtype==8) {MIDImessage(statusbyte, basenote+4, 100); MIDImessage(statusbyte, basenote+7, 100); MIDImessage(statusbyte, basenote+10, 100);}
       if(chordtype==9) {MIDImessage(statusbyte, basenote+4, 100); MIDImessage(statusbyte, basenote+7, 100); MIDImessage(statusbyte, basenote+11, 100);}
       if(chordtype==10) {MIDImessage(statusbyte, basenote+3, 100); MIDImessage(statusbyte, basenote+7, 100); MIDImessage(statusbyte, basenote+10, 100);}
	   }
	}
	
} /* End of MIDIchord */


/* ---------------------------------------------------------------------------- */
/* MIDIpitchbend():                                                             */
/* Function:     Creates MIDI pitchbend                                         */
/* Arguments:    channel       : MIDI channel                                   */
/*               pbvalue       : pitch bend value to send                       */
/* ---------------------------------------------------------------------------- */
void MIDIpitchbend(int channel, int pbvalue)
{
  /* initalize variables */
  int statusbyte;         /* statusbyte to send to MIDI */
  int dataMSB;            /* MSB portion of the pitchbend value */
  int dataLSB;            /* LSB portion of the pitchbend value */

  /* construct statusbyte : 224+channel */
  statusbyte = 224+channel-1;

  /* get MSB and LSB values from the pitchbend value */
  dataMSB = pbvalue/128; if(dataMSB>127) dataMSB=127;
  dataLSB = pbvalue%128; if(dataLSB>127) dataLSB=127;

  /* send the pitchbend message to MIDI */
  MIDImessage(statusbyte, dataLSB, dataMSB);

} /* End of MIDIpitchbend */


/* ---------------------------------------------------------------------------- */
/* MIDImessage():                                                               */
/* Function:     Sends a MIDI messsage over the serial interface to MIDI OUT    */
/* Arguments:    statusbyte     : status byte for the MIDI message              */
/*               databyte1      : first data byte for the MIDI message          */
/*               databyte2      : second data byte for the MIDI message         */
/* ---------------------------------------------------------------------------- */
void MIDImessage(int statusbyte, int databyte1, int databyte2)
{
  Serial.write(statusbyte);
  Serial.write(databyte1);
  Serial.write(databyte2);

} /* End of MIDImessage */



