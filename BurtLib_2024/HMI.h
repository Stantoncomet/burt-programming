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
 * We are using an enum here to give each index of the 
 *   holding_regs array an easily referencable name.
 * This is essentially like using one of these:
 *   `#define THRUSTER_1 0`
 *   `const int THRUSTER_2 = 1`
 *   But the entries, or enumerators, are automatically assigned values starting at 0
 *   and incrimenting by 1.
*/
enum {
  THRUSTER_1, //BACK LEFT
  THRUSTER_2, //BACK RIGHT
  THRUSTER_3, //FRONT LEFT
  THRUSTER_4, //BACK VERTICAL
  THRUSTER_5, //FRONT RIGHT
  THRUSTER_6, //FRONT VERTICAL
  ARM,

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

/**
 * Additional boost signal to counteract drag (or anything else) on ROV.
 * Same size as Holding_Regs because even tho we only have six thrusters,
 * we *could* give offsets to other components (like the manip) if wanted.
 */
extern int Drag_Offset[HOLDING_REGS_SIZE];


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
