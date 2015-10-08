#include <Servo.h>

int inSensorPin1 = 48; //sensor1 pin on Arduino. Reached after rotating counter-clockwise, speed>90.
int inSensorPin2 = 30; //sensor1 pin on Arduino. Reached after rotating clockwise, speed<90.
int inEnginePin = 52; //Engine control pin on Arduino
int stopValue = 90; //Walue when the engine should actually stop
int sensorInertia = 1; //Time in milliseconds for the sensor to respond correctly.

int lowPassFilter = 100;  //How low pass the filtar is actualy is
double passFraction = 0.9;  //Correctness ratio of the sensor

unsigned long runDuration; // duration of 1 run at maximal speed. Aquired by the call of timeCalibrate

Servo servo1;

void setup() {
  // Set two sensor pins on input.
  pinMode(inSensorPin1, INPUT);
  pinMode(inSensorPin2, INPUT);

  //Set up Serial port.
  Serial.begin(9600);

  //Set up engine.
  servo1.attach(inEnginePin);
  servo1.write(stopValue);
  
  while(digitalRead(inSensorPin1)); //Make shure switch works
  
  end1Calibrate();  //Move to the start of the platform.
  runDuration=timeCalibrate();  //Move to the other end of the platform, measure time.

  //Implementation of information transfer protocol.
  //C - calibration
  //3 digits = time in 0.1*seconds
  //So, time of movement should be between 0.1 and 100 seconds.
  Serial.print("C");
  if (runDuration<10000) {
    if(runDuration<1000) {Serial.print("0");}
    Serial.print("0");
  }
  Serial.println(runDuration/100);
  //End of information transfer protocol.
}

int num = 0;
String readString;

void loop() {
  endCheck(); //Safety check, in case the mover is running to the end.
  //Serial.print("Hello");
  
  // put your main code here, to run repeatedly:
  
  while(!Serial.available()) {
    endCheck(); //Safety check, in case the mover is running to the end.
  }
  
  // serial read section
  while (Serial.available())
  {
    endCheck(); //Safety check, in case the mover is running to the end.
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

//the mover goes maximaly left and stops. Speed is set 180 by defolt.
void end1Calibrate (){ //the mover goes maximaly left and stops.
  int sensorInput=0;
  int i;
  for (i=0; i<lowPassFilter; ++i){  //Low pass filter
    sensorInput+=digitalRead(inSensorPin1);
    delay(sensorInertia); //Inertia of the sensor
  }
  while ( sensorInput<(lowPassFilter*passFraction) ){ //Some inputs might have mistakes
    sensorInput=0; 
    servo1.write(180);
    for (i=0; i<lowPassFilter; ++i){  //Low pass filter
      //Serial.print(sensorInput);
      sensorInput+=digitalRead(inSensorPin1);
      delay(sensorInertia); //Inertia of the sensor
    }
  }
  servo1.write(stopValue);  //Stop at the starting point
}

//the mover goes maximaly left and stops. Speed should be set 180 by defolt.
void end1Calibrate (int calibrationSpeed){
  int sensorInput=0;
  for (int i=0; i<lowPassFilter; ++i){    //Low pass filter
    sensorInput+=digitalRead(inSensorPin1);
  }
  while ( sensorInput<(lowPassFilter*passFraction) ){ //Some inputs might have mistakes
    sensorInput=0; 
    servo1.write(calibrationSpeed);
    for (int i=0; i<lowPassFilter; ++i){  //Low pass filter
      //Serial.print(sensorInput);
      sensorInput+=digitalRead(inSensorPin1);
      delay(sensorInertia); //Inertia of the sensor
    }
  }
  servo1.write(stopValue);  //Stop at the starting point
}


//Run a mover untill we reach sensor 2, then stop. Speed is set 0.
//Output: duration of movement in milliseconds.
int timeCalibrate (){
  int oneRunTime=0;
  int sensorInput=0;
  int i;
  unsigned long startTime = millis();  //starting point in time.
  for (i=0; i<lowPassFilter; ++i){  //Low pass filter
    sensorInput+=digitalRead(inSensorPin2);
    delay(sensorInertia); //Inertia of the sensor
  }
  while ( sensorInput<(lowPassFilter*passFraction) ){ //Some inputs might have mistakes
    sensorInput=0; 
    servo1.write(0);
    for (i=0; i<lowPassFilter; ++i){  //Low pass filter
      //Serial.print(sensorInput);
      sensorInput+=digitalRead(inSensorPin2);
      delay(sensorInertia); //Inertia of the sensor
    }
  }
  servo1.write(stopValue);  //Stop at the starting point
  return millis()- startTime;; //finnishing (point in time) - starting (point in time) = duration
}

//Run a mover untill we reach sensor 2, then stop.
//Output: duration of movement in milliseconds. Speed schould be set 0 by defoult.
int timeCalibrate (int calibrationSpeed){
  int oneRunTime=0;
  int sensorInput=0;
  int i;
  unsigned long startTime = millis();  //starting point in time.
  for (i=0; i<lowPassFilter; ++i){  //Low pass filter
    sensorInput+=digitalRead(inSensorPin2);
    delay(sensorInertia); //Inertia of the sensor
  }
  while ( sensorInput<(lowPassFilter*passFraction) ){ //Some inputs might have mistakes
    sensorInput=0; 
    servo1.write(calibrationSpeed);
    for (i=0; i<lowPassFilter; ++i){  //Low pass filter
      //Serial.print(sensorInput);
      sensorInput+=digitalRead(inSensorPin2);
      delay(sensorInertia); //Inertia of the sensor
    }
  }
  servo1.write(stopValue);  //Stop at the starting point
  return millis() - startTime; //finnishing (point in time) - starting (point in time) = duration
}

//The mover schould stop if any of the switches activates!
void endCheck (){
  int i;
  int sensorInput1=0;
  int sensorInput2=0;
  for (i=0; i<lowPassFilter; ++i){  //Low pass filter
    sensorInput1+=digitalRead(inSensorPin1);
    sensorInput2+=digitalRead(inSensorPin1);
    delay(sensorInertia); //Inertia of the sensor
    }
  //Serial.print(sensorInput);
  if ( ( sensorInput1 >= (lowPassFilter*passFraction) )||( sensorInput2 >= (lowPassFilter*passFraction) ) ){
    servo1.write(stopValue);  //Stop!
  }
}

