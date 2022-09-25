// Code to control the servo actuator
// August-September 2022
// Lucas Wirz-Vitiuk

// Load servo library
#include <Servo.h>

// Define pin
const int Servo_Pin = 5;

// Define servo-variables
const int unlocked = 94;
const int locked = 20;

// Initiate servo
Servo lever;

void setup_servo() {
  lever.attach(Servo_Pin);
  lever.write(unlocked);
}

// Servo functions

void lock() {
  lever.write(locked);
}

void unlock() {
  lever.write(unlocked);
}
