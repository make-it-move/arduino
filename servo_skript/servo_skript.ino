#include <Servo.h>

int inSensorPin = 48; //sensor pin on Arduino
int inEnginePin = 52; //Engine control pin on Arduino
int stopValue=90; //Walue when the engine should actually stop

int lowPassFilter = 100;  //How low pass the filtar is actualy is
double passFraction = 0.9;  //Correctness ratio of the sensor

Servo servo1;

void setup() {
  // put your setup code here, to run once:
  pinMode(inSensorPin, INPUT);
  Serial.begin(9600);
  servo1.attach(inEnginePin);
  servo1.write(stopValue);
  
  while(digitalRead(inSensorPin)); //Make shure switch works
  
  endCalibrate();
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
    servo1.write(stopValue);
  }

  readString = "";
}

//void speedCalibrate

//the mover goes maximaly left and stops. Speed is set 180 by defolt.
void endCalibrate (){ //the mover goes maximaly left and stops.
  int sensorInput=0;
  int i;
  for (i=0; i<lowPassFilter; ++i){  //Low pass filter
    sensorInput+=digitalRead(inSensorPin);
  }
  while ( sensorInput<(lowPassFilter*passFraction) ){ //Some inouts might have mistakes
    sensorInput=0; 
    for (i=0; i<lowPassFilter; ++i){  //Low pass filter
      //Serial.print(sensorInput);
      delay(1); //Inertia of the sensor
      servo1.write(180);
      sensorInput+=digitalRead(inSensorPin);
    }
  }
  servo1.write(stopValue);  //Stop at the starting point
}

//the mover goes maximaly left and stops. Speed should be set 180 by defolt.
void endCalibrate (int calibrationSpeed){ //the mover goes maximaly left and stops.
  int sensorInput=0;
  for (int i=0; i<lowPassFilter; ++i){    //Low pass filter
    sensorInput+=digitalRead(inSensorPin);
  }
  while ( sensorInput<(lowPassFilter*passFraction) ){ //Some inouts might have mistakes
    sensorInput=0; 
    for (int i=0; i<lowPassFilter; ++i){  //Low pass filter
      //Serial.print(sensorInput);
      delay(1); //Inertia of the sensor
      servo1.write(calibrationSpeed);
      sensorInput+=digitalRead(inSensorPin);
    }
  }
  servo1.write(stopValue);  //Stop at the starting point
}
