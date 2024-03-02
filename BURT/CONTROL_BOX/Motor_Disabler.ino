
//variables containing the state of each of the thrusters inputs

 int Vert_Front_SP_State = 0;
 int Vert_Back_SP_State = 0;
 int Front_Left_SP_State = 0;
 int Front_Right_SP_State = 0;
 int Back_Left_SP_State = 0;
 int Back_Right_SP_State = 0;

//this shows how many motor control inputs are recieved 

 int Input_Level = Vert_Front_SP_State + Vert_Back_SP_State + Front_Left_SP_State + Front_Right_SP_State + Back_Left_SP_State + Back_Right_SP_State;

//this string determines if the motors can be on or not

 String Motor_Enabler = "Enabled";


void setup() {

 Serial.begin(9600);

  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
}

void loop() {

  if (digitalRead(2) == HIGH) {
    Vert_Front_SP_State = 1;              //if an input is recieved for the front vertical motor to go, store that in its "state"
  } else {                                // "digitalRead(2) == High" will be replaced by code checking for button press from controller
    Vert_Front_SP_State = 0;
  }

  if (digitalRead(3) == HIGH) {
    Vert_Back_SP_State = 1;
  } else {
    Vert_Back_SP_State = 0;
  }

  if (digitalRead(4) == HIGH) {
    Front_Left_SP_State = 1;
  } else {
    Front_Left_SP_State = 0;
  }

  if (digitalRead(5) == HIGH) {
    Front_Right_SP_State = 1;
  } else {
    Front_Right_SP_State = 0;
  }

  if (digitalRead(6) == HIGH) {
    Back_Left_SP_State = 1;
  } else {
    Back_Left_SP_State = 0;
  }

  if (digitalRead(7) == HIGH) {
    Back_Right_SP_State = 1;
  } else {
    Back_Right_SP_State = 0;
  }

  if (Input_Level > 2) {
    Motor_Enabler = "Disabled";   //if too many inputs are registered for motor control, this will disable all motors. used so only 2 motors can run at a time.
  } else {
    Motor_Enabler = "Enabled";
  }

Input_Level = Vert_Front_SP_State + Vert_Back_SP_State + Front_Left_SP_State + Front_Right_SP_State + Back_Left_SP_State + Back_Right_SP_State;

Serial.println(Motor_Enabler);
}
