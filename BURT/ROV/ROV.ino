//LIBRARIES
#include <burtLib.h>
#include <ROV.h>
#include <motors.h>


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  setupComms();
  setupMotors();

}


void loop() {
  ROVCommunications(); // fetch data from control box
  writeMotorSpeeds(); // set motors

  
}
