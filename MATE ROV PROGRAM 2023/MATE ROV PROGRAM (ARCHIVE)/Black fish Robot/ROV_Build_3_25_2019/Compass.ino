//  ROV_Build_3_25_2019

#ifndef BUILD_2019   // take this out of the compiled code for now.
/*************  compass.c   ****************************************************
LSM9DS1_Basic_I2C.ino
SFE_LSM9DS1 Library Simple Example Code - I2C Interface
Jim Lindblom @ SparkFun Electronics
Original Creation Date: April 30, 2015
https://github.com/sparkfun/LSM9DS1_Breakout

The LSM9DS1 is a versatile 9DOF sensor. It has a built-in
accelerometer, gyroscope, and magnetometer. Very cool! Plus it
functions over either SPI or I2C.

This Arduino sketch is a demo of the simple side of the
SFE_LSM9DS1 library. It'll demo the following:
* How to create a LSM9DS1 object, using a constructor (global
  variables section).
* How to use the begin() function of the LSM9DS1 class.
* How to read the gyroscope, accelerometer, and magnetometer
  using the readGryo(), readAccel(), readMag() functions and 
  the gx, gy, gz, ax, ay, az, mx, my, and mz variables.
* How to calculate actual acceleration, rotation speed, 
  magnetic field strength using the calcAccel(), calcGyro() 
  and calcMag() functions.
* How to use the data from the LSM9DS1 to calculate 
  orientation and heading.

Hardware setup: This library supports communicating with the
LSM9DS1 over either I2C or SPI. This example demonstrates how
to use I2C. The pin-out is as follows:
  LSM9DS1 --------- Arduino
   SCL ---------- SCL (A5 on older 'Duinos')
   SDA ---------- SDA (A4 on older 'Duinos')
   VDD ------------- 3.3V
   GND ------------- GND
(CSG, CSXM, SDOG, and SDOXM should all be pulled high. 
Jumpers on the breakout board will do this for you.)

The LSM9DS1 has a maxCompassm voltage of 3.6V. Make sure you power it
off the 3.3V rail! I2C pins are open-drain, so you'll be 
(mostly) safe connecting the LSM9DS1's SCL and SDA pins 
directly to the Arduino.

Development environment specifics:
  IDE: Arduino 1.6.3
  Hardware Platform: SparkFun Redboard
  LSM9DS1 Breakout Version: 1.0

This code is beerware. If you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, 
please buy us a round!

Distributed as-is; no warranty is given.
******************************************************]**
HoldingReg[COMPASS] =  
HoldingReg[TIP] = 
HoldingReg[TILT] = 
*********/


/*******************************
 * printAttitude  resides in ROV
 * updates the HoldingReg[COMPASS], HoldingReg[TIP], HoldingReg[TILT] 
 * lots of fp math takes about one ms to calcuate the mess.
 * remove pitch and roll if pressed for time.
 */

#define LSM9DS1_TEMP_BIAS 27.5
 
void printAttitude(float ax, float ay, float az, float mx, float my, float mz)
{
  float roll = atan2(ay, az);
  float pitch = atan2(-ax, sqrt(ay * ay + az * az));
  
  float heading;
  if (my == 0)
    heading = (mx < 0) ? PI : 0;
  else
    heading = atan2(mx, my);
    
  heading -= DECLINATION * PI / 180;
  
  if (heading > PI) heading -= (2 * PI);
  else if (heading < -PI) heading += (2 * PI);
  else if (heading < 0) heading += 2 * PI;
  
  // Convert everything from radians to degrees:
  heading *= 180.0 / PI;
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;

  // remeber to subtract 200  at the HMI 
  HoldingRegs[TIP] = (unsigned) pitch + 200;
  HoldingRegs[TILT] = (unsigned) roll + 200;
  HoldingRegs[COMPASS] = (unsigned) heading;
  
}

/*******************************
 * Init_compass  resides in ROV
 * sets the HoldingReg[COMPASS], HoldingReg[TIP], HoldingReg[TILT] = 0
 * if unable to initilize then set Nav_working to false.
 * 
 */
void Init_compass(void)
{
  // Before initializing the IMU, there are a few settings
  // we may need to adjust. Use the settings struct to set
  // the device's communication mode and addresses:
  //lastPrint = millis(); // Update lastPrint time
      
  Compass.settings.device.commInterface = IMU_MODE_I2C;
  Compass.settings.device.mAddress = LSM9DS1_M;
  Compass.settings.device.agAddress = LSM9DS1_AG;
  
  // The above lines will only take effect AFTER calling
  // Compass.begin(), which verifies communication with the IMU
  // and turns it on.


  
  delay(500);   // we can use delay here because nothing else is going on.
  if (!Compass.begin())
  {
    Nav_working = false;    // Could not initilize compass module
    // an idea todo make diagnostic light if nav is working.
 
    HoldingRegs[COMPASS] =
    HoldingRegs[TILT] = 
    HoldingRegs[TIP] = 0;
    
    delay(2000);
       
  }  // !Compass.begin()
  else
    Nav_working = true;

    
}

/*******************************************
 * Update_compass  resides in ROV
 * 
 * updates the compass, tip and tilt
 * not called if  Nav_working = false;  Could not initilize compass module
 * if Nav_working == false then compass, etc returns zero and all done
 * 
 */
void Update_compass() // resides in ROV
 {
 // Update the sensor values whenever new data is available

/*
  if ( Compass.gyroAvailable() )
  {
    // To read from the gyroscope,  first call the
    // readGyro() function. When it exits, it'll update the
    // gx, gy, and gz variables with the most current data.
    Compass.readGyro();
  }
*/
    
  if(Compass.tempAvailable())
  {
    Compass.readTemp();
    HoldingRegs[TEMPERATURE] = (unsigned) conv_temp(Compass.temperature);
    
  }
    
  if ( Compass.accelAvailable() )
  {
    // To read from the accelerometer, first call the
    // readAccel() function. When it exits, it'll update the
    // ax, ay, and az variables with the most current data.
    Compass.readAccel();
  }
  
  if ( Compass.magAvailable() )
  {
    // To read from the magnetometer, first call the
    // readMag() function. When it exits, it'll update the
    // mx, my, and mz variables with the most current data.
    Compass.readMag();
  }
  
  if ((lastPrint + PRINT_SPEED) < millis())
  {
    #ifndef ROV_DEV
      //printGyro();  // Print "G: gx, gy, gz"
     // printAccel(); // Print "A: ax, ay, az"
      //printMag();   // Print "M: mx, my, mz"
    #endif
  // Print the heading and orientation for fun!
    // Call print attitude. The LSM9DS1's mag x and y
    // axes are opposite to the accelerometer, so my, mx are
    // substituted for each other.

    // printAttitude updates Holding Registers
    printAttitude(Compass.ax, Compass.ay, Compass.az, 
                 -Compass.my, -Compass.mx, Compass.mz);
       
    lastPrint = millis(); // Update lastPrint time
  }
     Compass.readTemp();

 }

// this took a bit of effort 
static int conv_temp(int16_t raw_temp)
{
  //return raw_temp / LSM9DS1_TEMP_SCALE + LSM9DS1_TEMP_BIAS;
  return ((int) raw_temp/256. + LSM9DS1_TEMP_BIAS); 
} 
 
void PrintCompass(void)
{
delay(500);
 Serial.print(HoldingRegs[COMPASS]);
 Serial.print(HoldingRegs[TIP]);
 Serial.print(HoldingRegs[TILT]);
 Serial.print(HoldingRegs[TEMPERATURE]);
  
}

#endif BUILD_2019  // we are removing all compass references

