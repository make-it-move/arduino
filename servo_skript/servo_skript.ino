#include <Servo.h>

//In case you need a cycle that can take long time to execute, please, use endCheck1() in it.
//It chacks if the mover is runing out of permited area and, if so, stops it.
//Safety first =).

//Arduino DUI Configuration
//int inEnginePin1 = 52; //engine1 control pin on Arduino.
//int inEnginePin2 = 53; //engine1 control pin on Arduino.
//int inSensorPin1 = 48; //sensor1 pin on Arduino. Reached after rotating counter-clockwise, speed>90.
//int inSensorPin2 = 30; //sensor1 pin on Arduino. Reached after rotating clockwise, speed<90.

//Arduino UNO Configuration
int inEnginePin1 = 10; // Linear
int inEnginePin2 = 9; // Rotary

bool ifSensorsAttached = 1;  //Says if we have switches or not.
int inSensorPin1 = 6; //sensor1 pin on Arduino. Reached after rotating counter-clockwise, speed>90.
int inSensorPin2 = 7; //sensor1 pin on Arduino. Reached after rotating clockwise, speed<90.

int stopValue = 90; //Walue when the engine should actually stop
int sensorInertia = 1; //Time in milliseconds for the sensor to respond correctly.

int lowPassFilter = 10;  //How low pass the filtar is actualy is
double passFraction = 0.1;  //Correctness ratio of the sensor

unsigned long runDuration; // duration of 1 run at maximal speed. Aquired by the call of timeCalibrate

// Song-Playing
int moverPosition = 0; // after endcalibrate, all to the left (max 9)

//Declare engines.
Servo servo1;
Servo servo2;

void setup() {
  Serial.println("Starting Setup");
  // Set two sensor pins on input.
  pinMode(inSensorPin1, INPUT);
  pinMode(inSensorPin2, INPUT);

  //Set up Serial port.
  Serial.begin(9600);

  //Set up engine1. Schould correspond to linear module engine.
  servo1.attach(inEnginePin1);
  servo1.write(stopValue);

  //Set up engine2. Schould correspond to rotation module engine.
  servo2.attach(inEnginePin2);
  servo2.write(stopValue);

  if (ifSensorsAttached){
    while(digitalRead(inSensorPin1)); //Make shure sensor1 works.
    while(digitalRead(inSensorPin2)); //Make shure sensor2 works.

    /*endCalibrate1();  //Move to the start of the platform.
  
    runDuration=timeCalibrate1();  //Move to the other end of the platform, measure time.
    sendCalibrationInfoProtocol(runDuration); //Send calibration data via serial using the protocol.*/
  }
  Serial.println("Starting Completed");
}

//int num = 0;
String readString;

void loop() {
  
  endCheck1(); //Safety check, in case the mover is running to the end.

  while(!Serial.available()) {
    endCheck1(); //Safety check, in case the mover is running to the end.
  }
  
  // Serial read section

  //endCheck1(); //Safety check, in case the mover is running to the end.
  if (Serial.available() >0){
    char c = Serial.read();  //gets one byte from serial buffer
    readString += c; //makes the string readString
  }

  if (readString.length() < 4){
    checkIfStringValid(readString);
  } else {
    checkIfStringValid(readString);
    parseSerialData(readString);  //Recive orders via Serial using the protocol.
    readString = "";
  }
  
}

//Calibration block starts______________________________________________________________

//the mover goes maximaly left and stops. Speed is set 180 by default.
void endCalibrate1 (){
  endCalibrate1(180);
}

//the mover goes maximaly left and stops. Speed should be set 180 by default.
void endCalibrate1 (int calibrationSpeed){
  int sensorInput=0;
  for (int i=0; i<lowPassFilter; ++i){    //Low pass filter
    sensorInput+=digitalRead(inSensorPin1);
  }
  while ( sensorInput<(lowPassFilter*passFraction) ){ //Some inputs might have mistakes
    sensorInput=0; 
    servo1.write(calibrationSpeed);
    Serial.println(sensorInput);
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
int timeCalibrate1 (){
  timeCalibrate1(180);
}

//Run a mover untill we reach sensor 2, then stop.
//Output: duration of movement in milliseconds. Speed schould be set 0 by defoult.
int timeCalibrate1 (int calibrationSpeed){
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

//Calibration block finishes______________________________________________________________

//The mover schould stop if any of the switches activates!
//Note: uses global variable ifSensorsAttached
//Should work only if sensors are indeed there.ifSensorsAttached: 1 - works, 0 - doesn't work.
void endCheck1 (){
  if (ifSensorsAttached){
    int i;
    int sensorInput1=0;
    int sensorInput2=0;
    for (i=0; i<lowPassFilter; ++i){  //Low pass filter
      sensorInput1+=digitalRead(inSensorPin1);
      sensorInput2+=digitalRead(inSensorPin2);
      delay(sensorInertia); //Inertia of the sensor
    }
    // Left button pressed
    if ( sensorInput1 >= (lowPassFilter*passFraction) ){
      servo1.write(stopValue);
      moveAwayFromSwitch(100); // initially moving left
    }
    if ( sensorInput2 >= (lowPassFilter*passFraction) ){
      servo1.write(stopValue);
      moveAwayFromSwitch(80); // initially moving right
    }
  }
}

void checkIfStringValid(String toCheck){
  
}

//The information protocol block starts______________________________________________________________

//Gets information from according to the protocol. Executes rotation with respect to it.
//Note: uses global variable ifSensorsAttached
void parseSerialData(String serialString){
  if(serialString[0] == 'P'){
    if(serialString[1] == '1'){
      playSongOne();
    } else if ( serialString[1] == '2'){
      playSongTwo();
    }
  } else if(serialString[0] == 'R'){ //rotation
    if(serialString[1] == 'H'){
      hitOnce();
    } else {
      servo2.write(serialString.substring(1).toInt());
    }
  } else if ( serialString[0] == 'L'){  //linear movement
    if(serialString[1]=='L'){
      linearMoveLeft();
    } else if ( serialString[1]=='R'){
      linearMoveRight();
    } else {
      servo1.write(serialString.substring(1).toInt());
    }
  } else if ( serialString[0] == 'C'){  //Calibration (of linear mover).
    int calibrationSpeed = serialString.substring(1).toInt();
    if (ifSensorsAttached){
      endCalibrate1(calibrationSpeed);  //Move to the start of the platform.
      
      int timeCalibrateSpeed = 180 - calibrationSpeed; // Move in the other direction
      sendCalibrationInfoProtocol(timeCalibrate1(timeCalibrateSpeed));  //Move to the other end of the platform, measure time. Send calibration data via serial using the protocol.
      moveAwayFromSwitch(timeCalibrateSpeed);
    }
  }
  //Serial.print(serialString.substring(1).toInt());
  //Serial.println(serialString);
}

void moveAwayFromSwitch(int initialSpeed){
  if(initialSpeed > 90){ // left
    servo1.write(80); // right
  } else { // right
    servo1.write(100); //left
  }
  delay(200);
  servo1.write(90);
}

//Implementation of information transfer protocol for calibration data.
//C - calibration
//3 digits = time in 0.1*seconds
//So, time of movement should be between 0.1 and 100 seconds.
void sendCalibrationInfoProtocol(unsigned long runDuration){
  String calibrationInfo = "C";
  if (runDuration<10000) {
    if(runDuration<1000) {
      calibrationInfo += "0";
    }
    calibrationInfo += "0";
  }
  int runDurationInfo = runDuration/100;
  calibrationInfo += String(runDurationInfo);
  Serial.println(calibrationInfo);  
}

//The information protocol block finishes______________________________________________________________

/* NOT SAFE for boundaries! */
void linearMoveRight(){
  if(moverPosition<9){
    servo1.write(0);
    delay(270);
    servo1.write(90);
    moverPosition = moverPosition + 1;
  }
}

void linearMoveLeft(){
  if(moverPosition>0){
    servo1.write(180);
    delay(270);
    servo1.write(90);
    moverPosition = moverPosition - 1;
  }
}

void hitOnce(){
  servo2.write(0);
  delay(103); // dirty fix: it needs a little bit more down so every hit is a hit.
  servo2.write(180);
  delay(100);
  servo2.write(90);
}

// ** London bridge falling down
void playSongOne(){
  endCalibrate1(100);
  
  linearMoveRight();
  linearMoveRight();
  linearMoveRight();
  linearMoveRight();
  linearMoveRight();
   //5 
  hitOnce();
  linearMoveRight();
  hitOnce();
  linearMoveLeft();
  hitOnce();
  //10          
  linearMoveLeft();
  hitOnce();
  linearMoveLeft();
  hitOnce();
  linearMoveRight();
  //15
  hitOnce();
  linearMoveRight();
  hitOnce();
  linearMoveLeft();
  linearMoveLeft();
  //20   
  linearMoveLeft();
  hitOnce();
  linearMoveRight();
  hitOnce();
  linearMoveRight();
  //25
  hitOnce();          
  linearMoveLeft();
  hitOnce();
  linearMoveRight();
  hitOnce();
  //30
  linearMoveRight();   
  hitOnce();
  delay(300);
  hitOnce();
  linearMoveRight();  
  hitOnce();
  //35
  linearMoveLeft();
  hitOnce();          
  linearMoveLeft();
  hitOnce();
  linearMoveLeft();
  //40
  hitOnce();
  linearMoveRight();
  hitOnce();
  linearMoveRight();
  hitOnce();
  //45
  linearMoveLeft();
  linearMoveLeft();
  linearMoveLeft();
  hitOnce();
  linearMoveRight();
  //50
  linearMoveRight();
  linearMoveRight();  
  hitOnce();
  linearMoveLeft();
  linearMoveLeft();
  //55
  hitOnce();
  linearMoveLeft();
  linearMoveLeft();
  hitOnce();
}
//Alle meine Enten
void playSongTwo(){
  endCalibrate1(100);

  //0
  hitOnce();
  linearMoveRight();
  hitOnce();
  linearMoveRight();
  hitOnce();
  //5
  linearMoveRight();
  hitOnce();
  linearMoveRight();
  hitOnce();
  delay(300);
  //10
  hitOnce();
  linearMoveRight();
  hitOnce();
  hitOnce();
  hitOnce();
  //15
  hitOnce();
  linearMoveLeft();
  hitOnce();
  linearMoveRight();
  hitOnce();
  //20
  hitOnce();
  hitOnce();
  hitOnce();
  linearMoveLeft();
  hitOnce();
  //25
  linearMoveLeft();
  hitOnce();
  hitOnce();
  hitOnce();
  hitOnce();
  //30
  linearMoveLeft();
  hitOnce();
  delay(300);
  hitOnce();
  linearMoveRight();
  //35
  linearMoveRight();
  hitOnce();
  hitOnce();
  hitOnce();
  hitOnce();
  //40
  linearMoveLeft();
  linearMoveLeft();
  linearMoveLeft();
  linearMoveLeft();
  hitOnce();
  //45
}

