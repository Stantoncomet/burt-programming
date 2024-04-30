# Burt Programming
A place designated to sharing, collaborating on, and saving code for the Blackfish Underwater Robotics Team at the Port Townsend High School.

## Using BurtLib_2024
To use the library containing almost all of our code, you must copy the entire `BurtLib_2024` folder you downloaded into your `Arduino/libraries/` folder!! It will not work any other way to my knowledge. If you have made it work a different way, please let me know!

## Progress
**Todo**
- Fix writing to motors (write to EVERY servo)

- Implement motor limiting

- Write doc on how to use BurtLib_2024

**Doing**
- having a good time
- not gonna stop at all
- *having a GREAT time

**Finished**
- writing to motors
- Test everything I've added (It works!!!)
- Hopefully fixed controller issues
- Send correct data
- Send data over long distance using chip thing
- Get controller input to send data to 2nd Arduino using MAX485 chip
- Send data between Arduinos
- Add proper controller deadbands and tollerances
- Tollerances actually work now
- BONUS! Controller works without batteries now (hoping it fixes various connection issues)
- Get BurtLib_2024 working right
- Get controller working with library system

## Thruster Info
For power and complexity reasons, for now we are not implementing any sort of angled movement. ROV can drive straight in any of six directions on its axis (forward, back, left, right, up, and down), no translations, etc.
- [Servo Library](https://www.arduino.cc/reference/en/libraries/servo/)
- [Arduino x ESC](https://bluerobotics.com/learn/controlling-basic-esc-with-the-arduino-serial-monitor/)
- [T100/200 Details](https://bluerobotics.com/store/thrusters/t100-t200-thrusters/t200-thruster-r2-rp/#tab-technical-details)

## Spec Sheets
- [Arduino Uno](https://docs.arduino.cc/resources/datasheets/A000066-datasheet.pdf)

## Libraries
**Using**
- [Radiohead Wireless Tranciever](https://github.com/adafruit/RadioHead)
- [USB Host Shield 2.0](https://github.com/felis/USB_Host_Shield_2.0)
  - Supports xbox, play station

**Not Using**
- [Sparkfun Wireless Tranciever](https://github.com/sparkfun/RFM69HCW_Breakout)
- [Tinker Controller](https://github.com/nathanRamaNoodles/Tinker-Controller)

## Code Formatting Guide
To keep things at least semi-organized, please follow these standards when writing any code that will end up a part of Burt. If you have any suggestions, feel free to bring it up and we'll talk about it!

Here are the types of naming conventions used in programming:
- snake_case
- Fomral_Snake_Case
- UPPER_CASE_SNAKE_CASE
- camelCase
- PascalCase
- kebab-case

Notice how each word is seperated, either with an underscore, hyphen, or capital letter. If you're name is really long or commonly shortened, like Human Machine Interface, or Remotely Operated Vehicle, you can use an acronym. Try to follow these cases, unless you have a good reason not to.

#### Naming Files
Use UPPER_CASE_SNAKE_CASE when naming `.ino` files.

Use camelCase when naming `.cpp`, `.h`, or any types of files.

#### Naming Variables and Functions
Normal variables are named with snake_case.

Global variables, such as those defined in headers, are named with Formal_Snake_Case

`#define`s or constant variables are named with UPPER_CASE_SNAKE_CASE.

`struct`s are named with PascalCase.

Functions follow the camelCase convention. Also please put the open bracket on the same line as the function definition.

#### Writing Comments
Please write comments, but write them cleanly if you can! Here's an example.
```cpp
/*
 * Returns the average of two integers.
*/
double findMean(int val1, int val2) {
  double mean = (val1 + val2) / 2; // Calculate sum of the two given numbers, divided by the number of given numbers.
  return mean;
}
```

#### Misc
Some other things to note.
- Always try to write numbers in their numerical form, rather than their word form.
- When commenting on a chunk of code, put the comment *above* the code. When commenting on just a line, like setting a variable, you can write the comment in-line. Or do whatever feels right, as long as it's clear.

## Misc
- https://projecthub.arduino.cc/Arduino_Scuola/joystick-controlled-rc-car-338a83 (???)
