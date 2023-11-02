#include <RFM69.h>
#include <SPI.h>

RFM69 radio;

const int DOWNBUTTONPIN = 9;

int buttonState;
int lastButtonState = LOW; 

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; 



void setup() {
  Serial.begin(9600);

  pinMode(DOWNBUTTONPIN, INPUT);

  radio.initialize(RF69_915MHZ, 3, 0);
  radio.setHighPower();
}


void loop() {
  if (radio.receiveDone()) {
    static char time_team[16];

    for (byte i = 0; i < radio.DATALEN; i++)
      strcat(time_team, (char)radio.DATA[i]);
    
    write_to_monitor(time_team);
  }

  //sends "1" to the controlbox if the button is pressed
  if (digitalRead(DOWNBUTTONPIN)) {

  }


  int reading = digitalRead(DOWNBUTTONPIN);

  if (reading != lastButtonState) {
    lastDebounceTime = millis(); // reset the debouncing timer
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        //should work, sends a signal if buttonstate is high
        radio.send(1, "1", 1);
        Serial.println("sent signal 1");
      }
    }
  }
}


void write_to_monitor(char* monitor_string) {
  Serial.print(monitor_string); //print(), not println()
}
