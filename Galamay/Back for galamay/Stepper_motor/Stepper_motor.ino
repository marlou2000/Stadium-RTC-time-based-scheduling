#include <Stepper.h>
#include <SoftwareSerial.h> 
#include <AccelStepper.h>

#define FULLSTEP 4

const int stepsPerRevolution = 200;  // change this value to match the number of steps per revolution for your motor

#define motorPin1  8     // Blue   - 28BYJ48 pin 1
#define motorPin2  9     // Pink   - 28BYJ48 pin 2
#define motorPin3  10    // Yellow - 28BYJ48 pin 3
#define motorPin4  11    // Orange - 28BYJ48 pin 4
 
AccelStepper stepper(FULLSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

const int buttonPin = 7;
int buttonState = 0;
const int buttonPin1 = 4;
int buttonState1 = 0;

const int ledPinAutomaticSignal =  6;
const int ledPinManualSignal =  5;
const int ledPinOpenSignal =  13;
const int ledPinCloseSignal =  12;

bool buttonAutomatic = true;

String message;

int maxSpeedValue = 50;
int accelerationValue = 50;
int speedValue = 50;

void setup() {
  Serial.begin(9600);
  
  stepper.setMaxSpeed(maxSpeedValue);
  stepper.setAcceleration(accelerationValue);
  stepper.setSpeed(speedValue);

  pinMode(buttonPin, INPUT);
  pinMode(buttonPin1, INPUT);

  pinMode(ledPinAutomaticSignal, OUTPUT);
  pinMode(ledPinManualSignal, OUTPUT);
  pinMode(ledPinOpenSignal, OUTPUT);
  pinMode(ledPinCloseSignal, OUTPUT);

  digitalWrite(ledPinAutomaticSignal, HIGH);
  digitalWrite(ledPinManualSignal, LOW);
  digitalWrite(ledPinOpenSignal, LOW);
  digitalWrite(ledPinCloseSignal, LOW);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  buttonState1 = digitalRead(buttonPin1);
  
  if(buttonState == HIGH) {
      digitalWrite(ledPinAutomaticSignal, LOW);
      digitalWrite(ledPinManualSignal, HIGH);
      buttonAutomatic = false;
      
      int steps = 100;
      for(;;){
          buttonState = digitalRead(buttonPin);
          stepper.moveTo(steps);
          stepper.run();
          steps++;
    
          if(buttonState == LOW){
             stepper.stop();
             break;
          }
      }
  }

  if (buttonState1 == HIGH) {
      digitalWrite(ledPinAutomaticSignal, HIGH);
      digitalWrite(ledPinManualSignal, LOW);
      buttonAutomatic = true;
  }
  
  else if (Serial.available()>0 && buttonAutomatic == true) {
      message = Serial.readStringUntil('%'); 
      
      if(message == "Open"){
          digitalWrite(ledPinOpenSignal, HIGH);
          for(;;){
              buttonState = digitalRead(buttonPin);
        
              if(buttonState == HIGH) {
                  stepper.stop();
                  break;
              }
            
              stepper.moveTo(1000);
              stepper.run();
              
              if (!stepper.distanceToGo()) {
                digitalWrite(ledPinOpenSignal, LOW);
                break;
              } 
          }
      }

      else if(message == "Close"){
          digitalWrite(ledPinCloseSignal, HIGH);
          for(;;){
              buttonState = digitalRead(buttonPin);
        
              if(buttonState == HIGH) {
                  stepper.stop();
                  break;
              }
            
              stepper.moveTo(-1000);
              stepper.run();
              
              if (!stepper.distanceToGo()) {
                digitalWrite(ledPinCloseSignal, LOW  );
                break;
              } 
          }
      } 
  }

}
