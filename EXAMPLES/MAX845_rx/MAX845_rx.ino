//Receiving messages over long distances with the MAX845 chip

char Mymessage[10]; //Initialized variable to store recieved data
int iCallCount = 0;

void setup() {
  // Begin the Serial at 9600 Baud
  Serial1.begin(115200);
  Serial.begin(115200);
}

void loop() {
  iCallCount++;
  Serial1.readBytes(Mymessage,5); //Read the serial data and store in var
  Serial.print(iCallCount);
  Serial.print(":  ");
  Serial.println(Mymessage); //Print data on Serial Monitor
  delay(1000);
}