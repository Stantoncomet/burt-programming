#ifndef ROV_h
#define ROV_h

#define ROV_COMM_BUFF 24 // Length of buffer from HMI, presumably


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
  THRUSTER_1, //VERTICAL
  THRUSTER_2, //VERTICAL
  THRUSTER_3, //PORT BOW
  THRUSTER_4, //PORT QUARTER
  THRUSTER_5, //STARBOARD BOW
  THRUSTER_6, //STARBOARD QUATER

  HOLDING_REGS_SIZE
};

/**
 * This is the actual array of data.
 * This array represents data that is synced between HIM and ROV.
 * Each index in the array is a value that is used to 
 *   control a part of the ROV.
 * - To reference the value controlling the third thruster, you could
 *      use holding_regs[2], or holding_regs[THRUSTER_3], since THRUSTER_3
 *      is equivalent to 2.
*/
static unsigned int Holding_Regs[HOLDING_REGS_SIZE];
// HOLDING_REGS_SIZE is the largest value in enum, so it's perfect for setting the size of the array.


void setupComms();

/**
 * Recieve data from HMI
*/
void ROVCommunications();

#endif