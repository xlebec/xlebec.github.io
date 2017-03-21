// Code based on sample from www.elegoo.com

// Include statements
#include <SR04.h>
#include "SR04.h"

// Global constants
#define TRIG_PIN 12      // TRIG pin, defined by SR04.h and used by the sensor
#define ECHO_PIN 11      // ECHO pin, defined by SR04.h and used by the sensor
#define SERIAL_RATE 9600 // The rate of the COM serial connection from the Arduino to the computer
#define DELAY 100        // The delay between readings in ms
#define DELAY_INIT 100   // The delay after initializing

// Variables
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN); // The ultrasonic sensor object
long input;                          // The input, in cm.

// Setup
void setup() {
  Serial.begin(SERIAL_RATE);
  delay(DELAY_INIT);
}

// Main loop
void loop() {
  Serial.println(sr04.Distance()); // Get the distance and print it, terminated by a '\n'character.
  delay(DELAY); // Wait some time before reporting the next reading.
}
