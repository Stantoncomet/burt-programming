//Receiving messages over long distances with the MAX845 chip

char Mymessage[10]; //Initialized variable to store recieved data
int iCallCount = 0;

void setup() {
  // Begin the Serial at 9600 Baud
  Serial1.begin(115200);
  Serial.begin(115200);
}

void loop() {
  
}


void ROV_communications() {
 
  int i, j;
  int val, index;
  char str[ROV_COMM_BUFF];
  char tempchar[9];
  
  digitalWrite(TxENABLE, LOW);  // read enable, Tx disable
  
  if (Serial.available() > 0) { 
     delay(20);    // data takes 7.2 ms at 9600 baud, (from scope) 
      // wait for all data to be loaded. or use an endstring char like a ";"
      for (i = 0; i < ROV_COMM_BUFF ; i++)    //first clear out the buffer
          str[i] = '\0';
     
      i = 0; j = 0;
      //val = Serial.available();   // used for testing
      while(Serial.available() > 0) { // read data into char array
        str[i++] =  Serial.read();    // heart of the comm. read all the data and put in str buffer.
      }  // while  (Serial.available)
      
      str[i] = '\0';  // make sure the last byte is null.
        
      if(str[0] == 87) { // have a write value command  'W' to HoldingRegs[index] 
        // first find the index data format is Windex:value
        i = 0; j = 0;
   
        while(isdigit(str[++i])) { // start parsing out the command go until a non digit is seen ":"
            tempchar[j++] = str[i];
        }
        tempchar[j] = '\0';    // gota finish string with a null
        // todo put test that index is between 0 and  HOLDING_REGS_SIZE
        index = atoi(tempchar);  // now have valid HoldingReg index value
          
        // find the value. skip past the ":"
        j = 0;
        while(isdigit(str[++i])) { // continue walking throug str
            tempchar[j++] = str[i];
        }
            
        tempchar[j] = '\0';
        val = atoi(tempchar);
        // todo put test that value is good
        HoldingRegs[index]  =  val;  // this is the value to directly control outputs. 
      // Serial.print(index); Serial.print("  "); Serial.println(HoldingRegs[index]);    
     }  // write to register if(str[0] == 87)

//  *******************************************************************
    // this section not tested yet
    if(str[0] == 82)  // "R" read value from register
    {
      // first find the index data format is Windex:value
      i = 0; j = 0;
      
      while(isdigit(str[++i]))
      {
         tempchar[j++] = str[i];
      }
      tempchar[j] = '\0';    // gota finish string
      index = atoi(tempchar);  // now have valid HoldingReg index   
      
      if(index  < HOLDING_REGS_SIZE)
          Write_to_HMI(index);  // This writes back to the HMI contents in HoldingRegs[index]
      //else  do nothing
      // return an error,
    }  //if(str[0] == 82) 
  
  Serial.print(index); Serial.print("  "); Serial.println(HoldingRegs[index]);    
    }  // if Serial.available.
   
} // ROV_communications()