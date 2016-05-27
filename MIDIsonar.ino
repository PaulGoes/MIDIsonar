/* MIDIsonar                                                                    */
/* ---------------------------------------------------------------------------- */
/* Author:  Paul Goes                                                           */
/* Version: 0.2                                                                 */
/* Date:    27-5-2016                                                           */
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
/* ---------------------------------------------------------------------------- */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// initialize the push buttons
const int buttonPin1 = 15;   /* button 1 connected to pin 15 (analog A1) MODE SELECT */
const int buttonPin2 = 16;   /* button 2 connected to pin 16 (analog A2) SETUP NEXT  */
const int buttonPin3 = 17;   /* button 3 connected to pin 17 (analog A3) MENU ITEM 1 */
const int buttonPin4 = 18;   /* button 4 connected to pin 18 (analog A4) MENU ITEM 2 */
const int buttonPin5 = 19;   /* button 5 connected to pin 19 (analog A5) MENU ITEM 3 */

int buttonState = 0;

// initialize the data array with default values
int value[2][12] = {
  { 1, 1, 0, 10, 50, 1, 1, 0, 127, 1, 60, 71 },
  { 1, 2, 0, 10, 50, 1, 1, 0, 127, 1, 60, 71 }
};

// initialize the string array with menu names
char* menu[] = { "ACTV", "TYPE", "CHNL", "LDST", "HDST", "POLR", "CNTR", "LVAL", "HVAL", "NOTE", "LVAL", "HVAL" };

void setup() {
  // set up the LCD's number of columns and rows:
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);
  pinMode(buttonPin5, INPUT);
  
  /* set up the LCD screen as 2 lines of 16 characters */
  lcd.begin(16, 2);

  /* clear the LCD display */
  lcd.clear();

  /* display productname and version */
  lcd.setCursor(0, 0);
  lcd.print("MIDIsonar   v0.2");

  delay(1000);

  /* display the welcome message */
  for(int counter=0; counter<3; counter++){
      lcd.setCursor(0,1);
      lcd.print("by Paul Goes");

      delay(750);

      lcd.setCursor(0,1);
      lcd.print("            ");

      delay(750);
  }

  lcd.clear();

}

void loop() {

    MODEstandby();
    MODEsetup();
    MODEplay();
  
}


/* Function that handles the standby mode */
void MODEstandby(){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Mode: Standby");
  delay(500);

  do 
  {

  

  } while (digitalRead(buttonPin1) == LOW);

} /* End of MODEstandby */


/* Function that handles the setup mode */
void MODEsetup(){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Mode: Setup  ");
  delay(500);
  
  do 
  {

  

  } while (digitalRead(buttonPin1) == LOW);

} /* End of MODEsetup */


/* Function that handles the play mode */
void MODEplay(){

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Mode: Play   ");
  delay(500);
  lcd.clear();
  
  do 
  {

    for(int counter=0; counter<12; counter++){
      lcd.setCursor(0,0); lcd.print("   "); lcd.setCursor(0,0); lcd.print(menu[counter]);
      lcd.setCursor(0,1); lcd.print("   "); lcd.setCursor(0,1); lcd.print(String(value[0][counter]));
      lcd.setCursor(8,1); lcd.print("   "); lcd.setCursor(8,1); lcd.print(String(value[1][counter]));
      
      for(int timer=0; timer<1000; timer++){
        if(digitalRead(buttonPin1) == HIGH) return;
        delay(1);
      }
    }

  } while (digitalRead(buttonPin1) == LOW);

} /* End of MODEplay */

