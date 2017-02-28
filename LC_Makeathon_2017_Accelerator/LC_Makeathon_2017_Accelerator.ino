// Feb 25-26, 2017
// Code created by Kevin Chan, Neil Sun, and Xiaoyu Yan at Life Changing Make-a-thon hosted at Cornell University.
// Hackathon Prompt: Smart Transporation
// Idea: Users keep an accelerator on their pocket/bag while they are on the subway. The accelerometer detects acceleration and deceleration to 
//       let us know that they have arrived at a station.
// Purpose: NOTIFIES USERS WHEN THEY HAVE ARRIVED ON THEIR STOP ON THE SUBWAY. Uses an accelerometer to countdown 
//       the number of stations that the user passes on the subway. Users start by setting the number of stops
//       they plan to go on. Everytime the accelerometer detects acceleration and deceleration to 
//       let us know that they have arrived at a station, the counter decreases by 1. When the counter reaches 0,
//       the vibrator is turned on. User can stop the vibrator by clicking button. The process is then reset
// Additional Features: LCD RGB Backlight used to display, "How many stops? [that the user wants]", "Confirmed! [when user has
//       confirmed their choice]", "Stops remaining: [the number of stops remaining]", and "You have arrived! [when counter is 0]".



#include <Wire.h>
#include <rgb_lcd.h>

rgb_lcd lcd;

const int colorR = 10;
const int colorG = 100;
const int colorB = 50;

#include <Grove_LED_Bar.h>
Grove_LED_Bar bar(9, 8, 0);  // Clock pin, Data pin, Orientation

int vibPin = 7;    // vibrator Grove connected to digital pin 2
int vibState = 0;  



int buttonPin = 3; //button port
int buttonState = 0;

int setterButton = 4; //setterbutton port
int setterButtonState = 0;

int numStops; // number of stops
int counter;  // counter for number of stops (decreases as user arrives at a station)
int resetCounter = 0; //counter for when to reset numstops

bool start = false;
bool moved = false;

#include <Wire.h>
#include <ADXL345.h>


ADXL345 adxl; //variable adxl is an instance of the ADXL345 library

void setup() {
  Serial.begin(9600);
      pinMode( vibPin, OUTPUT );
      
      numStops = 0;
      counter = 1; //set to 1 just to prevent initial vibration. user is expected to set to number of stops.

      bar.begin(); //code for LED bar
      lcd.begin(16, 2);
      lcd.setRGB(colorR, colorG, colorB);
      

  adxl.powerOn();

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
 
  //look of activity movement on this axes - 1 == on; 0 == off 
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);
 
  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);
 
  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);
 
  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment
 
  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
 
  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
 
  //register interrupt actions - 1 == on; 0 == off  
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);

}

void loop(){

    
  int x,y,z;  
  adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
  // Output x,y,z values 
  Serial.print("values of X , Y , Z: ");
  Serial.print(x);
  Serial.print(" , ");
  Serial.print(y);
  Serial.print(" , ");
  Serial.println(z);
  
  double xyz[3];
  double ax,ay,az;
  adxl.getAcceleration(xyz);
  ax = xyz[0];
  ay = xyz[1];
  az = xyz[2];
    
    if (counter > 10) {
      bar.setLevel(10); //setLevel can’t set the level greater than 10 (probably)
    } 
    else if (counter == 1 and start == false) {
      bar.setLevel(numStops); //illuminates a number of LEDs while setting number of stops
      lcd.setRGB(colorR, colorG, colorB);
      lcd.setCursor(0,0);
      lcd.print("How many stops?");
      lcd.setCursor(0,1);
      lcd.print(numStops);
    }
      
    else {
      bar.setLevel(counter); //illuminates a number of LEDS equal to stops left
    }


    
    buttonState = digitalRead(buttonPin);
    setterButtonState = digitalRead(setterButton);
    vibState = digitalRead(vibPin);
    //Serial.println(lightState);      prints number of light sensor

    // if it is dark, counter decreases by 1
    if (((xyz[0] > 0.5 and xyz[0]<1.0) or (xyz[1] > 0.5 and xyz[1] < 1.0) or (xyz[2] > 0.5 and xyz[2] < 1.0 )) and start) {

      // this if block makes it so that the counter decreases on every other thrust
      moved = not moved;    
      if (moved == false){
        counter = counter - 1;
      }
      
      lcd.clear();    
      lcd.setCursor(0,0);
      lcd.print("Stops Remaining");   // Displays the real-time number of stops remaining
      lcd.setCursor(0,1);
      lcd.print(counter);   
      if (counter > 0){
        delay(3000); 
      }
    }
    
    
    
    // if counter has reached 0 or below, a.k.a. user arrived at destination, vibrator starts
    if (counter <= 0 and start == true and numStops != 0) {
      Serial.println("You have arrived!");
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

    // if button is pressed (and setter button is NOT pressed), vibrator stops, and numStops gets reset to 0 and counter gets reset to 1; 
    

    // if button and setter button are held at the same time, counter gets set to numStops
    if (buttonState == HIGH and setterButtonState == HIGH) { 
      Serial.println("both pressed");
      counter = numStops;
      start = true;
      resetCounter = 0;
      delay(200);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Confirmed!");
      lcd.setCursor(0,1);
      lcd.print(numStops);
      Serial.println(counter);
    }
    
    //if setter button is pressed (while button is NOT pressed), increase numStops by 1
    if (setterButtonState == HIGH and buttonState == LOW) {
      numStops = numStops + 1;
      Serial.println(numStops);
      delay(250);
    }
    
    // click on button to reset the numStops, in case you misclicks.
    else if (setterButtonState == LOW and buttonState == HIGH){
      resetCounter ++;
      if (resetCounter == 50){
        numStops = 0;
        counter = 1;
        start = false;
        resetCounter = 0;
        lcd.clear();
        
      }
    }

}







