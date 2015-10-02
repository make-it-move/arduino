#include <Wire.h>

//int p=1;
char c;

void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop() {
  Wire.requestFrom(8, 6);    // request 6 bytes from slave device #8
//  if (p) {Serial.println("I am Due");p=0;}
  while (Wire.available()) { // slave may send less than requested
    c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  } 
  Wire.requestFrom(12, 6);
  //Serial.print("anybody ");
  while (Wire.available()) { // slave may send less than requested
    //Serial.print("here? ");
    c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }

  delay(500);
}
