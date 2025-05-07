#include "HMI.h"

#include "pinsMapHMI.h"
#include "Arduino.h"
#include "burtLib.h"

// define these again
int Drag_Offset[HOLDING_REGS_SIZE];
unsigned int Holding_Regs_HMI[HOLDING_REGS_SIZE];

void setupCommsHMI() {
    pinMode(TX_ENABLE, OUTPUT);
    digitalWrite(TX_ENABLE, LOW);
}

void writeToROV(int index) { // Should look like W[index]:1234\0 in the end

    char out_str[20]; // String containing the command to send to ROV
    char str[5];

    // Constantly enable MAX485 for testing
    //digitalWrite(TX_ENABLE, HIGH);
    
    // Clear buffer
    for (int i = 0; i < 20; i++) {
        out_str[i] = '\0';
    }
    
    strcpy(out_str, "W"); // Let ROV know we are writing to it, command looks like "W"
    
    itoa(index, str, 10); // Turn index into a character array (a string)
    strcat(out_str, str); // Append index to the command, now says "Windex"

    strcat(out_str, ":"); // Now says "W[index]:"
    
    itoa(Holding_Regs_HMI[index], str, 10); // Turn value into a character array
    strcat(out_str, str); // Now says "W[index]:value\0"
    
    digitalWrite(TX_ENABLE, HIGH);  // Enable tx to MAX485
    digitalWrite(TX_ENABLE, LOW);

    //debug
    //Serial.println(out_str);

}

void updateROVData() {
    static unsigned int last_data[HOLDING_REGS_SIZE];
    static unsigned long last_time = 0;
    static int index;

    // Write every delay of about 50 ms. Data takes about 8 ms to transmit.
    if ((last_time + ROV_UPDATE_DELAY) < millis()) {
        last_time = millis();
        index = ++index % HOLDING_REGS_SIZE; // Incriments index every time function is called

        if (last_data[index] != Holding_Regs_HMI[index]) {
            writeToROV(index);
            last_data[index] = Holding_Regs_HMI[index];

        } else {
            last_time += ROV_UPDATE_DELAY; // If value is unchanged, don't delay next call
        }
    }
}
