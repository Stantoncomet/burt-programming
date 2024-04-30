//LIBRARIES
#include <burtLib.h>
#include <HMI.h>
#include <controller.h>
//#include <
//#include <motors.h>


void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);

  setupCommsHMI();
  setupController();

  //pinMode(A0, OUTPUT);

}


void loop() {
  controllerRoutine(); // Reads and sets all inputs to variables
  updateROVData(); // Communication to ROV
}
