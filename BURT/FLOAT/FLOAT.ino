#include <PWMServo.h> //servo library
#include <RH_RF69.h>
#include <RHReliableDatagram.h> //transciever libraries
#include <SPI.h> //literally no idea what this does

#define HOME_ADDR 1 //This arduino's transiever's address
#define TO_ADDR 2 //this is the other arduino's address

RH_RF69 rf69(4, 3); //4: NSS/CS port; 3: data/G0 port
RHReliableDatagram rf69_manager(rf69, HOME_ADDR); //setup addresses and stuff

const char TEAMNUMBER[] = "r6"; //team number to display
const int MFBUTTONPIN = 7; //pin for the button that makes a servo motor spin
const int MFMOTORPIN = 9; //servo pin
int MFButtonState;

const long PRINT_INTERVAL = 1000; //changed to 1000, doesn't need to print every 10th of a second
const unsigned int STARTCLOCK[] = {5, 11, 30}; //Input time here!
unsigned long startTime;
const long REALSTARTTIME = 657700; //<<<<<<<<
const long UPLOADTIME = 7;
const int DISPLAYLIMIT = 16;

unsigned long timeOfSolenoid = 0;
bool solenoidOpen = false;
const int MFMOTORINTERVAL = 1000;
const int SOLENOIDINTERVAL = 3000;

int lastButtonState = LOW;  // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers



const int SOLENOIDPIN = 6;

PWMServo MFMotor; //make a servo motor object

void setup() 
{
  Serial.begin(9600); //This would probably be different with the transceiver? maybe?

  //manual reset the transiever, please don't ask me why
  //this could all be in a function
  pinMode(2, OUTPUT); //reset pin number
  digitalWrite(2, LOW);
  digitalWrite(2, HIGH);
  delay(10);
  digitalWrite(2, LOW);
  delay(10);

  //initialize
  rf69_manager.init();
  //set frequency
  rf69.setFrequency(434.0);
  //set high power mode (for RFM69HCW model)
  rf69.setTxPower(20, true);

  Serial.println("------------------------------");


  
  pinMode(MFBUTTONPIN, INPUT); //button that makes the float go up
  pinMode(SOLENOIDPIN, OUTPUT); //Sets the solenoid pin as an output

  MFMotor.attach(MFMOTORPIN);


  //converts clock time to seconds
  startTime = STARTCLOCK[0]*3600;
  startTime += STARTCLOCK[1]*60;
  startTime += STARTCLOCK[2];

 
}

uint8_t buf[RH_RF69_MAX_MESSAGE_LEN]; //buffer where recieved message will be stored

void loop() 
{
  // read the state of the switch into a local variable:
  int reading = digitalRead(MFBUTTONPIN);

  unsigned long currentMillis = millis();
  static unsigned long previousMillis;
  

  //test code:        
  if (digitalRead(MFBUTTONPIN) == HIGH)
  {                                       //will be removed
    Serial.print("b");
  }

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  
  if (reading != lastButtonState) 
  {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != MFButtonState) 
    {
      MFButtonState = reading;

      // only step if the new button state is HIGH
      if (MFButtonState == HIGH) 
      {
        Serial.print("hola"); //currently doesn't work, not sure why, debounce might not be necessary though
        MFMotor.write(110); //also debating switching to stepper or second solenoid
        if (currentMillis - previousMillis >= MFMOTORINTERVAL) 
        { //runs every PRINT_INTERVAL 1000 milliseconds
          previousMillis = currentMillis; //Reset timer

          MFMotor.write(90);
        }
      }
    }


    // save the reading. Next time through the loop, it'll be the lastButtonState:
    lastButtonState = reading;

  }

  
  write_to_VFD();


  if (rf69_manager.available()) 
  { //if tranciever is running
    uint8_t len = sizeof(buf); //length of buffer
    if (rf69_manager.recvfromAck(buf, &len) && solenoidOpen == false) //if we have got a message
    { 
      buf[len] = 0; // zero out remaining string
      //strcat(buf, (char)buf[RH_RF69_MAX_MESSAGE_LEN]); //not sure if this works but i think it does?

     
      Serial.print("1");
      solenoidOpen = true;
      timeOfSolenoid = currentMillis;
      
    }
    
  }
  if (solenoidOpen == true)
  {
    if (timeOfSolenoid + SOLENOIDINTERVAL >= currentMillis) //runs every SOLENOIDINTERVAL 10000 milliseconds (10 sec)
    { 
  
      digitalWrite(SOLENOIDPIN, HIGH);
    } else {
      digitalWrite(SOLENOIDPIN, LOW);
      solenoidOpen = false;
    }
  }
  
}




void write_to_VFD() 
{     

  char temp[3]; //string for holding time to concat
  unsigned long minutes;
  unsigned long hours;
  unsigned long originalSeconds;
  unsigned long afterSeconds;


  unsigned long currentMillis;
  static unsigned long previousMillis;

  char time_team[DISPLAYLIMIT];   // string to write on display

  currentMillis = millis();

  if (currentMillis - previousMillis >= PRINT_INTERVAL) //runs every PRINT_INTERVAL 1000 milliseconds
  { 
    previousMillis = currentMillis; //Reset timer


    originalSeconds = (currentMillis/1000)+REALSTARTTIME+UPLOADTIME; //changes millis to seconds and adds current clock time plus time it takes to upload
    hours = ((int)(originalSeconds/3600) % 24); //divides the total number of seconds by 3600 to make hours, turns into an integer to remove the decimal, then takes a remainder of 24
    minutes = ((int)(originalSeconds/60) % 60); //divides the total number of seconds by 60, turns that into an integer to remove the decimal, then takes a remainder of 60 (explained below)
    afterSeconds = (originalSeconds % 60); //factors the total number out of 60, then keeps the remainder (61 seconds becomes 1, 132 seconds becomes 12, etc.)
   
    //erase the string
    for(int i = 0; i < DISPLAYLIMIT; i++) 
    {
      time_team[i] = '\0';
    }

    itoa(hours, temp, 10);
    strcat(time_team, temp);
    strcat(time_team, ":");

    if (minutes < 10) 
    {
      strcat(time_team, "0");
    }
    itoa(minutes, temp, 10);
    strcat(time_team, temp);
    strcat(time_team, ":");

    if (afterSeconds < 10) 
    {
      strcat(time_team, "0");
    }
    itoa(afterSeconds, temp, 10);
    strcat(time_team, temp);
   
    strcat(time_team, " #");
    strcat(time_team, TEAMNUMBER);

    
      
    //adds blank spaces to fill the display limit so it doesn't scroll and mess up
    int spaceLeft = DISPLAYLIMIT-strlen(time_team);
    for (int i = 0; i < spaceLeft; i++)
    {
      strcat(time_team, " ");

      strcat(time_team, "\0");
    }
   
    

    //send time and team number to box
    rf69_manager.sendtoWait((uint8_t *)time_team, strlen(time_team), TO_ADDR); //sends message
 
  }

}



