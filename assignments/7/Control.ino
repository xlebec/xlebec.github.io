// Include statements
#include <Stepper.h>
#include <SR04.h>
#include "SR04.h"

// Global constants
#define TRIG_PIN 13                // TRIG pin, defined by SR04.h and used by the sensor
#define ECHO_PIN 12                // ECHO pin, defined by SR04.h and used by the sensor
#define SERIAL_RATE 9600           // The rate of the COM serial connection from the Arduino to the computer
#define DELAY 16.67f               // The delay between readings in ms
#define DELAY_INIT 100             // The delay after initializing in ms
#define STEPS_PER_REVOLUTION 2050  // change this to fit the number of steps per revolution
#define GEAR_RADIUS 1.5f           // The radius of the gear
#define SERVO_RPM 10
#define MEDIAN_FILTER_WINDOW 25    // The number of medians to remember
#define FILTER_RATE 60             // The Hz at which to update the ultrasonic filter
#define LPF_BETA 0.15f             // The beta value for our low pass filter 0.05f

// Global Variables
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN); // The ultrasonic sensor object
float* sensorRecent = new float[MEDIAN_FILTER_WINDOW];
int sensorRecentIndex = 0;
float lpfMedian = 0; // The result of applying our low pass filter to our median filter.
float stepperPosition = 0;

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(STEPS_PER_REVOLUTION, 8, 10, 9, 11);


// SENSOR FILTER METHODS

// O(N^2) instead of O(N*log(N)), but using a small array so it doesn't matter
void bubbleSort(float input[]) {
  //Serial.println("bubble sort");
  boolean sorted = false;
  for (int i = 0, l = sizeof(input) - 1; i < l; i++) {
    boolean swapOccured = false;
    for (int j = i + 1; j < l; j++) {
      float currentValue = input[j];
      float otherValue = input[j + 1];
      if (currentValue > otherValue) {
        input[j] = otherValue;
        input[j + 1] = currentValue;
      }
    }
  }
}

// Gets a median from the array. This implementation is O(N^2) instead of O(N) like median of medians, but the array is small it so doesn't matter.
float getMedian(float input[]) {
  //Serial.println("get median");
  int len = sizeof(input);
  float* sortedValues = new float[len];
  for (int i = 0; i < len; i++) {
    sortedValues[i] = input[i];
  }
  bubbleSort(sortedValues);
  return sortedValues[len / 2];
}

float getFilteredSensorReading(float inputValue) {
  //Serial.println("get filtered sensor reading");

  // Our median filter. Add the current value to the record of recent values, then get the median.
  sensorRecent[sensorRecentIndex] = inputValue;
  sensorRecentIndex++;
  if (sensorRecentIndex >= sizeof(sensorRecent)) {
    sensorRecentIndex = 0;
  }
  float median = getMedian(sensorRecent);
  
  // Implement a LPF on the median filter - protection from spikes, smooth traversal
  lpfMedian = lpfMedian + LPF_BETA * (median - lpfMedian);
}


// STEPPER METHODS

// Given a position, as measured in cm from the home position (lens fully retracted), turn the stepper a number of steps to go to that position
void stepToPosition(int targetPosition) {
  //Serial.println("step to position");

  float distance = targetPosition - stepperPosition;

  // Find the angle the from distance (s = r * theta) in radians.
  float theta = distance / GEAR_RADIUS;

  // Find the number of steps from the angle.
  int steps = -theta * STEPS_PER_REVOLUTION / (2 * 3.14158f);

  // Method is blocking. Consider limiting distance in case of sensor spike.
  if ((steps > 0 && targetPosition <= 4) || (steps < 0 && targetPosition >= 0)) {
    //Serial.println("steps");
    //Serial.println(steps);
    myStepper.step(steps);
    stepperPosition = targetPosition;
  }
}


// MAIN METHODS

// Setup
void setup() {
  for (int i = 0, l = sizeof(sensorRecent); i < l; i++) {
    sensorRecent[i] = 0;
  }
  myStepper.setSpeed(SERVO_RPM);
  Serial.begin(SERIAL_RATE);
  delay(DELAY_INIT);
}

// Main loop
void loop() {
  //Serial.println("main loop");
  float distance = sr04.Distance();
  if (distance == 0) {
    distance = 300; // cm
  }
  getFilteredSensorReading(distance);
  // Serial.println(distance);
  // Serial.println(lpfMedian);
  // replace with clipped mapping
  if (lpfMedian < 40) {
    stepToPosition(3);
  } else if (lpfMedian > 60) {
    stepToPosition(0);
  }
  delay(DELAY); // Wait some time before reporting the next reading.
}
