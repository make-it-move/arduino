#include <Servo.h>

Servo servo1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  servo1.attach(52);
  servo1.write(90);
}

int num = 0;
String readString;

void loop() {
  // put your main code here, to run repeatedly:
  
  while(!Serial.available()) {}
  // serial read section
  while (Serial.available())
  {
    if (Serial.available() >0)
    {
      char c = Serial.read();  //gets one byte from serial buffer
      readString += c; //makes the string readString
    }
  }

  if (readString.length() >0)
  {
    Serial.print("Arduino received: ");  
    Serial.println(readString); //see what was received
  }

  if( readString == "L") {
    servo1.write(180);
  } else if ( readString == "R" ) {
    servo1.write(0);
  } else if ( readString == "S" ) {
    servo1.write(90);
  }

  readString = "";
}

