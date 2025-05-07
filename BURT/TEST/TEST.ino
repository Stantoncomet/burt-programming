#include <Arduino.h>

void setup() {
    // initialize serial communication at 9600 bits per second:
    Serial2.begin(9600);
    //pinMode(A6, INPUT);
  }
  
  // the loop routine runs over and over again forever:
  void loop() {
    // read the input on analog pin 0:
    int sensorValue = analogRead(A0);
    // print out the value you read:
    Serial2.println(sensorValue);
    delay(1);  // delay in between reads for stability
  } 