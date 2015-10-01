#include <Servo.h>
#define Stop_Value 94

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

bool pos;    // variable to store the fact tat we need to rotate
unsigned long Turn_Time;  //turn time =)
int Turn_Speed=180; //turn speed, 180 is maximum

void setup() {
  myservo.attach(52);  // attaches the servo on pin 42 to the servo object
  Serial.begin(9600);
}

void loop() {
  myservo.write(Stop_Value);
  if (Serial.available() > 0) {
    Turn_Time = Serial.parseInt()*1000; //get rotation time in seconds
    //Serial.println(Turn_Time);
    //pos=0;
    //if (!pos) {
      myservo.write(Turn_Speed); //rotate counter-clockwise with max speed == 180
      delay(Turn_Time); //rotate for Turn_Time miliseconds
      myservo.write(Stop_Value); //stop
    //  pos=1;
    //}
  }
}

