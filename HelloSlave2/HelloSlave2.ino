#include <Wire.h>

void setup() {
  Wire.begin(12);                // join i2c bus with address #12
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  delay(100);
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write("3fold "); // respond with message of 6 bytes
  // as expected by master
}
