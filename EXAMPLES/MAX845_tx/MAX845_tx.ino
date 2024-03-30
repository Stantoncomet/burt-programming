//Sending messages over long distances with the MAX845 chip

void setup() {
  Serial.begin(115200); //start serial
  
}

void loop() {
    Serial.println("hello world");
    delay(100);
}


/*
Arduino: 1.8.19 (Linux), Board: "Arduino Uno"

Sketch uses 1632 bytes (5%) of program storage space. Maximum is 32256 bytes.
Global variables use 200 bytes (9%) of dynamic memory, leaving 1848 bytes for local variables. Maximum is 2048 bytes.
avrdude: ser_open(): can't open device "/dev/ttyACM0": Permission denied
Problem uploading to board.  See https://support.arduino.cc/hc/en-us/sections/360003198300 for suggestions.


This report would have more information with
"Show verbose output during compilation"
option enabled in File -> Preferences.





*/