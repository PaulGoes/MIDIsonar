/* MIDIsonar                                                                    */
/* ---------------------------------------------------------------------------- */
/* Author:  Paul Goes                                                           */
/* Version: 0.1                                                                 */
/* Date:    22-5-2016                                                           */
/* ---------------------------------------------------------------------------- */ 
/* Revision history:                                                            */
/* v0.1    : Initial version LCD control scroll through modes.                  */
/* ---------------------------------------------------------------------------- */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

const int buttonPin1 = 15;   /* button 1 connected to pin 15 (analog A1) MODE SELECT */
const int buttonPin2 = 16;   /* button 2 connected to pin 16 (analog A2) ...         */
const int buttonPin3 = 17;   /* button 3 connected to pin 17 (analog A3) ...         */
const int buttonPin4 = 18;   /* button 4 connected to pin 18 (analog A4) ...         */

int buttonState = 0;


void setup() {
  // set up the LCD's number of columns and rows:
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);
  
  /* set up the LCD screen as 2 lines of 16 characters */
  lcd.begin(16, 2);

  /* clear the LCD display */
  lcd.clear();

  /* display productname and version */
  lcd.setCursor(0, 0);
  lcd.print("MIDIsonar   v0.1");

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

  lcd.setCursor(0,1);
  lcd.print("Mode: Standby");

do 
{

  

} while (digitalRead(buttonPin1) == LOW);
delay(500);
}


/* Function that handles the setup mode */
void MODEsetup(){

  lcd.setCursor(0,1);
  lcd.print("Mode: Setup  ");

do 
{

  

} while (digitalRead(buttonPin1) == LOW);
delay(500);
}


/* Function that handles the play mode */
void MODEplay(){

  lcd.setCursor(0,1);
  lcd.print("Mode: Play   ");

do 
{

  

} while (digitalRead(buttonPin1) == LOW);
delay(500);
}


