//LIBRARIES
#include <XBOXONE.h>
#include <SPI.h>

#include <burtLib.h>
#include <controller.h>

//OBJECTS

//Xbox Controller
USB Usb;
XBOXONE Xbox(&Usb);

void setup() {
  Serial.begin(115200);
  setupController();

}


void loop() {
  //CONTROLLER
  Usb.Task();

  if (Xbox.XboxOneConnected) {

    //MOVEMENT
    if (Xbox.getButtonPress(MAP_RISE)) {
      //go up
      Serial.println("upping");
    }

    if (Xbox.getButtonPress(MAP_FALL)) {
      //go down
      Serial.println("downing");
    }

    if (forward()) {
      const MOVEVALUE = forward();
      Serial.print("choo choo  ");
      
      Serial.println(Xbox.getAnalogHat(MAP_STRAFE));
    }

    delay(1); //REPLACE WITH NOT THIS
  }
}
