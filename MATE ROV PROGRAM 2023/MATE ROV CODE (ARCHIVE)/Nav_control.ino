/*
 * MODUKE  Nav_control.c
 * module for build HMI_BUILD_3_25_2109
 * vfd_disply() reads input button
 * not used for 2019 contest
 * debounces the same.
 * selects the vfd element to display
 * 
 */
int Nav_display(void)  // called from loop()
{ 
  static int LastButtonState = LOW;   // the previous reading from the input pin
  static unsigned long LastDebounceTime = 0;  // the last time the output pin was toggled
  static unsigned long Display_update = millis();
  static int prev_button;
  static int selection = 0;
  int  reading;
 

  reading = digitalRead(DISPLAY_SELECT);   // read button 5 from input
  digitalWrite(LEDPIN, reading);  // Light pin 13

  if (reading != LastButtonState) //button state is changed, 
  {
        LastDebounceTime = millis();     // reset the debouncing timer & start over
        
  }
  LastButtonState = reading; // update the last state even if bad data.
   
    /* whatever the reading is at, it's been there for longer
     than the debounce delay, so take it as the actual current state
    */
  if (( LastDebounceTime + DEBOUNCEDELAY) > millis())  //DEBOUNCEDELAY = 50
  {

    // reading is good value can be high / low 1 or 0
    if (reading == 1) 
    {
      prev_button = 1;
    }
    
    if(!reading && prev_button) 
    {
      selection  = ++selection % (TEMPERATURE - COMPASS +1);  // TEMPERATURE == 10, COMPASS = 7
      switch_compass(selection + COMPASS);  // start count at enum 7 TO 10
      Display_update = millis();
      prev_button = 0;
    } 
  }
  
  // update display once per DISPLAY_DELAY about one second.
  if((Display_update + DISPLAY_DELAY) < millis() )
  {
    switch_compass(selection + COMPASS);  // selection will  be from the holding register enum
    Display_update = millis();
  }

}

/*
 * switch_compass(int vfd_display)
 * at the moment Serial is the default Serial
 * it should be Serial2 for mega
 * 
 */

void switch_compass(int vfd_display)
{
  //Serial.println(vfd_display);
   switch(vfd_display)   
   {
      case COMPASS:
          // compass
            Serial2.print('\n');
            Serial2.print("Compass = ");
            Serial2.print(HoldingRegs[vfd_display]);
            break; 
      
        
       case TIP:
       
            Serial2.print('\n');
            Serial2.print("Pitch = ");
            Serial2.print(HoldingRegs[vfd_display]- 200);
            break;
            
        case TILT:
            Serial2.print('\n');
            Serial2.print("Tilt = ");
            Serial2.print(HoldingRegs[vfd_display]-200);
             break;
            
        case TEMPERATURE:
            Serial2.print('\n');
            Serial2.print("Temerature = ");
            Serial2.print(HoldingRegs[vfd_display]);
             break;
    
         default:
            Serial2.print('\n');
            Serial2.print("Are you Lost? ");
            break;
    } 
}
