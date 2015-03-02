/* Blink without Delay

 Turns on and off a light emitting diode (LED) connected to a digital
 pin, without using the delay() function.  This means that other code
 can run at the same time without being interrupted by the LED code.

 The circuit:
 * LED attached from pin 13 to ground.
 * Note: on most Arduinos, there is already an LED on the board
 that's attached to pin 13, so no hardware is needed for this example.
 
 created 2005
 by David A. Mellis
 modified 8 Feb 2010
 by Paul Stoffregen
 modified 11 Nov 2013
 by Scott Fitzgerald
 
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
 */

// constants won't change. Used here to set a pin number :
int speakerPin = PB4;     // Connect a piezo sounder between Ground and this pin
int potiPin = A3;         // Potentiometer connected to AnalogIn pin 3

// Variables will change :
int pinState = LOW;             // ledState used to set the LED
unsigned int analogValue; // define variable to store the sensor value

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMicros = 0;        // will store last time LED was updated

// constants won't change :
unsigned long phaseLength = 1000;           // interval at which to blink (milliseconds)

void setup() {
  // set the digital pin as output:
  pinMode(speakerPin, OUTPUT);
  pinMode(potiPin, INPUT);
  digitalWrite (potiPin, LOW);
}

void loop()
{
  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the
  // difference between the current time and last time you blinked
  // the LED is bigger than the interval at which you want to
  // blink the LED.
  unsigned long currentMicros = micros();
  phaseLength = 10 * (1023 - analogRead (potiPin));
  
  
  if(currentMicros - previousMicros >= phaseLength) {
    // save the last time you blinked the LED 
    previousMicros = currentMicros;   

    // if the LED is off turn it on and vice-versa:
    if (pinState == LOW)
      pinState = HIGH;
    else
      pinState = LOW;

    // set the LED with the ledState of the variable:
    digitalWrite(speakerPin, pinState);
  }
  
}

