//Sending messages over long distances with the MAX845 chip

char Mymessage[5] = "Hello"; //String data

void setup() {
  // Begin the Serial at 9600 Baud
  Serial1.begin(115200);
}

void loop() {
  Serial1.write(Mymessage,5); //Write the serial data
  delay(1000);
}