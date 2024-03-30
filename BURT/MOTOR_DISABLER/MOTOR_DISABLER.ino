#include <XBOXONE.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>


USB Usb;
XBOXONE Xbox(&Usb);

//variables containing the state of each of the thrusters inputs

 int Rise_State = 0;
 int Fall_State = 0;
 int Strafe_State = 0;
 int Move_State = 0;
 int Turn_State = 0;
 int Buck_State = 0;

//this shows how many motor control inputs are recieved 

 int Input_Level = Rise_State + Fall_State + Strafe_State + Move_State + Turn_State + Buck_State;

//this string determines if the motors can be on or not

 String Motor_Enabler = "Enabled";




void setup() {



  Serial.begin(115200);

  if (Usb.Init() == -1) {
        Serial.print(F("\r\nOSC did not start"));
        while (1); //halt
    }
    Serial.print(F("\r\nXBOX ONE USB Library Started"));

}

void loop() {

  motorEnabler(); //to call it as a function

}

void motorEnabler() {

  Usb.Task();

  if (Xbox.XboxOneConnected) { 

    Xbox.setRumbleOff();        //rumble sucks


    if (Xbox.getButtonPress(Y)) {
     Rise_State = 2;              //if an input is recieved for rising, store that in its "state," is 2 because there are 2 motors running to rise
     Serial.println("Y");         //print willl be removed
    } else {                                
     Rise_State = 0;
    }

    if (Xbox.getButtonPress(A)) {
      Fall_State = 2;
      Serial.println("A");
    } else {
      Fall_State = 0;
    }

     if (Xbox.getAnalogHat(LeftHatX) > 7500 || Xbox.getAnalogHat(LeftHatX) < -7500) {
      Strafe_State = 2;
      Serial.println(Xbox.getAnalogHat(LeftHatX));
    } else {
      Strafe_State = 0;
    }

    if (Xbox.getAnalogHat(LeftHatY) > 7500 || Xbox.getAnalogHat(LeftHatY) < -7500) {
      Move_State = 2;
      Serial.println(Xbox.getAnalogHat(LeftHatY));
    } else {
      Move_State = 0;
    }

    if (Xbox.getAnalogHat(RightHatX) > 7500 || Xbox.getAnalogHat(RightHatX) < -7500) {
      Turn_State = 2;
      Serial.println(Xbox.getAnalogHat(RightHatX));
    } else {
      Turn_State = 0;
    }

    if (Xbox.getAnalogHat(RightHatY) > 7500 || Xbox.getAnalogHat(RightHatY) < -7500) {
      Buck_State = 2;
      Serial.println(Xbox.getAnalogHat(RightHatY));
    } else {
      Buck_State = 0;
    }

    if (Input_Level > 2) {
      Motor_Enabler = "Disabled";   //if too many inputs are registered for motor control, this will disable all motors. used so only 2 motors can run at a time.
    } else {
      Motor_Enabler = "Enabled";
    }

  Input_Level = Rise_State + Fall_State + Strafe_State + Move_State + Turn_State + Buck_State;

  Serial.println(Motor_Enabler); //instead of printing, in real code it will be if Motor_Enabler = "Disabled" motor = off

  delay(1); // kill delay later

  }
}
