/*********** ******************************
 * HMI_header.H
 * build BUILD_2_3_22
 * build HMI_BUILD_3_25_2109
 *  written for Arduino Mega
 *  4_23_18 
 *  rev for port stbd motors, two vertical axis motors. 
 *  rev for crab motor.
 *  motor assignments are wired in the rov, Rov code stays the same.
 *  the twist pot is reassigned used for side side crab.
 *  
 *  3/20/2017
 *  s. pugh 
 *  rev for 3 motors using a/d joy stick input
 *  Rev 3. remove test mode to allow fwd, reverse switch. 
 *  REV 2. Added reverse motor control;
 * 
 * Test basic program interface to BlueRobotics T100 motor, using their ESC motor control module.
 * This uses the arduino PWM server library. the code thinks it is talking to a hobby servo positioner.
 * the rate of pulse is not all that critical. the pulse lenght determines the direction and speed. 
 * 1500 us at start is init control and for stop motor +- 25 us. deadband
 * 
 * Numbers greater than 1500 us is forward direction
 * Values less than 1500 us is reverse direction
 * range is 1100 to 1900 us pulse width. 
 * full thrust can be 5 lbs, at about 135 watts , 16 volts. 12.5 amps.. 
 * Avoid running long periods out of water. 
 * The UNO can support 6 servos
 *  * 
 * *****************************************/



/*
 * build HMI_BUILD_5_10_2109
 * This version is for the 2019 Mate ROV
 * 
 * 5.1.'18 
 * remapped out the motors. 
 * Rebuilt program to run port / stbd motors  two vertical motors, and one crab motor.
 * This is the operater control program for the rov.
 * Two programs one for the human operater, second resides in the robot.
 * the program is done to emulate  an industral OIT or HMI (name depending on your background)
 * the rov is designed to be a industral controller like a PLC.
 * The operator comptuer is Ardunio Mega
 * 
The ROV computer is Arduino Uno. 

 I  prefer defines rather than const vars. Personal choice
 also make #define all CAPS, makes easier debug
 the more modern C++ guideline suggest using const int instead.
 advanced use of #defines can cause uexepected problems.

 globals start with a cap
 local vars start with lower case

 */

#include <arduino.h>
#include <string.h>
#include "HMI_header.h"


#define  BUILD_2019   true // We will not erase existing code but rather not use it.
#define BUILD_2_3_22  true

void setup() 
{
 
  pinMode(CAMERA_SELECT, INPUT);    // initialize the pushbutton pin as an input: video 4
  pinMode(DISPLAY_SELECT, INPUT); // nav display  5

  pinMode(LEDPIN , OUTPUT);   // USED FOR TESTING on obard led output 13
   
  pinMode(TxENABLE,OUTPUT);  // connect pin 22 of MAX485
  Serial.begin(9600);       // communication channel for debugging on monitor
  Serial1.begin(9600);      // communication channel for HMI-ROV RS-484 link
  Serial2.begin(9600);      // communication channel for panel display only works at 9600 baud
   
 
}

void loop() 
{
  camera_select();  // pb for select camera , NUM_CAMERAS = 2
  simple_motors();  // 4 and 2 thruster control. see HMI_header.h for config

  Update_ROV_Data();  // communication to ROV via Serial1
  
   
} // end loop               


 /******************************** 
  *  assign test values will be over written
   *correct valuses will come from the nav chip
   *HoldingRegs{} is global array used for data xmission and receive.
  *************************************/
void Nav_test_values(void)
{
  HoldingRegs[COMPASS] = 99;  //realistic but random start values.
  HoldingRegs[TIP] = 300;
  HoldingRegs[TILT]= 234;
  HoldingRegs[TEMPERATURE] = 19;
}

/************************************
The prgram takes a single push button and at each press index to the next video camera.
The push button resides in the controller on the surface. It generates a camera select, sends the 
select down to the ROV where the actual selceiton happens. 

The program in final form is in two parts, and two computers. An operator control on the surface, and a camera select
on the ROV. 

********************************************/

void camera_select(void)  // called from HMI loop()
{
  int valid_button;
  valid_button = debounce2(CAMERA_SELECT);  // return -1 if no data, 0 low, 1 high
  if(valid_button >= 0) 
    button2(valid_button);
}

int debounce2(int button2)  // called from camera_select
{
  /* read the state of the switch into a local variable:
    check to see if you just pressed the button
    (i.e. the input went from LOW to HIGH),  and you've waited
    long enough since the last press to ignore any noise:
    If the switch changed, due to noise or pressing:
    reading   LastButtonState
    0   0      
    0   1
    1   0
    1   1
*/ 
  
  static int LastButtonState = LOW;   // the previous reading from the input pin
  static unsigned long LastDebounceTime = 0;  // the last time the output pin was toggled

  int status;
  int reading = digitalRead(button2);   // read button
  
 
  if (reading != LastButtonState)
  {
        LastDebounceTime = millis();     // reset the debouncing timer & start over
        status = -1;
  }
    
  LastButtonState = reading; 
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
  
  if ((millis() - LastDebounceTime) > DEBOUNCEDELAY) 
  {
    status = reading;  // good value can be high / low
  }
   return (status);   // return 0 if open, 1 if closed, -1 not ready
}



int button2(int state)// locate in HMI
{
  static int flag = 0;    //static is like a global variable
  static int light;       // that is only valid in function.
  int button;
  
  // need to debounce 
  
  if(state >= 0)  // if button press is good. 
  {
    // toggle lights...
    if (state && !flag)
    {
      flag = 1;
      button = false;
    }
    if (!state && flag)
    {
      flag = 0;
      light = ++light % NUM_CAMERAS;  // light will count 0,1,2,3
      // what will really happen is light is a value written in the command 
     
       HoldingRegs[VIDEO_CHANNEL] = light; // this resides in HMI
      //Serial.println(HoldingRegs[VIDEO_CHANNEL]);  // print out to see what is going on.    
    }
  }
} // end button2


 
/*
 * 
 * / SPEED_LIMIT is about 200
 * called from calc_motors()
 * read joystick read analog input 0-1023
 * converts the value to between +/- SPEED_LIMIT
 * any value between +/- DEAD_BAND is set to 0
 * returns value of speed +/- SPEED_LIMIT
 */
 int read_joystck (char analog_input_pin) 
 {
   
    int sensorValue;  // value between 0 - 1023
    int outputValue;  // value between between -SPEED_LIMIT, +SPEED_LIMIT  zero is stopped
    
    sensorValue = analogRead(analog_input_pin); // returns value  between 0 - 1023,  512 is midway
    outputValue = map(sensorValue, 0, 1023, -SPEED_LIMIT, +SPEED_LIMIT);
    //outputValue is a number between -SPEED_LIMIT, +SPEED_LIMIT  zero is stopped
    // typical  -200 to -10,  +10 to 200, values -10 to +10 are forced to zero.
    
    if((outputValue < DEAD_BAND) && (outputValue > -DEAD_BAND) )  
      outputValue = 0;   // force the stop command to servos and t-100 motors. add  to 1500 later
  
    return outputValue;  //return value between  -SPEED_LIMIT AND + SPEED_LIMIT
}

 
void simple_motors(void)
{
  int motor1_speed;
  int motor2_speed;
  int motor3_speed;
  int motor4_speed;
       // move to header file later.
//  int motor0_speed;
 
      motor1_speed = read_joystck( JOYSTICK_1);  // value between -SPEED_LIMIT  +SPEED_LIMIT
      motor2_speed = read_joystck( JOYSTICK_3);  // value between -SPEED_LIMIT  +SPEED_LIMIT
      motor3_speed = read_joystck( JOYSTICK_4);  // value between -SPEED_LIMIT  +SPEED_LIMIT
      motor4_speed = read_joystck( JOYSTICK_2);  // value between -SPEED_LIMIT  +SPEED_LIMIT
  
      // try this
     
      
    
      
      // maybe if motorspeed + DEAD_BAND then add a starting number like 50 just to give the system a kick.
      // DEAD_BAND exists because of system noise about +
      
#ifdef BUILD_2_3_22     // this is a test of an idea needs oneshot
       if(motor1_speed  >  DEAD_BAND)
          HoldingRegs[THRUSTER_1] = motor1_speed + INITSERVO + KICK_UP_SPEED ;
       if(motor1_speed  <  -DEAD_BAND)
          HoldingRegs[THRUSTER_1] = motor1_speed + INITSERVO -KICK_UP_SPEED;
       
       if(motor2_speed  >  DEAD_BAND)
          HoldingRegs[THRUSTER_2] = motor2_speed + INITSERVO + KICK_UP_SPEED;
        if(motor2_speed  <  -DEAD_BAND)
          HoldingRegs[THRUSTER_2] = motor2_speed + INITSERVO -KICK_UP_SPEED;

        if(motor3_speed  >  DEAD_BAND)
          HoldingRegs[THRUSTER_3] = motor3_speed + INITSERVO + KICK_UP_SPEED;
         if(motor3_speed  <  -DEAD_BAND)
          HoldingRegs[THRUSTER_3] = motor3_speed + INITSERVO -KICK_UP_SPEED;

        if(motor4_speed  >  DEAD_BAND)
          HoldingRegs[THRUSTER_4] = motor4_speed + INITSERVO + KICK_UP_SPEED;
        if(motor4_speed  <  -DEAD_BAND)
          HoldingRegs[THRUSTER_4] = motor4_speed + INITSERVO -KICK_UP_SPEED;
      
#else
      // this is the original code, motor comes to speed far to slowly
      HoldingRegs[THRUSTER_1]= constrain( motor1_speed,-SPEED_LIMIT, SPEED_LIMIT)+ INITSERVO ; // 
      HoldingRegs[THRUSTER_2]= constrain( motor2_speed,-SPEED_LIMIT, SPEED_LIMIT)+ INITSERVO ; // 
      HoldingRegs[THRUSTER_3]= constrain( motor3_speed,-SPEED_LIMIT, SPEED_LIMIT)+ INITSERVO ; // 
      HoldingRegs[THRUSTER_4]= constrain( motor4_speed,-SPEED_LIMIT, SPEED_LIMIT)+ INITSERVO ; // 
     // HoldingRegs[THRUSTER_5]= constrain( motor4_speed,-SPEED_LIMIT, SPEED_LIMIT)+ INITSERVO ; // 
#endif
}
  /* 
 *  HMI_ASCII_TEST.C
 *  module for build HMI_BUILD_3_25_2109
 */

/************************
 * serial communication is difficult, the MODBUS libs did not work
 * keep at it. 
 */


void Write_to_ROV(int index) // should look like Windex:1234\0  
{
  char OutStr[20]; 
  char str[5];
  
  for(int i = 0 ; i<20 ; i++)
   OutStr[i] = '\0';  // clear buffer.
   
  strcpy(OutStr,"W");
  
  itoa(index, str, 10); //Turn value into a character array 
  strcat(OutStr,str); // now says "Windex"
  strcat(OutStr,":"); // // now says "Windex:"
  
  itoa(HoldingRegs[index], str, 10); //Turn value into a character array
  strcat(OutStr,str);// now says "Windex:value\0"
 
  digitalWrite(TxENABLE, HIGH);  // enable tx to MAX485
  Serial1.print(OutStr);  // print to rov  takes about 8 ms
  Serial1.flush();      // empty xmit buffer.
  digitalWrite(TxENABLE, LOW);
 Serial.println(OutStr);  // print to rov  takes about 8 ms
     
}


void Update_ROV_Data(void)    // coomes here from loop()
{
  static unsigned int Last_data[HOLDING_REGS_SIZE];
  static unsigned long LastTime = 0; 
  static int index;
  // write every delay of about 50 ms. Data takes about 8 ms to xmit.
  
  if (( LastTime + MOTOR_UPDATE_DELAY) < millis()) 
  { 
    LastTime = millis();
    index = ++ index % HOLDING_REGS_SIZE;
    
    if(Last_data[index] != HoldingRegs[index] )
    {
      switch(index)
      {
        case COMPASS:
        case TIP:
        case TILT:
        case TEMPERATURE:
        // Read_ROV_Data( index);
          break;
        
        default:    // treat this as an else stmt
          Write_to_ROV(index);
          break;
      } // end switch
    
      Last_data[index] = HoldingRegs[index];
    }else
    {
      LastTime += MOTOR_UPDATE_DELAY;  // catch next time around
    } 
  }
}


/*
 * HMI_ASCII_TEST.C
 * Read_ROV_Data has to read values back up from the rov
 * there a problem at the moment
 * The data is valid at the rov
 * but the the rov and teh topside are trying to read and write at the same time.
 * 
 * 
 */
void Read_ROV_Data(int index)
{
   char OutStr[20]; 

  char str[5];
 
      for(int i = 0 ; i<20 ; i++)
        OutStr[i] = '\0';  // clear buffer.
      
     
      strcpy(OutStr,"R");
      itoa(index, str, 10); //Turn value into a character array 
      strcat(OutStr,str); // now says "Rindex"
      strcat(OutStr,'\0'); // // now says "Rindex\0
    
     
      digitalWrite(TxENABLE, HIGH);  // enable tx to MAX485
      Serial.println(OutStr);
      
      Serial1.print(OutStr);  // print to rov  takes about 8 ms
      Serial1.flush();      // empty xmit buffer.
      digitalWrite(TxENABLE, LOW);
      
      //Serial2.println('\n');  // print to vfd clear line
      //Serial2.print(OutStr);  // print to vfd

}

/*
 * MODUKE  Nav_control.c
 * module for build HMI_BUILD_3_25_2109
 * vfd_disply() reads input button
 * not used for 2019 contest
 * debounces the same.
 * selects the vfd element to display
 * 
 */
int Nav_display(void)  // called from loop()
{ 
  static int LastButtonState = LOW;   // the previous reading from the input pin
  static unsigned long LastDebounceTime = 0;  // the last time the output pin was toggled
  static unsigned long Display_update = millis();
  static int prev_button;
  static int selection = 0;
  int  reading;
 

  reading = digitalRead(DISPLAY_SELECT);   // read button 5 from input
  digitalWrite(LEDPIN, reading);  // Light pin 13

  if (reading != LastButtonState) //button state is changed, 
  {
        LastDebounceTime = millis();     // reset the debouncing timer & start over
        
  }
  LastButtonState = reading; // update the last state even if bad data.
   
    /* whatever the reading is at, it's been there for longer
     than the debounce delay, so take it as the actual current state
    */
  if (( LastDebounceTime + DEBOUNCEDELAY) > millis())  //DEBOUNCEDELAY = 50
  {

    // reading is good value can be high / low 1 or 0
    if (reading == 1) 
    {
      prev_button = 1;
    }
    
    if(!reading && prev_button) 
    {
      selection  = ++selection % (TEMPERATURE - COMPASS +1);  // TEMPERATURE == 10, COMPASS = 7
      switch_compass(selection + COMPASS);  // start count at enum 7 TO 10
      Display_update = millis();
      prev_button = 0;
    } 
  }
  
  // update display once per DISPLAY_DELAY about one second.
  if((Display_update + DISPLAY_DELAY) < millis() )
  {
    switch_compass(selection + COMPASS);  // selection will  be from the holding register enum
    Display_update = millis();
  }

}

/*
 * switch_compass(int vfd_display)
 * at the moment Serial is the default Serial
 * it should be Serial2 for mega
 * 
 */

void switch_compass(int vfd_display)
{
  //Serial.println(vfd_display);
   switch(vfd_display)   
   {
      case COMPASS:
          // compass
            Serial2.print('\n');
            Serial2.print("Compass = ");
            Serial2.print(HoldingRegs[vfd_display]);
            break; 
      
        
       case TIP:
       
            Serial2.print('\n');
            Serial2.print("Pitch = ");
            Serial2.print(HoldingRegs[vfd_display]- 200);
            break;
            
        case TILT:
            Serial2.print('\n');
            Serial2.print("Tilt = ");
            Serial2.print(HoldingRegs[vfd_display]-200);
             break;
            
        case TEMPERATURE:
            Serial2.print('\n');
            Serial2.print("Temerature = ");
            Serial2.print(HoldingRegs[vfd_display]);
             break;
    
         default:
            Serial2.print('\n');
            Serial2.print("Are you Lost? ");
            break;
    } 
}




 /*  vertical_motor(void)
  * Called from loop();
  * most motors use an analog input, for this we will use
  * two digital inputs. 
  * an input ramps up the motor speed to max limit then stops. 
  * 
  */  
void vertical_motor(void)
{
    /* a bit wordy but we solve for all 4 combinations of 2 buttons (or inputs)
    *  in practice a dpdt toggle is used spring back to off center. 
        #define PB_UP 2   for reference
        #define PB_DOWN 3   
        /MIN_SPEED (INITSERVO - SPEED_LIMIT)
        This routine is similar to the crab function
    */
    
    #define RAMP_SPEED 20  /// update speed bigger is slower 400 is to slow
    #define RAMP_STEP 10   // step size of speed change smaller is slower
    int up, down; // do this so we do not keep calling digitalRead() many times
    static unsigned last_speed;  // remeber speed last time we were here    
    static unsigned long LastTime = millis();  // remeber last time we were here
    
    if ((millis() - LastTime) > RAMP_SPEED)
    { 
        LastTime = millis();    // update the timer. 
        // look at buttons lets not debounce at this time. 
        up = digitalRead(PB_UP);
        down = digitalRead(PB_DOWN);
    
        if((!up && ! down) || (up && down) )   // reset motor to stop. (0,0 & 1,1)
            last_speed = INITSERVO;  // both false; expected. both true; a problem. 
            
#ifdef PB_MOTORS  // the motors are controlled by push button not joystick

    // insert new code here.  we need to think how to do this better.
    // start with original code
        if(up && ! down)   //go up,  
        {
            if(last_speed < MAX_SPEED)  // yea we can over speed by RAMP_STEP.
                last_speed += RAMP_STEP;          
        }
        
        if(!up && down)   //go down. 
        {
            if(last_speed > MIN_SPEED)  // yea we can under speed by RAMP_STEP.
                last_speed -= RAMP_STEP;          
        }
  // original code that does not make operater happy. from toggle switch, not joy
        if(up && ! down)   //go up,  
        {
            if(last_speed < MAX_SPEED)  // yea we can over speed by RAMP_STEP.
                last_speed += RAMP_STEP;          
        }
        
        if(!up && down)   //go down. 
        {
            if(last_speed > MIN_SPEED)  // yea we can under speed by RAMP_STEP.
                last_speed -= RAMP_STEP;          
        }
#endif
    } // if Last_Time
}  // end vertical_motor
