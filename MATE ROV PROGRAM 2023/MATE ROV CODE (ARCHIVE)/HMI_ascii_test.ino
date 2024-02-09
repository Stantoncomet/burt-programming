/* 
 *  HMI_ASCII_TEST.C
 *  module for build HMI_BUILD_3_25_2109
 */
#include <string.h>
/************************
 * serial communication is difficult, the MODBUS libs did not work
 * keep at it. 
 */


void Write_to_ROV(int index) // should look like Windex:1234\0  
{
  char OutStr[20]; 
  char str[5];
  
  for(int i = 0 ; i<20 ; i++)
   OutStr[i] = '\0';  // clear buffer.
   
  strcpy(OutStr,"W");
  
  itoa(index, str, 10); //Turn value into a character array 
  strcat(OutStr,str); // now says "Windex"
  strcat(OutStr,":"); // // now says "Windex:"
  
  itoa(HoldingRegs[index], str, 10); //Turn value into a character array
  strcat(OutStr,str);// now says "Windex:value\0"
 
  digitalWrite(TxENABLE, HIGH);  // enable tx to MAX485
  Serial1.print(OutStr);  // print to rov  takes about 8 ms
  Serial1.flush();      // empty xmit buffer.
  digitalWrite(TxENABLE, LOW);
 Serial.println(OutStr);  // print to rov  takes about 8 ms
     
}


void Update_ROV_Data(void)    // coomes here from loop()
{
  static unsigned int Last_data[HOLDING_REGS_SIZE];
  static unsigned long LastTime = 0; 
  static int index;
  // write every delay of about 50 ms. Data takes about 8 ms to xmit.
  
  if (( LastTime + MOTOR_UPDATE_DELAY) < millis()) 
  { 
    LastTime = millis();
    index = ++ index % HOLDING_REGS_SIZE;
    
    if(Last_data[index] != HoldingRegs[index] )
    {
      switch(index)
      {
        case COMPASS:
        case TIP:
        case TILT:
        case TEMPERATURE:
        // Read_ROV_Data( index);
          break;
        
        default:    // treat this as an else stmt
          Write_to_ROV(index);
          break;
      } // end switch
    
      Last_data[index] = HoldingRegs[index];
    }else
    {
      LastTime += MOTOR_UPDATE_DELAY;  // catch next time around
    } 
  }
}


/*
 * HMI_ASCII_TEST.C
 * Read_ROV_Data has to read values back up from the rov
 * there a problem at the moment
 * The data is valid at the rov
 * but the the rov and teh topside are trying to read and write at the same time.
 * 
 * 
 */
void Read_ROV_Data(int index)
{
   char OutStr[20]; 

  char str[5];
 
      for(int i = 0 ; i<20 ; i++)
        OutStr[i] = '\0';  // clear buffer.
      
     
      strcpy(OutStr,"R");
      itoa(index, str, 10); //Turn value into a character array 
      strcat(OutStr,str); // now says "Rindex"
      strcat(OutStr,'\0'); // // now says "Rindex\0
    
     
      digitalWrite(TxENABLE, HIGH);  // enable tx to MAX485
      Serial.println(OutStr);
      
      Serial1.print(OutStr);  // print to rov  takes about 8 ms
      Serial1.flush();      // empty xmit buffer.
      digitalWrite(TxENABLE, LOW);
      
      //Serial2.println('\n');  // print to vfd clear line
      //Serial2.print(OutStr);  // print to vfd

}
