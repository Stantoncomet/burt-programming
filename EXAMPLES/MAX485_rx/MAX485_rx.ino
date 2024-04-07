//Receiving messages over long distances with the MAX845 chip

/**
 * We are using an enum here to give each index of the 
 *   holding_regs array an easily referencable name.
 * This is essentially like using one of these:
 *   `#define THRUSTER_1 0`
 *   `const int THRUSTER_2 = 1`
 *   But the entries, or enumerators, are automatically assigned values starting at 0
 *   and incrimenting by 1.
*/
enum {
  THRUSTER_1,
  THRUSTER_2,
  THRUSTER_3,
  THRUSTER_4,
  THRUSTER_5,
  THRUSTER_6,

  HOLDING_REGS_SIZE
};
/**
 * This is the actual array of data.
 * This array represents data that is synced between HIM and ROV.
 * Each index in the array is a value that is used to 
 *   control a part of the ROV.
 * To reference the value controlling the third thruster, you could
 *   use holding_regs[2], or holding_regs[THRUSTER_3], since THRUSTER_3
 *   is equivalent to 2.
*/
unsigned int holding_regs[HOLDING_REGS_SIZE]; // HOLDING_REGS_SIZE is the largest value in enum, so it's perfect for setting the size of the array.

/**
 * The pin number of the Tx ENABLE, powers both the Tx and Rx enable.
 * However Rx is enabled when set to LOW, and disabled when set HIGH,
 *   so we control it with just one pin.
*/
#define TX_ENABLE 2
#define ROV_COMM_BUFF 24 // Length of buffer from HMI, presumably

void setup() {
  // Begin the Serial at 9600 Baud
  Serial1.begin(9600);
  Serial.begin(9600);
}

void loop() {
  ROVCommunications();
}


void ROVCommunications() {
 
  int i, j;
  int val, index;
  char str[ROV_COMM_BUFF]; // String that receives data from HMI
  char tempchar[9]; // Used for temporalily storing chars when reading str
  
  digitalWrite(TX_ENABLE, LOW);  // Enable read, by disabling Tx
  
  if (Serial1.available() > 0) { 
     delay(20);    // data takes 7.2 ms at 9600 baud, (from scope) 
    // wait for all data to be loaded. or use an endstring char like a ";"
    for (i = 0; i < ROV_COMM_BUFF ; i++)    //first clear out the buffer
        str[i] = '\0';
    
    i = 0; j = 0;
    //val = Serial.available();   // used for testing

    // Serial.read() reads next byte in the buffer each time it's called.
    // Each time a byte is read, Serial.available() decreases by 1.
    while (Serial1.available() > 0) { // read data into char array
      str[i++] = Serial1.read();    // heart of the comm. read all the data and put in str buffer.
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
      // todo: put test that value is good (OM: ???)
      holding_regs[index] = val; // this is the value to directly control outputs 
     }
 
    Serial.print(index);
    Serial.print("  ");
    Serial.println(holding_regs[index]);    
    }
}