/*
* Sunrise Simulator by Susan Rutherford
* Change the first set of variables below to reflect desired setup and sunrise times and durations.
* You must plug-in the arduino at the time you specify below in order to set the clock for the next day.
*/
#include <Time.h>
#include <TimeLib.h>

/* Change to the corresponding pin number on the board */
#define RED 10
#define GREEN 9
#define BLUE 11

/* Change to any value you like (Use a 24hr clock). Maximum values are 59 and 23 */
const unsigned int minutesOfYellow = 30; //How many minutes you would like the yellow light to increase
const unsigned int minutesOfBlue = 20; //How many minutes you would like the blue light to increase
const unsigned int minutesOfWhite = 10; //How many minutes you would like the white light to remain on full brightness
const unsigned int setupHour = 22; //The hour you will plug-in the arduino (ex: 22 for 10:00PM)
const unsigned int setupMinute = 0; //The minute you will plug-in the arduino (ex: 0 for *:00)
const unsigned int sunriseHour = 6;  //The hour you want the sunrise simulator to start (ex: 6 for 6:00AM)
const unsigned int sunriseMinute = 30;  //The minute you want the sunrise simulator to start (ex: 30 for *:30)

/* Constants - do not change */
#define secondsPerMinute 60
#define secondsPerHour 3600
#define secondsPerDay 86400
#define maxRgbValue 255

/* Calculations - do not change */
const float yellowIncPerSec = (float) maxRgbValue/(minutesOfYellow*secondsPerMinute);
const float blueIncPerSec = (float) maxRgbValue/(minutesOfBlue*secondsPerMinute);

/* Global variable declarations */
float yellowVal;
float blueVal;
unsigned long todayTime;
unsigned long startTime;
unsigned long turnOffTime;

void setup() {
  setTime(setupHour,setupMinute,0,1,1,0); // start the clock

  todayTime = now()-(now()%secondsPerDay); //the epoch time at midnight this morning
  startTime = todayTime 
  + secondsPerDay
  + (((unsigned long)sunriseHour * (unsigned long)secondsPerHour) 
  + ((unsigned long)sunriseMinute * (unsigned long)secondsPerMinute)); //this morning's epoch time plus the user-configured start time in seconds
  turnOffTime = startTime+(minutesOfYellow*secondsPerMinute 
  + minutesOfBlue*secondsPerMinute 
  + minutesOfWhite*secondsPerMinute); //sunrise ending time is the start time plus the length of each part of the sunrise

  // Configure each pin to produce output instead of receive input
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);  

  // Set data rate to 9600 baud
  Serial.begin(9600);

  // Send 0 to each pin to reset LED to complete darkness
  analogWrite(RED,0);
  analogWrite(GREEN,0);
  analogWrite(BLUE,0);

  // Initialize variables to 0
  yellowVal = 0;
  blueVal = 0;

  Serial.print("Start time: ");
  Serial.println(startTime);
  Serial.print("Turn off time: ");
  Serial.println(turnOffTime);
}

void loop(){
  sunrise();
  Serial.print("Current time: ");
  Serial.print(hour());
  Serial.print(":");
  Serial.print(minute());
  Serial.print(":");
  Serial.println(second());
  Serial.print("Current time in seconds: ");
  Serial.println(now());
  if (startTime>now()) {
   Serial.print("Seconds remaining until next sunrise: ");
   Serial.println(startTime-now()); 
  }
  delay(1000); //wait 1 second (1s=1000ms)
}

void sunrise(){
  if(now()>startTime && now()<turnOffTime){
     Serial.print("Current yellow/blue values: ");
     Serial.print(yellowVal);
     Serial.print("/");
     Serial.println(blueVal);

     if(yellowVal < maxRgbValue){
       analogWrite(RED,yellowVal);
       analogWrite(GREEN,yellowVal);
       yellowVal = (float)(now()-startTime) * yellowIncPerSec; //difference in current time and start time represents the number of seconds elapsed since sunrise started
     }
     else if (blueVal < maxRgbValue) {
      analogWrite(BLUE,blueVal);
      blueVal = (float)(now()-startTime-(minutesOfYellow*secondsPerMinute)) * blueIncPerSec; //this value should start from 0 after the yellow phase is complete
     }
     else {
       // Display max brightness white light
       analogWrite(RED,maxRgbValue);
       analogWrite(GREEN,maxRgbValue);
       analogWrite(BLUE,maxRgbValue);
     }
  }
  else if (now()>turnOffTime && yellowVal!=0) {
    // Turn off light
    Serial.print("Turning off the light");
    analogWrite(RED,0);
    analogWrite(GREEN,0);
    analogWrite(BLUE,0);
    yellowVal = 0;
    blueVal = 0;
    // Add 1 day to start/end time so it will repeat tomorrow
    startTime += secondsPerDay;
    turnOffTime += secondsPerDay;
  }
}
