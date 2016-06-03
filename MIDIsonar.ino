/* MIDIsonar                                                                    */
/* ---------------------------------------------------------------------------- */
/* Author:  Paul Goes                                                           */
/* Version: 0.4                                                                 */
/* Date:    3-6-2016                                                            */
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
/* ---------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------- */
/* Arduino Pin connections:                                                     */
/*    TXD - Midi Out Serial               D10 - Sonar A Led                     */
/*    D2  - LCD Bit 7 (pin 14)            D11 - LCD Clock (pin 6)               */
/*    D3  - LCD Bit 6 (pin 13)            D12 - LCD Reg Select (pin 4)          */    
/*    D4  - LCD Bit 5 (pin 12)            D13 - Sonar B Led                     */
/*    D5  - LCD Bit 4 (pin 11)            A0  - Potmeter Value                  */
/*    D6  - Sonar A Trigger               A1  - Button MODE SELECT              */
/*    D7  - Sonar A Echo                  A2  - Button MENU PREV                */
/*    D8  - Sonar B Trigger               A3  - Button MENU NEXT                */
/*    D9  - Sonar B Echo                                                        */
/* ---------------------------------------------------------------------------- */


// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// initialize the push buttons
const int buttonPin1 = 15;   /* button 1 connected to pin 15 (analog A1) MODE SELECT */
const int buttonPin2 = 16;   /* button 2 connected to pin 16 (analog A2) MENU PREV   */
const int buttonPin3 = 17;   /* button 3 connected to pin 17 (analog A3) MENU NEXT   */

int buttonState = 0;

// initialize the data array with default values
int value[2][12] = {
  { 1, 1, 1, 10, 50, 1, 1, 0, 127, 1, 60, 71 },
  { 1, 2, 1, 10, 50, 1, 1, 0, 127, 1, 60, 71 }
};

// initialize the data array with valuetypes
int type[12] = { 2, 3, 1, 1, 1, 4, 1, 1, 1, 5, 6, 6 };

// initialize the string array with menu names
char* menu[] = { "ACTV", "TYPE", "CHNL", "LDST", "HDST", "POLR", "CNTR", "LVAL", "HVAL", "NOTE", "LVAL", "HVAL" };

/* ---------------------------------------------------------------------------- */
/* Setup portion of the Arduino program.                                        */
/* ---------------------------------------------------------------------------- */

void setup() 
{
  // set up the LCD's number of columns and rows:
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);

  
  /* set up the LCD screen as 2 lines of 16 characters */
  lcd.begin(16, 2);

  /* clear the LCD display */
  lcd.clear();

  /* display productname and version */
  lcd.setCursor(0, 0);
  lcd.print("MIDIsonar   v0.4");

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

  lcd.clear();
  lcd.setCursor(0,0); lcd.print("MIDIsonar");
  lcd.setCursor(0,1); lcd.print("Standby");
  delay(1000);

  do 
  {
    if(digitalRead(buttonPin1) == HIGH) 
      {
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

  lcd.clear();
  lcd.setCursor(0,0); lcd.print("MIDIsonar");
  lcd.setCursor(0,1); lcd.print("Setup");
  delay(1000);
  lcd.clear();

  int menu_id = 0;      
  int menu_id_old = 3;
  int screen_id;
  int screen_id_old;

  int controller_id;
  int array_pos;

  char* valstr = "   ";

  do 
  {
    screen_id = menu_id/3;
    screen_id_old = menu_id_old/3;

    controller_id = menu_id/9;
    array_pos = menu_id%9;
    if(array_pos>5) array_pos=array_pos+3*(value[controller_id][1]-1);

    /* redraw the complete screen if the current menu item is on a new setup screen */
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

    /* draw the cursor at the place of the current menu item */
    lcd.noCursor();
    lcd.setCursor(5+(menu_id%3)*5,1); lcd.cursor();

    /* check for setup actions */
    do
    {
      /* MODE SELECT button pressed */
      if(digitalRead(buttonPin1) == HIGH)
      {
        do
            delay(20);
        while(digitalRead(buttonPin1) == HIGH);
        lcd.noCursor();
        return;
      }

      /* MENU PREV button pressed */
      if(digitalRead(buttonPin2) == HIGH) 
      {
        if(menu_id>0) 
        {
          menu_id_old=menu_id; 
          menu_id--;
          do
            delay(20);
          while(digitalRead(buttonPin2) == HIGH);
        } 
        break;
      }

      /* MENU NEXT button pressed */
      if(digitalRead(buttonPin3) == HIGH) 
      {
        if(menu_id<17) 
        {
          menu_id_old=menu_id; 
          menu_id++;
          do
            delay(20);
          while(digitalRead(buttonPin3) == HIGH);
        }
        break;
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

  char* valstr = "   ";

  lcd.clear();
  lcd.setCursor(0,0); lcd.print("MIDIsonar");
  lcd.setCursor(0,1); lcd.print("Play");
  delay(1000);
  lcd.clear();
  
  do 
  {

    for(int counter=0; counter<12; counter++)
    {

      lcd.setCursor(0,0); lcd.print("   "); lcd.setCursor(0,0); lcd.print(menu[counter]);
      lcd.setCursor(6,0); lcd.print("A:"); lcd.setCursor(6,1); lcd.print("B:");
      
      value2string(value[0][counter], valstr, type[counter]);
      lcd.setCursor(9,0); lcd.print("   "); lcd.setCursor(9,0); lcd.print(valstr);
      
      value2string(value[1][counter], valstr, type[counter]);
      lcd.setCursor(9,1); lcd.print("   "); lcd.setCursor(9,1); lcd.print(valstr);
      
      for(int timer=0; timer<1000; timer++)
      {
        if(digitalRead(buttonPin1) == HIGH) 
        {
          do
            delay(20);
          while(digitalRead(buttonPin1) == HIGH);
          return;
        }
        delay(1);
      }
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
        if(value==6) sprintf(valstr, "OCT");
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
  
}

