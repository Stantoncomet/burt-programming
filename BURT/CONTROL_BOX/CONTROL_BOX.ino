//LIBRARIES
#include <SPI.h>
#include <XBOXONE.h>
#include <Servo.h>

#include <burtLib.h>
#include <HMI.h>
#include <controller.h>
#include <motors.h>


//OBJECTS

//Xbox Controller
USB Usb;
XBOXONE Xbox(&Usb);




void setup() {
  Serial.begin(115200);

  //controller setup
  setupController();

}


void loop() {
  controllerRoutine(); // reads and sets all inputs to variables
  updateROVData(); // communication to ROV
  
}
