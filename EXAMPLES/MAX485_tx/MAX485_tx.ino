//Sending messages over long distances with the MAX845 chip

char Mymessage[6] = "Hello"; //String data

void setup() {
  // Begin the Serial at 9600 Baud
  Serial1.begin(115200);
}

void loop() {
  
}


#define TX_ENABLE 22       // The number of the Tx ENABLE

// Creates a command to send to the ROV
void writeToROV(int index) { // Should look like Windex:1234\0 in the end

  char out_str[20]; // String containing the command to send to ROV
  char str[5];
  
  for (int i = 0; i < 20; i++) {
    out_str[i] = '\0'; // Clear buffer
  }
   
  strcpy(out_str, "W"); // Let ROV we are writing to it, command looks like "W"
  
  itoa(index, str, 10); // Turn index into a character array (a string)
  strcat(out_str, str); // Append index to the command, now says "Windex"

  strcat(out_str, ":"); // Now says "Windex:"
  
  itoa(HoldingRegs[index], str, 10); // Turn value into a character array
  strcat(out_str, str); // Now says "Windex:value\0"
 
  digitalWrite(TX_ENABLE, HIGH);  // Enable tx to MAX485
  Serial1.print(out_str);  // Print to rov; takes about 8 ms
  Serial1.flush();         // Empty xmit buffer
  digitalWrite(TX_ENABLE, LOW);

}