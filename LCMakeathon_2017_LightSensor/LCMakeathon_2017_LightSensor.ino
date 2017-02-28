// Feb 25-26, 2017
// Code created by Kevin Chan, Neil Sun, and Xiaoyu Yan at Life Changing Make-a-thon hosted at Cornell University.
// Hackathon Prompt: Smart Transporation
// Idea: Subway tracks are installed with a light sensor. Light source is installed directly above it on the ceiling.
//       Therefore, the lightSensor only stops receiving light when a subway is at that stop.
// Purpose: NOTIFIES USERS WHEN THEY HAVE ARRIVED ON THEIR STOP ON THE SUBWAY. Uses a light sensor to notify users that they have arrived 
//       at a stop. Users start by setting the number of stops they plan to go on. Everytime the light sensor stops receiving light, the 
//       counter decreases by 1. When the counter reaches 0,
//       the vibrator is turned on. User can stop the vibrator by clicking button. The process is then reset
// Additional Features: LCD RGB Backlight used to display, "How many stops? [that the user wants]", "Confirmed! [when user has
//       confirmed their choice]", "Stops remaining: [the number of stops remaining]", and "You have arrived! [when counter is 0]".


#include <Wire.h>
#include <rgb_lcd.h>  //initializes LCD RGB Backlight

rgb_lcd lcd;

const int colorR = 10;
const int colorG = 100;
const int colorB = 50;

#include <Grove_LED_Bar.h>
Grove_LED_Bar bar(9, 8, 0);  // Clock pin, Data pin, Orientation

int vibPin = 7;    // vibrator Grove connected to digital pin 2
int vibState = 0;

int lightSens = 0; //light sensor port
int lightState = 0;

int buttonPin = 3; //button port
int buttonState = 0;

int setterButton = 4; //setterbutton port
int setterButtonState = 0;

int numStops; // number of stops
int counter;  // counter for number of stops (decreases as user arrives at a station)
int resetCounter = 0; //counter for when to reset numstops

bool start = false;   // boolean to tell us when the lightSensor should start decreasing the counter
bool below200 = false; // boolean to prevent multiple counts when lightSensor reaches below 200 

void setup() {
  Serial.begin(9600);
      pinMode( vibPin, OUTPUT );
      pinMode( lightSens, INPUT);
      numStops = 0;
      counter = 1; //set to 1 just to prevent vibration. user is expected to set to number of stops.

      bar.begin(); //code for LED bar
      lcd.begin(16, 2);
      lcd.setRGB(colorR, colorG, colorB);

}

void loop(){
    
    if (counter > 10) {
      bar.setLevel(10); //setLevel can’t set the level greater than 10 
    } 
    else if (counter == 1 and start == false) {
      bar.setLevel(numStops); //illuminates a number of LEDs while setting number of stops
      lcd.setRGB(colorR, colorG, colorB); //Displays the number of stops on LCD RGB Backlight
      lcd.setCursor(0,0);
      lcd.print("How many stops?");
      lcd.setCursor(0,1);
      lcd.print(numStops);
    }
      
    else {
      bar.setLevel(counter); //illuminates a number of LEDS equal to stops left
    }
    if (start == true){
      lcd.setCursor(0,0);       //Displays the "Confirmed" notification on LCD RGB Backlight
      lcd.print("Confirmed!");
      lcd.setCursor(0,1);
      lcd.print(counter);
      }

    lightState = analogRead(lightSens);
    buttonState = digitalRead(buttonPin);
    setterButtonState = digitalRead(setterButton);
    vibState = digitalRead(vibPin);
    //Serial.println(lightState);      prints number of light sensor

    // if it is dark, counter decreases by 1
    if (lightState < 200 and below200 == false) {
        counter = counter - 1;
        below200 = true;
    }
    if (lightState >= 200) {
      below200 = false;
    }
    
    // if counter has reached 0 or below, a.k.a. user arrived at destination, vibrator starts
    if (counter <= 0 and start == true and numStops != 0) {
      Serial.println("You have arrived!");  // Displays "You have arrived" on LCD RGB Backlight
      lcd.clear();
      lcd.setRGB(0,250,0);
      lcd.setCursor(0,0);
      lcd.print("You have arrived");

       // turns vibrator on while button is not pressed
      while (buttonState == LOW) {
        //Serial.println("buttons pressed but setter not");
        buttonState = digitalRead(buttonPin);
        digitalWrite(vibPin, HIGH);
        
      }
      
      //button is pressed so vibrator is turned off
      digitalWrite(vibPin, LOW);   
      numStops = 0;
      counter = 1;
      start = false;
      resetCounter = 0;
      lcd.clear();
      
    }

    

    // if button and setter button are held at the same time, counter gets set to numStops
    if (buttonState == HIGH and setterButtonState == HIGH) { 
      Serial.println("both pressed");
      counter = numStops;
      start = true;
      resetCounter = 0;
      delay(200);
      lcd.clear();
      Serial.println(counter);
    }
    
    //if setter button is pressed (while button is NOT pressed), increase numStops by 1
    if (setterButtonState == HIGH and buttonState == LOW) {
      numStops = numStops + 1;
      Serial.println(numStops);
      delay(250);
    }

    // click on button to reset the numStops, in case user misclicks
    else if (setterButtonState == LOW and buttonState == HIGH){
      resetCounter ++;
      if (resetCounter == 100){
        numStops = 0;
        counter = 1;
        start = false;
        resetCounter = 0;
        lcd.clear();
        
      }
    }

}

