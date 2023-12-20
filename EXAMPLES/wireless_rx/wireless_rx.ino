//RECIEVING MESSAGES WITH rfm69hcw adafruit RADIO
#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>

#define HOME_ADDR 2 //This arduino's transiever's address

RH_RF69 rf69(4, 3); //4: NSS/CS port; 3: data/G0 port
RHReliableDatagram rf69_manager(rf69, HOME_ADDR); //setup addresses and stuff

void setup() {
  Serial.begin(115200); //start serial
  while (!Serial) delay(1); //not needed if pc is not used

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

void loop() {
  if (rf69_manager.available()) { //if tranciever is running
    uint8_t len = sizeof(buf); //length of buffer
    if (rf69_manager.recvfromAck(buf, &len)) { //if we have got a message
      buf[len] = 0; // zero out remaining string
      Serial.print("yeah we go it, ");
      Serial.println((char*)buf); //print message
    }
  }
}
