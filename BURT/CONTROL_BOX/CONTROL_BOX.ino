//LIBRARIES
#include <burtLib.h>
#include <HMI.h>
#include <controller.h>
#include <Arduino.h>
//#include <
//#include <motors.h>


void setup() {
  Serial.begin(115200); // Serial (0) is used for the 
  Serial1.begin(9600);

  setupCommsHMI();//human-machine interface
  setupController();
  
  

}


void loop() {
  controllerRoutine(); // Reads and sets all inputs to variables
  updateROVData(); // Communication to RObot

}
