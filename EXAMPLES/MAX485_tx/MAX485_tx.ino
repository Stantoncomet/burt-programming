//Sending ROV instruction data over long distances with the MAX845 chip

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



#define TX_ENABLE 22 // The pin number of the Tx ENABLE



void setup() {
  // Begin the Serial at 9600 Baud
  Serial1.begin(9600);
  Serial.begin(9600);

  pinMode(13, INPUT);

}

void loop() {
  holding_regs[THRUSTER_6] = digitalRead(13);
  writeToROV(THRUSTER_6);
  
  delay(10);
}

// Creates a command to send to the ROV
void writeToROV(int index) { // Should look like Windex:1234\0 in the end

  char out_str[20]; // String containing the command to send to ROV
  char str[5];

  digitalWrite(TX_ENABLE, HIGH);
  
  // Clear buffer
  for (int i = 0; i < 20; i++) {
    out_str[i] = '\0';
  }
   
  strcpy(out_str, "W"); // Let ROV know we are writing to it, command looks like "W"
  
  itoa(index, str, 10); // Turn index into a character array (a string)
  strcat(out_str, str); // Append index to the command, now says "Windex"

  strcat(out_str, ":"); // Now says "Windex:"
  
  itoa(holding_regs[index], str, 10); // Turn value into a character array
  strcat(out_str, str); // Now says "Windex:value\0"
 
  //digitalWrite(TX_ENABLE, HIGH);  // Enable tx to MAX485
  Serial1.print(out_str);  // Print to rov; takes about 8 ms
  Serial1.flush();         // Empty xmit buffer
  //digitalWrite(TX_ENABLE, LOW);

}