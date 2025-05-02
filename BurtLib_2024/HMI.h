#ifndef HMI_h
#define HMI_h


/**
 * Milliseconds between sendind data to ROV.
*/
#define ROV_UPDATE_DELAY  50

/**
 * Value range where thrusters don't move
*/
#define THRUSTER_DEADBAND 10

/**
 * Additional boost signal to counteract drag on ROV
 * Should only be needed for forward/backward controls
 */
int drag_offset = 0;

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
  THRUSTER_1, //VERTICAL FRONT
  THRUSTER_2, //VERTICAL BACK
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
extern unsigned int Holding_Regs_HMI[HOLDING_REGS_SIZE];
// HOLDING_REGS_SIZE is the largest value in enum, so it's perfect for setting the size of the array.


void setupCommsHMI();

/**
 * Sends an entry of Holding_Regs via Serial1 to the ROV by constructing a
 *  sort of command. Ex: "W3:1600\0"
 * @param index The index of Holding_Regs to send.
*/
void writeToROV(int index);

/**
 * Sends each value of holding_regs to the ROV, one value each function call, incrimenting.
*/
void updateROVData();

#endif