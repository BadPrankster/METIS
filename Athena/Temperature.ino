// Code to measure the temperature
// August-September 2022
// Lucas Wirz-Vitiuk

// Load temperature libraries
#include <DallasTemperature.h>
#include <OneWire.h>

// Define pins
const int Temperature_Signal_Pin = 14;
const int Temperature_Power_Pin = 12;

// Configure the temperature-libraries
OneWire oneWire(Temperature_Signal_Pin);
DallasTemperature measure(&oneWire);

void setup_temperature_measurement() {

  // Setup temperature measurement and power it up
  pinMode(Temperature_Power_Pin, OUTPUT);
  digitalWrite(Temperature_Power_Pin, HIGH);
}

float measure_temperature(int sensor_number) {

  // Measure temperature
  measure.requestTemperatures();

  // Return temperature in degree Celsius
  return measure.getTempCByIndex(sensor_number);
}
