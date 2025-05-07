#include "ROV.h"

#include "pinsMapROV.h"
#include "Arduino.h"
#include "burtLib.h"

unsigned int Holding_Regs_ROV[HOLDING_REGS_SIZE];

void setupCommsROV() {
    pinMode(TX_ENABLE, OUTPUT);
    digitalWrite(TX_ENABLE, LOW);
}

void ROVCommunications() {
 
  int i, j;
  int val, index;
  char str[ROV_COMM_BUFF]; // String that receives data from HMI
  char tempchar[9]; // Used for temporalily storing chars when reading str
  
  digitalWrite(TX_ENABLE, LOW);  // Enable read, by disabling Tx
  
  if (Serial.available() > 0) { 
     delay(50);    // data takes 7.2 ms at 9600 baud, (from scope) 
    // wait for all data to be loaded. or use an endstring char like a ";"
    for (i = 0; i < ROV_COMM_BUFF ; i++)    //first clear out the buffer
        str[i] = '\0';
    
    i = 0; j = 0;
    //val = Serial.available();   // used for testing

    // Serial.read() reads next byte in the buffer each time it's called.
    // Each time a byte is read, Serial.available() decreases by 1.
    while (Serial.available() > 0) { // read data into char array
      str[i++] = Serial.read();    // heart of the comm. read all the data and put in str buffer.
    }
    
    str[i] = '\0';  // make sure the last byte is null.
      
    // If we recieve a write command, denoted by a starting 'W'
    if (str[0] == 87) {
      // first find the index data format is Windex:value
      i = 0; j = 0; 
  
      // Start parsing out the command
      // Go until a non digit is seen, ":"
      while (isdigit(str[++i])) { 
          tempchar[j++] = str[i];
      }
      tempchar[j] = '\0';    // gotta finish string with a null
      // todo: put test that checks index is between 0 and HOLDING_REGS_SIZE
      index = atoi(tempchar);  // now have valid holding_regs index value
        
      // find the value. skip past the ":"
      j = 0;
      while (isdigit(str[++i])) { // continue walking through str, starting from where we left off, ending at the '\0'
          tempchar[j++] = str[i];
      }
          
      tempchar[j] = '\0';
      val = atoi(tempchar);
      // todo: put test that value is good 
      Holding_Regs_ROV[index] = val; // this is the value to directly control outputs 
     }
 
    // Testing, print out recieved data
    Serial.print(index);
    Serial.print("  ");
    Serial.println(Holding_Regs_ROV[index]);    
    }
}

void iscool(){
    
}
