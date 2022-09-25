// Code to actuate the LEDs
// August-September 2022
// Lucas Wirz-Vitiuk

// Define pins
const int green_Pin = 23;
const int yellow_Pin = 32;
const int red_Pin = 33;

// Define LED-variables
const int LED_delay = 250;

// LED-functions

void setup_LEDs() {
  pinMode(green_Pin, OUTPUT);
  pinMode(yellow_Pin, OUTPUT);
  pinMode(red_Pin, OUTPUT);
}

void green_Signal() {
  digitalWrite(green_Pin, HIGH);
  digitalWrite(yellow_Pin, LOW);
  digitalWrite(red_Pin, LOW);
}

void yellow_Signal() {
  digitalWrite(green_Pin, LOW);
  digitalWrite(yellow_Pin, HIGH);
  digitalWrite(red_Pin, LOW);
}

void red_Signal() {
  digitalWrite(green_Pin, LOW);
  digitalWrite(yellow_Pin, LOW);
  digitalWrite(red_Pin, HIGH);
}

void alarm(String level){
  if (millis()%(2*LED_delay)<LED_delay) {
    if (level == "yellow") {
      yellow_Signal();
    }
    if (level == "red") {
      red_Signal();
    }
  } else {
    if (level == "yellow") {
      digitalWrite(yellow_Pin, LOW);
    }
    if (level == "red") {
      digitalWrite(red_Pin, LOW);
    }
  }
}

void test_LEDs() {
  digitalWrite(green_Pin, HIGH);
  digitalWrite(yellow_Pin, HIGH);
  digitalWrite(red_Pin, HIGH);
  delay(10*LED_delay);
  for (int i = 0; i < 4; i++) {
    green_Signal();
    delay(LED_delay);
    yellow_Signal();
    delay(LED_delay);
    red_Signal();
    delay(LED_delay);
    yellow_Signal();
    delay(LED_delay);
  }
  green_Signal();
  delay(LED_delay);
  digitalWrite(green_Pin, LOW);
  digitalWrite(yellow_Pin, LOW);
  digitalWrite(red_Pin, LOW);
  delay(LED_delay);
}
