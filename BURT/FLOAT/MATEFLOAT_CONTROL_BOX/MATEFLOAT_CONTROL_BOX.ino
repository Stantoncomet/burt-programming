#include <RH_RF69.h>
#include <RHReliableDatagram.h>
#include <SPI.h>

#define HOME_ADDR 2 //This arduino's transiever's address
#define TO_ADDR 1 //this is the other arduino's address

RH_RF69 rf69(4, 3); //4: NSS/CS port; 3: data/G0 port
RHReliableDatagram rf69_manager(rf69, HOME_ADDR); //setup addresses and stuff

const int DOWNBUTTONPIN = 8;

int buttonState;//poop:)
int lastButtonState = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;


void setup() 
{
  Serial.begin(9600);

  pinMode(DOWNBUTTONPIN, OUTPUT);

  //manual reset the transiever, please don't ask me why
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
}

uint8_t buf[RH_RF69_MAX_MESSAGE_LEN]; //buffer where recieved message will be stored
char message[20] = "1"; //message to send to begin float sequence

void loop() 
{
  if (rf69_manager.available()) //if tranciever is running
  { 
    uint8_t len = sizeof(buf); //length of buffer
    if (rf69_manager.recvfromAck(buf, &len)) //if we have got a message
    { 
      buf[len] = 0; // zero out remaining string
      //strcat(buf, buf[RH_RF69_MAX_MESSAGE_LEN]); //not sure if this works but i think it does?
      //strcat(buf, '\0');

      Serial.print((char*)buf);
    }
  }

  int reading = digitalRead(DOWNBUTTONPIN);

  if (reading != lastButtonState) 
  {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) 
  {

    if (reading != buttonState) 
    {
      buttonState = reading;

      if (buttonState == 1) 
      {
        
        rf69_manager.sendtoWait((uint8_t *)message, strlen(message), TO_ADDR); //sends message
      }
    }
    
  }

  lastButtonState = reading;
}



