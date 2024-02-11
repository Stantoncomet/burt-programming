//LIBRARIES
#include <SPI.h>
#include <XBOXONE.h>
#include <Servo.h>

#include <burtLib.h>
#include <controller.h>
#include <motors.h>


//OBJECTS

//Xbox Controller
USB Usb;
XBOXONE Xbox(&Usb);

//Thrusters (which are basically servo motors, hence the Servo.h lib)
Servo vertFrontServo; // up/down front thruster
Servo vertBackServo;  // up/down back thruster
Servo frontLeftServo;
Servo frontRightServo;
Servo backLeftServo;
Servo backRightServo;




void setup() {
  Serial.begin(115200);

  //controller setup
  setupController();

  //motor setup
  vertFrontServo.attach(VERT_FRONT_SP); // better way to do this??
  vertBackServo.attach(VERT_BACK_SP);
  frontLeftServo.attach(FRONT_LEFT_SP);
  frontRightServo.attach(FRONT_RIGHT_SP);
  backLeftServo.attach(BACK_LEFT_SP);
  backRightServo.attach(BACK_RIGHT_SP);
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
