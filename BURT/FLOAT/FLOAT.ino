#include <Servo.h> //servo library
#include <RFM69.h> //transciever library
#include <SPI.h> //literally no idea what this does

RFM69 radio;


const char TEAMNUMBER[] = "r6"; //team number to display
const int MFBUTTONPIN = 2; //pin for the button that makes a stepper motor spin
const int MFMOTORPIN = 8; //servo pin
int MFButtonState;

const long PRINT_INTERVAL = 1000; //changed to 1000, doesn't need to print every 10th of a second
const unsigned int STARTCLOCK[] = {5, 11, 30}; //Input time here!
unsigned long startTime;
const long REALSTARTTIME = 77272; //<<<<<<<<
const long UPLOADTIME = 7;
const int DISPLAYLIMIT = 16;

const int MFMOTORINTERVAL = 1000;
const int SOLENOIDINTERVAL = 10000;

int lastButtonState = LOW;  // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers


const int SOLENOIDPIN = 4;

Servo MFMotor; //make a servo motor object

void setup() {
  Serial.begin(9600); //This would probably be different with the transceiver? maybe?
 
  pinMode(MFBUTTONPIN, INPUT); //button that makes the float go up
  pinMode(SOLENOIDPIN, OUTPUT); //Sets the solenoid pin as an output

MFMotor.attach(MFMOTORPIN);


  //converts clock time to seconds
  startTime = STARTCLOCK[0]*3600;
  startTime += STARTCLOCK[1]*60;
  startTime += STARTCLOCK[2];

  radio.initialize(RF69_915MHZ, 1, 0);
  radio.setHighPower();
}


void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(MFBUTTONPIN);

  unsigned long currentMillis = millis();
  static unsigned long previousMillis;

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != MFButtonState) {
      MFButtonState = reading;

      // only step if the new button state is HIGH
      if (MFButtonState == HIGH) {
        MFMotor.write(110);
        if (currentMillis - previousMillis >= MFMOTORINTERVAL) { //runs every PRINT_INTERVAL 1000 milliseconds
    previousMillis = currentMillis; //Reset timer

    MFMotor.write(90);
      }
    }
  }


  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;

  write_to_VFD();
  }


  //size of message being recieved, should just be "1", with a size of 1 character
  static char message[1];

  if (radio.receiveDone()) {
    for (byte i = 0; i < radio.DATALEN; i++)
      strcat(message, (char)radio.DATA[i]);
    

    if (message == "1") {

    digitalWrite(SOLENOIDPIN, HIGH);
    if (currentMillis - previousMillis >= SOLENOIDINTERVAL) { //runs every SOLENOIDINTERVAL 10000 milliseconds (10 sec)
      previousMillis = currentMillis; //Reset timer
  
      digitalWrite(SOLENOIDPIN, LOW);
      }
    }
  }



}


//void loop() {
  //write_to_VFD();


  //mFButtonState = digitalRead(MFBUTTONPIN);
  //if (mFButtonState) { //spin step if button is pressed
    //mFMotor.step(1);
  //}
//}


void write_to_VFD() {

  char temp[3]; //string for holding time to concat
  unsigned long minutes;
  unsigned long hours;
  unsigned long originalSeconds;
  unsigned long afterSeconds;


  unsigned long currentMillis;
  static unsigned long previousMillis;

  char time_team[DISPLAYLIMIT];   // string to write on display

  currentMillis = millis();

  if (currentMillis - previousMillis >= PRINT_INTERVAL) { //runs every PRINT_INTERVAL 1000 milliseconds
    previousMillis = currentMillis; //Reset timer


    originalSeconds = (currentMillis/1000)+REALSTARTTIME+UPLOADTIME; //changes millis to seconds and adds current clock time plus time it takes to upload
    hours = ((int)(originalSeconds/3600) % 24); //divides the total number of seconds by 3600 to make hours, turns into an integer to remove the decimal, then takes a remainder of 24
    minutes = ((int)(originalSeconds/60) % 60); //divides the total number of seconds by 60, turns that into an integer to remove the decimal, then takes a remainder of 60 (explained below)
    afterSeconds = (originalSeconds % 60); //factors the total number out of 60, then keeps the remainder (61 seconds becomes 1, 132 seconds becomes 12, etc.)
   
    //erase the string
    for(int i = 0; i < DISPLAYLIMIT; i++)
      time_team[i] = '\0';


    itoa(hours, temp, 10);
    strcat(time_team, temp);
    strcat(time_team, ":");

    if (minutes < 10)
      strcat(time_team, "0");
    itoa(minutes, temp, 10);
    strcat(time_team, temp);
    strcat(time_team, ":");

    if (afterSeconds < 10)
      strcat(time_team, "0");
    itoa(afterSeconds, temp, 10);
    strcat(time_team, temp);
   
    strcat(time_team, " #");
    strcat(time_team, TEAMNUMBER);

    //adds blank spaces to fill the display limit so it doesn't scroll and mess up
    int spaceLeft = DISPLAYLIMIT-strlen(time_team);
    for (int i = 0; i < spaceLeft; i++)
      strcat(time_team, " ");


    strcat(time_team, "\0");

    //write_to_monitor(time_team);
  }
  //send time and team number to box
   radio.send(3, time_team, DISPLAYLIMIT);
 
}


//void write_to_monitor(char* monitor_string) {
 // Serial.print(monitor_string); //print(), not println()
