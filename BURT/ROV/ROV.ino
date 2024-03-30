//LIBRARIES
#include <burtLib.h>
#include <ROV.h>
#include <motors.h>


void setup() {
  Serial.begin(115200);

  //controller setup
  setupController();

}


void loop() {
  controllerRoutine(); // reads and sets all inputs to variables
  updateROVData(); // communication to ROV

  
}
