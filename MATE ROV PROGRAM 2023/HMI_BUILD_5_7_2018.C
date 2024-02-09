
/*
 * build 5_7_2018
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

 globals start with a cap
 local vars start with lower case

 */

#include <arduino.h>
#include "HMI_header.h"


void setup() 
{
 
  pinMode(CAMERA_SELECT, INPUT);    // initialize the pushbutton pin as an input: video 4
  pinMode(DISPLAY_SELECT, INPUT); // nav display  5
  pinMode(PB_UP, INPUT); // up pin 2  used for vertical motor
  pinMode(PB_DOWN, INPUT); // down 3
  pinMode(PB_LEFT, INPUT); // up pin 6  used for crab motor
  pinMode(PB_RIGHT, INPUT); // down 7
 
  pinMode(LEDPIN , OUTPUT);   // USED FOR TESTING on obard led output 13
   
  pinMode(TxENABLE,OUTPUT);  // connect pin 22 of MAX485
  Serial.begin(9600);       // communication channel for debugging on monitor
  Serial1.begin(9600);      // communication channel for HMI-ROV RS-484 link
  Serial2.begin(9600);      // communication channel for panel display only works at 9600 baud

  Nav_test_values();  // assign test values will be over written  correct valuses will come from the nav chip
 
}

void loop() 
{
  camera_select();  // pb for select camera , NUM_CAMERAS = 2
  vertical_motor();  // two pb, up and down.
  Thrust_motors();  // 4 and 2 thruster control. see HMI_header.h for config
  #ifdef  CRAB_ENABLE    // crab can be done via analog or digital input see HMI_header.h for config
     crab_motor_control();
  #endif
  Update_ROV_Data();  // communication to ROV via Serial1
  Nav_display(); 
    
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
  * Thruster_control.c  module
  * crab is a rov that has a trasverse t-100 motor
  * we can use the rotation dial or a toggle switch. 
  */

 /* 
  *  
  *  Crab motor #6 used to move left - right.
  *  independent of other motors.
  *  direction not confirmered yet
  */
void crab_motor_control(void)
 {
  #ifdef  CRAB_JOYSTICK      // crab is controlld by the  rotation knob on the joystick
  
    int crab = read_joystck(JOYSTICK_R);  // return value of -SPEED_LIMIT, +SPEED_LIMIT
    HoldingRegs[CRAB_THRUSTER5] = crab + INITSERVO ; 
  
  #else                       // crab motor is push button or toggle switch 

    /* a bit wordy but we solve for all 4 combinations of 2 buttons (or inputs)
         in practice a dpdt toggle is used spring back to off center. 
          #define PB_LEFT 6               // aadded for creep control
          #define PB_RIGHT 7              // added for creep control         
          MIN_SPEED (INITSERVO - SPEED_LIMIT)
    */
    
    #define RAMP_SPEED 50  /// update speed bigger is slower 400 is to slow
    #define RAMP_STEP 20   // step size of speed change smaller is slower
    volatile  int left, right; // do this so we do not keep calling digitalRead() many times, volatile may not be necessary.
    static unsigned last_speed;  // remeber last time we were here
    static unsigned long LastTime = millis(); // remeber last time we were here
    
    if ((millis() - LastTime) > RAMP_SPEED)
    { 
      LastTime = millis();    // update the timer. 
      // look at buttons lets not debounce at this time. 
      left = digitalRead(PB_LEFT);
      right = digitalRead(PB_RIGHT);
      
      if((!left && ! right) || (left && right) )   // reset motor to stop. (0,0 & 1,1)
         last_speed = INITSERVO;  // both false; expected. both true; a problem. 
      
      if(left && ! right)   //go left,  
      {
        if(last_speed < MAX_SPEED)  // yea we can over speed by RAMP_STEP.
          last_speed += RAMP_STEP;     // we could use constrain() but really not critical     
      }
      
      if(!left && right)   //go right. 
      {
        if(last_speed > MIN_SPEED)  // yea we can under speed by RAMP_STEP.
           last_speed -= RAMP_STEP;   // we could use constrain() but really not critical          
      }   
      HoldingRegs[CRAB_THRUSTER5] = last_speed;     // update this ever RAMP_SPEED microseconds.
    
      
   } // if Last_Time
  #endif
 }

 
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
   #define DEAD_BAND 10   // what is correct value? was 5
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


 /*************************
  * This is for the 4 motors that are mounted on the corners of the
  * rov cube, x axis, y axis and r axis is when the analog knob is twisted.
  * Motors t1,t2,t3,t4
  * called from loop()
  * 
  * 2 motors are also used as optional contorol as of 5/1/18
  * motors are numbered differently
  */
 

void Thrust_motors(void)
{
  #ifdef FOUR_MOTORS
      
        int t1_x, t1_y, t1_r;
        int t2_x, t2_y, t2_r;
        int t3_x, t3_y, t3_r;
        int t4_x, t4_y, t4_r;
        int  x_speed, y_speed, r_speed;
        
        x_speed = y_speed = r_speed = 0;
        
        x_speed = read_joystck( JOYSTICK_X);  // value between -SPEED_LIMIT  +SPEED_LIMIT
        //Serial.print(x_speed);
        y_speed = read_joystck( JOYSTICK_Y);  // why was this commented out?
        r_speed = read_joystck( JOYSTICK_R);  // why was this commented out?
        
        // now we just read the joy stick and have 3 tested values that work with the T-100 motors.
        
        if((x_speed > DEAD_BAND) || (x_speed < -DEAD_BAND) )  // forward || reverse
        {
          t2_x = t3_x = x_speed;
          t1_x = t4_x = -x_speed;
        }
        
        if((y_speed > DEAD_BAND) || (y_speed < -DEAD_BAND) )  //right
        {
          t1_y = t2_y = y_speed;
          t3_y = t4_y = -y_speed;
          }
        
        if((r_speed > DEAD_BAND) || (r_speed < -DEAD_BAND) )  // rotate cw || ccw
        {
          t1_r = t3_r = r_speed;
          t2_r = t4_r = -r_speed;
        }
        
        HoldingRegs[THRUSTER1] = constrain(t1_x + t1_y + t1_r, -SPEED_LIMIT, SPEED_LIMIT) + INITSERVO;
        HoldingRegs[THRUSTER2] = constrain(t2_x + t2_y + t2_r, -SPEED_LIMIT, SPEED_LIMIT) + INITSERVO;
        HoldingRegs[THRUSTER3] = constrain(t3_x + t3_y + t3_r, -SPEED_LIMIT, SPEED_LIMIT) + INITSERVO;
        HoldingRegs[THRUSTER4] = constrain(t4_x + t4_y + t4_r, -SPEED_LIMIT, SPEED_LIMIT) + INITSERVO;
        
  #else       // 2 motor port and stbd motors
        
        int yaxis = read_joystck (JOYSTICK_Y);  // return value of -SPEED_LIMIT, +SPEED_LIMIT  a1
        int xaxis = read_joystck (JOYSTICK_X);  // return value of -SPEED_LIMIT, +SPEED_LIMIT   a0
        /*
        HoldingRegs[PORT_THRUSTER1]= yaxis;
        HoldingRegs[STBD_THRUSTER3] = yaxis;
         now add up these values for single joystick
        * all the way forward or back only on JOYSTICK_Y
        * will drive motor1 and motor2 at full speed fwd or rev.
        * so we will call map() again to return the T100 motor speeds
        * 1300 for full reverse, 1700 for forward.
        * 1500 for stop. still using SPEED_LIMIT defined as 200
       */
      //delay(500);
        HoldingRegs[PORT_THRUSTER5]= constrain( yaxis - xaxis,-SPEED_LIMIT, SPEED_LIMIT)+ INITSERVO ; // 
        HoldingRegs[STBD_THRUSTER3] = constrain( yaxis + xaxis,-SPEED_LIMIT, SPEED_LIMIT)+ INITSERVO ; //
      /*  
        Serial.print(HoldingRegs[PORT_THRUSTER5]); Serial.print("    ");
        Serial.println(HoldingRegs[STBD_THRUSTER3]);   
  */
  
/*
         Serial.print(read_joystck (JOYSTICK_Y)); Serial.print("    "); 
         Serial.print(read_joystck (JOYSTICK_R)); Serial.print("    ");
         Serial.println(read_joystck(JOYSTICK_X));
         */
  #endif
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
  #ifdef FOUR_MOTORS
      HoldingRegs[UP_THRUSTER2] = last_speed;     // update this ever RAMP_SPEED microseconds.
  #else
      HoldingRegs[UP_THRUSTER2] = HoldingRegs[UP_THRUSTER4] = last_speed;     // update this ever RAMP_SPEED microseconds.
  #endif
   
   } // if Last_Time
}  // end vertical_motor



/*
 * MODUKE  Nav_control.c
 * module for hmi_build
 * vfd_disply() reads input button
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
 *  Serial is the default testing
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

/* 
 *  HMI_ASCII_TEST.C
 *  module for HMI_BUILD_5_7_2018
 */
#include <string.h>
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
 * 
 * print this as a separate file
 * 
 */
/*****************************************
 * HMI_header.H
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

#include <Servo.h>  // loads servo class for pwm control of RC servos 

// #define FOUR_MOTORS   defined for a motor at each corner.
#define CRAB_ENABLE    true // rotational analog knob for left - right motor.
#define CRAB_JOYSTICK  true // rotational analog knob for left - right motor.  ould also be push button 
#define DEAD_BAND 10   // what is correct value? was 5 used for T100 motors

#define INITSERVO 1500  /* this is the init speed of the ESC controlleror stopped motor */
#define MOTOR_UPDATE_DELAY  50  // units of milliseconds
// speed_limit can be a
#define SPEED_LIMIT 200 // this limits top speed of motor for test. Actual motor range is 1100 to 1900, 

#define MIN_SPEED (INITSERVO - SPEED_LIMIT)
#define MAX_SPEED (INITSERVO + SPEED_LIMIT)

#define POSITION_LIMIT   600
#define NEG_POS_LIMIT 600   // this can change if required after fitted to mechanical device.
#define POS_POS_LIMIT 600 // this can change if required after fitted to mechanical device.

#define NUM_CAMERAS 2    // 2 cameras makes 
#define DISPLAY_DELAY 1000  // update the vfd once per second

#define PB_UP 2   
#define PB_DOWN 3   
#define CAMERA_SELECT   4       // the pin number of the video select pb for testing
#define DISPLAY_SELECT   5       // the number of the vfd display select pb for testing
#define PB_LEFT 6               // aadded for crab control not used but keep reserved.
#define PB_RIGHT 7              // added for crab control  not used but keep reserved.
#define TxENABLE 22       // the number of the Tx ENABLE
#define LEDPIN  13 // on board led used for testing
 // analog inputs
#define JOYSTICK_X  A2  // Select A/D input  pin     
#define JOYSTICK_Y  A0  // Select A/D input  pin     
#define JOYSTICK_R  A1  // Select A/D input  pin     
#define JOYSTICK_M1  A3 // Select A/D input  pin    // MANULIPATOR1
#define JOYSTICK_M2  A4 // Select A/D input  pin    // MANULIPATOR2



#define DEBOUNCEDELAY 50    // debounce time in ms.
#define VIDEO_SWITCH_DELAY 1000

void vertical_motor(void);
void Thrust_motors(void);
void vertical_motor(void);
void camera_select(void);
void Write_to_ROV(int index);
void read_camera_select(void);
int button2(int);
int debounce2(int);
void Update_ROV_Data(void);
void Two_motor_control(void);
int Nav_display(void);  // called from loop()


/*
 * 
 * hmi_header.h
 * This is a perfect place for an enum
 * these are the addresses for the data array that is sent to the ROV
 * could use something like const int PORT_THRUSTER = 2
 * HoldingRegs[] holds the data sent to the ROV
 * '
 * The labels have been updated using the port - starboard thruster motor config
 */

enum  // this HAS to match the same in the ROV
{     
    // just add or remove registers and your good to go...
    // MODBUS expects unsigned int uint16_t
    // The first register starts at address 0
    ADC_VAL,     // 0 system voltage need reference
    PWM_VAL,      // not used
    UP_THRUSTER2,  // UP MOTOR 2
    STBD_THRUSTER3,  //STBD MOTOR 3
    UP_THRUSTER4,  // UP MOTOR
    PORT_THRUSTER5,  // rev PORT 5
    CRAB_THRUSTER5,  // crab motor
    COMPASS,  // 7  // keep compass to temp group together. 
    TIP,    //works
    TILT,     //works     
    TEMPERATURE,    //works
    VIDEO_CHANNEL,  // 11
    MANIPULATOR1,
    MANIPULATOR2, 
    HOLDING_REGS_SIZE // leave this one as last or array size
    
};


unsigned int HoldingRegs[HOLDING_REGS_SIZE]; // Global array of data


