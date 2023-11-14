#include <Servo.h> 

const int switchPowerPin = 3;  // Pin to supply power to the switch
const int switchReadPin = 2;   // Pin to read the switch state
const int servoPin  = 9;

const int readFirePin = 10; // Pin to read input from mcu_controller nano
const int writeFireStatusPin = 11; // Pin to write to mcu_controller nano
Servo servo;

int switchState = 0;
int lastSwitchState = HIGH;
int servoAngleOn = 180;
int servoAngleOff = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 500;

void setup() {
  Serial.begin(9600);
  pinMode(switchPowerPin, OUTPUT);
  digitalWrite(switchPowerPin, HIGH); // Supply power to the switch
  pinMode(switchReadPin, INPUT);

// Pins for both reading fire-pin from other Nano, and to write status-value back if triggers. 
  pinMode(readFirePin, INPUT);
  pinMode(writeFireStatusPin, OUTPUT);
  servo.attach(servoPin);
}

void loop() {
  int reading = digitalRead(switchReadPin);

  if (reading != lastSwitchState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != switchState) {
      switchState = reading;


      if (switchState == LOW && digitalRead(readFirePin)) {
        servo.write(servoAngleOn);
        digitalWrite(writeFireStatusPin, HIGH);
        delay(50);
        digitalWrite(writeFireStatusPin, LOW);
        Serial.println("Moving Servo to ON position");
      } 
      
      else {
        servo.write(servoAngleOff);
        Serial.println("Moving Servo to OFF position");
      }
    }
  }

  lastSwitchState = reading;
}