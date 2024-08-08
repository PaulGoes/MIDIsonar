/* MIDIsonar                                                                    */
/* ---------------------------------------------------------------------------- */
/* Author:  Paul Goes                                                           */
/* Version: 1.0                                                                */
/* Date:    15-10-2023                                                           */
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
/* ---------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/* Arduino Pin connections:                                                     */
/*    TXD - MIDI OUT Serial               D10 - Sonar A Led                     */
/*    D2  - LCD Bit 7 (pin 14)            D11 - LCD Clock (pin 6)               */
/*    D3  - LCD Bit 6 (pin 13)            D12 - LCD Reg Select (pin 4)          */    
/*    D4  - LCD Bit 5 (pin 12)            D13 - Sonar B Led                     */
/*    D5  - LCD Bit 4 (pin 11)            A0  - Potmeter Value                  */
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

/* initialize the Sonars */
#define triggerPinA   8      /* trigger pin sonar sensor A connected to pin D8 */
#define echoPinA      9      /* echo pin sonar sensor A connected to pin D9    */
#define triggerPinB   6      /* trigger pin sonar sensor B connected to pin D6 */
#define echoPinB      7      /* echo pin sonar sensor B connected to pin D7    */

NewPing sonarA(triggerPinA, echoPinA, 100);   /* NewPing setup of sonar A pins and maximum distance */
NewPing sonarB(triggerPinB, echoPinB, 100);   /* NewPing setup of sonar A pins and maximum distance */

/* initialize the data array with default values */
int value[2][12] = {
  { 1, 1, 1, 10, 50, 1, 1, 0, 127, 1, 60, 71 },
  { 1, 2, 1, 10, 50, 1, 1, 0, 127, 1, 60, 71 }
};

/* initialize the data array with valuetypes */
int type[12] = { 2, 3, 1, 1, 1, 4, 1, 1, 1, 5, 6, 6 };

/* initialize the string array with menu names */
char* menu[] = { "ACTV", "TYPE", "CHNL", "LDST", "HDST", "POLR", "CNTR", "LVAL", "HVAL", "NOTE", "LVAL", "HVAL" };


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
  lcd.print("MIDIsonar   v1.1");

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
  int menu_id = 0;        /* menu id of the selected menu item */
  int menu_id_old = 3;    /* menu id of the previous menu item */
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
    if(screen_id != screen_id_old)
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
        if(menu_id>0) 
        {
          /* decrease the menu_id */
          menu_id_old=menu_id; 
          menu_id--;
          /* wait until MENU PREV button is released */
          do
            delay(20);
          while(digitalRead(buttonPin2) == HIGH);
        } 
        break;
      }

      /* check if MENU NEXT button is pressed */
      if(digitalRead(buttonPin3) == HIGH) 
      {
        if(menu_id<17) 
        {
          /* increase the menu_id */
          menu_id_old=menu_id; 
          menu_id++;
          /* wait until MENU NEXT button is released */
          do
            delay(20);
          while(digitalRead(buttonPin3) == HIGH);
        }
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
                       if(temp_val == 3) temp_val = 1; /* wrap around */
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
              /* CNTR */
              case  6: temp_val = temp_val + 1;
                       if(temp_val == 128) temp_val = 0; /* wrap around */
                       break;
              /* CNTR LVAL */
              case  7: temp_val = temp_val + 1;
                       if(temp_val==value[controller_id][array_pos+1]) temp_val=value[controller_id][array_pos+1]-1; /* value of LVAL must be lower then value of HVAL */
                       break;
              /* CNTR HVAL */
              case  8: temp_val = temp_val + 1; 
                       if(temp_val == 128) temp_val = 127; /* no wrap around */
                       break;
              /* NOTE */
              case  9: temp_val = temp_val + 1;
                       if(temp_val == 9) temp_val = 1; /* wrap around */
                       break;
              /* NOTE LVAL */
              case 10: temp_val = temp_val + 1;
                       if(temp_val==value[controller_id][array_pos+1]) temp_val=value[controller_id][array_pos+1]-1; /* value of LVAL must be lower then value of HVAL */
                       break;
              /* NOTE HVAL */
              case 11: temp_val = temp_val + 1;
                       if(temp_val == 120) temp_val = 119; /* no wrap around */
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
                       if(temp_val == 0) temp_val = 2; /* wrap around */
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
              /* CNTR */
              case  6: temp_val = temp_val - 1;
                       if(temp_val == -1) temp_val = 127; /* wrap around */
                       break;
              /* CNTR LVAL */
              case  7: temp_val = temp_val - 1;
                       if(temp_val == -1) temp_val = 0; /* no wrap around */
                       break;
              /* CNTR HVAL */
              case  8: temp_val = temp_val - 1; 
                       if(temp_val==value[controller_id][array_pos-1]) temp_val=value[controller_id][array_pos-1]+1; /* value of HVAL must be higher then value of LVAL */
                       break;
              /* NOTE */
              case  9: temp_val = temp_val - 1;
                       if(temp_val == 0) temp_val = 8; /* wrap around */
                       break;
              /* NOTE LVAL */
              case 10: temp_val = temp_val - 1;
                       if(temp_val == 11) temp_val = 12; /* no wrap around */
                       break;
              /* NOTE HVAL */
              case 11: temp_val = temp_val - 1;
                       if(temp_val==value[controller_id][array_pos-1]) temp_val=value[controller_id][array_pos-1]+1; /* value of LVAL must be lower then value of HVAL */
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
  int oldValA = 0;        /* previous value measured for controller A */
  int noteStateA = 0;     /* State of the notes when type=NOT: 0=off, 1=on */

  int pingTimeOldB = 0;   /* previous ping time for controller B */
  int lowRangeB;          /* low range ping controller B in microseconds */
  int highRangeB;         /* high range ping controller B in microseconds */
  int rangePolB;          /* polarity controller B */
  int lowValB;            /* low value for controller B*/
  int highValB;           /* high value for controller B*/
  int newValB = 0;        /* current value measured for controller B */
  int oldValB = 0;        /* previous value measured for controller B */
  int noteStateB = 0;     /* State of the notes when type=NOT: 0=off, 1=on */

  int pingTime;           /* measured ping time between trigger and echo */
  int displayTime = 0;    /* display update timer to avoid value flickering */

  char* valstr = "   ";       /* temporary string used to construct formatted string values */

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

      /* display the controller CNTR or NOTE */
      if(value[counter][1]==1) value2string(value[counter][6], valstr, type[6]);
      else value2string(value[counter][9], valstr, type[9]);
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
  if(value[0][1]==1){ lowValA=value[0][7]; highValA=value[0][8];} else {lowValA=value[0][10]; highValA=value[0][11];}
  if(value[1][1]==1){ lowValB=value[1][7]; highValB=value[1][8];} else {lowValB=value[1][10]; highValB=value[1][11];}

  do 
  {
      /* if active, process controller A */
      if(value[0][0]==1)
      {
        /* send ping and get ping time in microseconds based on average of 3 pings */
        pingTime = sonarA.ping_median(3);   

        /* process the results when pingTime is within range */
        if (pingTime>lowRangeA && pingTime<highRangeA)
        {
          /* determine new value */
          newValA = int ( (pingTime-lowRangeA) / ( (highRangeA-lowRangeA) / (highValA-lowValA+1) ) );

          /* adjust value for polarity */
          if (rangePolA == 1) newValA = lowValA + newValA; else newValA = highValA - newValA;

          /* keep value within range in case of rounding errors */
          if (newValA<lowValA) newValA=lowValA; if (newValA>highValA) newValA=highValA;

          /* send MIDI controller message if value has changed */
          if ( (newValA!=oldValA) && (abs(pingTime-pingTimeOldA) > 10 ) )
          {
            /* set previous value of the pingTime */ 
            pingTimeOldA = pingTime;
            
            /* send CC message if TYPE is CC */
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

            /* send Note messages if TYPE is NOT */
            if(value[0][1]==2)
            {
              /* Send noteOff messages for the previous chord */
              MIDIchord(value[0][2], 0, oldValA, value[0][9]);

              /* Send noteOn messages for the new chord */
              MIDIchord(value[0][2], 1, newValA, value[0][9]);

              /* Switch noteState to On: notes are sounding */
              noteStateA = 1;

              /* display value on the LCD screen */
              value2string(newValA, valstr, 6);
              lcd.setCursor(13,0); lcd.print(valstr);
            }
             
          }

          /* remember the current value for next cycle */
          oldValA = newValA;
        } 
        
        /* process the results when pingTime is out of range */
        else
        {
          /* send Note message if TYPE is NOT and notes are sounding */
          if( (value[0][1]==2) && (noteStateA==1))
            {
              /* Send noteOff messages for the previous chord */
              MIDIchord(value[0][2], 0, oldValA, value[0][9]);

              /* Switch noteState to Off: notes are no longer sounding */
              noteStateA = 0;

              /* display value on the LCD screen */
              lcd.setCursor(13, 0);
              lcd.print("---");
            }
        }
        
      }

      /* if active, process controller B */
      if(value[1][0]==1)
      {
        /* send ping and get ping time in microseconds based on average of 3 pings */
        pingTime = sonarB.ping_median(3);   

        /* process the results when pingTime is within range */
        if (pingTime>lowRangeB && pingTime<highRangeB)
        {
          /* determine new value */
          newValB = int ( (pingTime-lowRangeB) / ( (highRangeB-lowRangeB) / (highValB-lowValB+1) ) );

          /* adjust value for polarity */
          if (rangePolB == 1) newValB = lowValB + newValB; else newValB = highValB - newValB;

          /* keep value within range in case of rounding errors */
          if (newValB<lowValB) newValB=lowValB; if (newValB>highValB) newValB=highValB;

          /* send MIDI controller message if value has changed */
          if ( (newValB!=oldValB) && (abs(pingTime-pingTimeOldB) > 10 ) )
          {
            /* set previous value of the pingTime */ 
            pingTimeOldB = pingTime;
            
            /* send CC message if TYPE is CC */
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

            /* send Note messages if TYPE is NOT */
            if(value[1][1]==2)
            {
              /* Send noteOff messages for the previous chord */
              MIDIchord(value[1][2], 0, oldValB, value[1][9]);

              /* Send noteOn messages for the new chord */
              MIDIchord(value[1][2], 1, newValB, value[1][9]);

              /* Switch noteState to On: notes are sounding */
              noteStateB = 1;

              /* display value on the LCD screen */
              value2string(newValB, valstr, 6);
              lcd.setCursor(13,1); lcd.print(valstr);
            }
             
          }

          /* remember the current value for next cycle */
          oldValB = newValB;
        } 
        
        /* process the results when pingTime is out of range */
        else
        {
          /* send Note message if TYPE is NOT and notes are sounding */
          if( (value[1][1]==2) && (noteStateB==1))
            {
              /* Send noteOff messages for the previous chord */
              MIDIchord(value[1][2], 0, oldValB, value[1][9]);

              /* Switch noteState to Off: notes are no longer sounding */
              noteStateB = 0;

              /* display value on the LCD screen */
              if(displayTime==0)
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

        /* send noteOff messages for the previous chord if type = NOT */
        if(value[0][1]==2) MIDIchord(value[0][2], 0, oldValA, value[0][9]);
        if(value[1][1]==2) MIDIchord(value[1][2], 0, oldValB, value[1][9]);

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
/*               type       : conversion type                                   */
/* ---------------------------------------------------------------------------- */
void value2string(int value, char *valstr, int type)
{
   switch(type)
   {
      case 1:
        sprintf(valstr, "%3d", value);
        break;
      case 2:
        if(value==0) sprintf(valstr, "OFF");
        if(value==1) sprintf(valstr, " ON");
        break;
      case 3:
        if(value==1) sprintf(valstr, " CC");
        if(value==2) sprintf(valstr, "NOT");
        break;
      case 4:
        if(value==0) sprintf(valstr, "NEG");
        if(value==1) sprintf(valstr, "POS");
        break;
      case 5:
        if(value==1) sprintf(valstr, "NOT");
        if(value==2) sprintf(valstr, "MAJ");
        if(value==3) sprintf(valstr, "MIN");
        if(value==4) sprintf(valstr, "AUG");
        if(value==5) sprintf(valstr, "DIM");
        if(value==6) sprintf(valstr, "SUS");
        if(value==7) sprintf(valstr, "OCT");
        if(value==8) sprintf(valstr, "7TH");
        break;
      case 6:
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
        break;
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

  /* send basenote to MIDI */
  MIDImessage(statusbyte, basenote, 100);

  /* send additional notes if NOTE =  MAJ, MIN, AUG, DIM, SUS, OCT or 7TH */
  switch(chordtype)
    {
      case 1: /* single note */
        break;
      case 2: /* major chord */ 
        MIDImessage(statusbyte, basenote+4, 100); MIDImessage(statusbyte, basenote+7, 100);
        break;
      case 3: /* minor chord */
        MIDImessage(statusbyte, basenote+3, 100); MIDImessage(statusbyte, basenote+7, 100);
        break;
      case 4: /* augmented chord */ 
        MIDImessage(statusbyte, basenote+4, 100); MIDImessage(statusbyte, basenote+8, 100); 
        break;
      case 5: /* diminished chord */ 
        MIDImessage(statusbyte, basenote+3, 100); MIDImessage(statusbyte, basenote+6, 100); 
        break;
      case 6: /* suspended chord */ 
        MIDImessage(statusbyte, basenote+5, 100); MIDImessage(statusbyte, basenote+7, 100); 
        break;
      case 7: /* octave */ 
        MIDImessage(statusbyte, basenote+12, 100); 
        break;
      case 8: /* seventh chord */ 
        MIDImessage(statusbyte, basenote+4, 100); MIDImessage(statusbyte, basenote+7, 100); MIDImessage(statusbyte, basenote+10, 100); 
        break;
    }
 
} /* End of MIDIchord */


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



