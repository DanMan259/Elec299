//Include all the correct libraries 
#include <Servo.h>
#include "QSerial.h"

// Define constants for pins and other variables/objects 
#define GRIPSERVO 10
#define PANSERVO 8
#define TILTSERVO 9
#define GRIPSENSOR 12
#define M1 7
#define E1 6
#define M2 4
#define E2 5
#define RBUMPER 3
#define LBUMPER 2
#define RLINE A1
#define CLINE A2
#define LLINE A0
#define IRRRX 11
#define panStart 105
#define tiltStart 180
#define gripStart 0
#define LIGHTTHRESHOLD 600
#define LEFTLOW 150
#define RIGHTLOW 150

int ballCount = 0;
int POS = 0;

class Button {
    private:
        bool _state;
        uint8_t _pin;

    public:
        Button(uint8_t pin) : _pin(pin) {}

        void begin() {
            pinMode(_pin, INPUT_PULLUP);
            _state = digitalRead(_pin);
        }

        bool isReleased() {
            bool v = digitalRead(_pin);
            if (v != _state) {
                _state = v;
                if (_state) {
                    return true;
                }
            }
            return false;
        }
        bool isPressed(){
          return !digitalRead(_pin);
        }
};

QSerial IRR;
Servo panServo, tiltServo, gripServo;
Button rightBumper(RBUMPER);
Button leftBumper(LBUMPER);

void iniPosition(){
  //This function allows for the robot to be assigned a starting position via the IR transmitter circuit 
  //Receives Code from IR Sensor 
  while (!(POS == 1 || POS == 2 || POS == 3)){ 
    POS = IRR.receive(100);
  }
}

void setDrive(int rightPow, int leftPow){
  if (rightPow >= 0){
    digitalWrite(M1, HIGH);
  }else {
    digitalWrite(M1, LOW);
    rightPow*=-1;
  }
  if (leftPow >= 0){
    digitalWrite(M2, HIGH);
  }else {
    digitalWrite(M2, LOW);
    leftPow*=-1;
  }
  analogWrite(E1, rightPow);
  analogWrite(E2, leftPow);
}

void followLine(int speed, int kP){
    //Follows the line and sets speed based off of error
    
    //Minimum speed
    int LF_MIN = 10;

    //Gets the error in the left and right
    int left_error = analogRead(LLINE) - LEFTLOW;
    int right_error = analogRead(RLINE) - RIGHTLOW;
    int error = left_error - right_error;

    //calculates the left and right speed
    int left_pow = max(speed - (error * kP / 1000), LF_MIN);
    int right_pow = max(speed + (error * kP / 1000), LF_MIN);

    //Write the value to the motor
    setDrive(right_pow,left_pow);
}

void followLinecount(int speed, int numInter){ 
  //intialize the intersection count to zero
  int interCount, LVal, RVal;
  LVal = RVal = interCount = 0; 

  //While the interCount is less than the number of intersections that want to be passed  

  while(interCount < numInter){ 
    followLine(speed, 80); 
    //wait until an intersection is detected based off the IR line detector 
    //Intersection can be found if all three bottom sensors (Left, Right, and Center) sense black
    LVal = analogRead(LLINE);
    RVal = analogRead(RLINE);
    
    if ((LVal > LIGHTTHRESHOLD) && (RVal > LIGHTTHRESHOLD)){
      //update InterCount when an intersection is detected
      delay(100); 
      interCount++;
    } 
  }
  //Stop when done intersections
  delay(200);
  setDrive(0,0);
}  

void rotate(int speed, bool left, int numberOfLinesPassed, int delayStart){
  if (left){ 
    //if left is true
    setDrive(speed, speed*-1);
    //Set left motor to –1*speed and right motor to speed 
  } else { 
    //if left is false, which means right
    setDrive(speed*-1, speed); 
    //Set left motor to speed and right motor to –1* speed 
  }
  
  for (int i = 0; i<= numberOfLinesPassed; i++){ 
    //Delay to give the machine a headstart so it won't detect the same line
    delay(delayStart);
    while(!(analogRead(CLINE) >= LIGHTTHRESHOLD)){
      Serial.println("Rotating");
    }
  }
  setDrive(0, 0);
  delay(500);
} 

bool touchWall(){ 
  //This function returns whether either of the bumpers have been pressed 
  return (rightBumper.isPressed() || leftBumper.isPressed());
}

void interactBall(){ 
  int gripValue = 0;
  int gripped = 0;
  //This code can both pick up or drop off the ball 
  //First position the gripper’s tilt servo at 115-120 degrees and open the gripper 
  tiltServo.write(120);
  delay(3000);
  gripServo.write(gripValue);
  delay(3000);
  
  //Position the tilt servo at 70 degrees then start closing the gripper until closed or ball gripped 
  tiltServo.write(90);
  delay(1000);
  //Checked whether ball is gripped using the grip sensor 
  while (!gripped){
    //Keep increasing grip servo until ball is gripped or servo completely closed
    delay(5);
    gripServo.write(++gripValue);
    gripped = digitalRead(GRIPSENSOR);
    if (gripValue == 180){
      gripped = 1;  
    }
  }
  Serial.println("Gripped or Closed");
  delay(1000);
  
  //Position the tilt servo completely up 
  tiltServo.write(tiltStart);
  delay(1000);
} 

void setup() {
  //Standard Initialization steps 
  Serial.begin(9600);

  //Initialize all the pins for the motors and sensors
  gripServo.attach(GRIPSERVO);
  panServo.attach(PANSERVO);
  tiltServo.attach(TILTSERVO);
  pinMode(GRIPSENSOR, INPUT);
  pinMode(M1, OUTPUT);
  pinMode(E1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(E2, OUTPUT);
  rightBumper.begin();
  leftBumper.begin();
  pinMode(RLINE, INPUT);
  pinMode(CLINE, INPUT);
  pinMode(LLINE, INPUT);
  IRR.attach(IRRRX,-1);
  
  //Initialize the robot to the position 
  iniPosition(); 

  //Initialize gripper servo to upright, centre, and open grippers 
  panServo.write(panStart);
  tiltServo.write(tiltStart);
  gripServo.write(gripStart);
}

void loop() {
  //The steps inside each of the if statements would utilize the base functions provided at the top 
  
  if (POS ==  1){ 
    // In this step the code would reflect the actions of robot at position 1 
    //increase the ball count to work towards a new path 
    ballCount++;
    
    if (ballCount == 1){ 
      //Code for first robot’s 1st path 
      followLinecount(100, 1);
      rotate(50, 1, 1, 200);
      followLinecount(100, 1);
      rotate(50, 0, 1, 200);
      while(!touchWall()){
        followLine(100, 80);
      }
      interactBall();
      rotate(50, 1, 1, 200);       
      followLinecount(100, 5);
      rotate(50, 1, 1, 200);
      followLinecount(100, 1);
      rotate(50, 0, 1, 200);
      while(!touchWall()){
        followLine(100, 80);
      }
      interactBall();
      rotate(50, 1, 1, 200);  
    } else if (ballCount == 2){ 
      //Code for first robot’s 2nd path
      followLinecount(100, 1);
      rotate(50, 1, 1, 200);
      while(!touchWall()){
        followLine(100, 80);
      }  
      interactBall();
      rotate(50, 1, 1, 200);       
      followLinecount(100, 2);
      rotate(50, 0, 1, 200);
      while(!touchWall()){
        followLine(100, 80);
      }
      interactBall();
      rotate(50, 1, 1, 200);
    } else if (ballCount == 3){ 
      //Code for first robot’s 3rd path
      followLinecount(100, 4);
      rotate(50, 1, 1, 200);
      while(!touchWall()){
        followLine(100, 80);
      }  
      interactBall();
      rotate(50, 1, 1, 200);       
      followLinecount(100, 2);
      rotate(50, 0, 1, 200);
      while(!touchWall()){
        followLine(100, 80);
      }
      interactBall();
      rotate(50, 1, 1, 200);
    } else if (ballCount == 4){ 
      //Code for first robot’s 4st path
      followLinecount(100, 3);
      rotate(50, 1, 1, 200);
      while(!touchWall()){
        followLine(100, 80);
      }    
      interactBall();
      rotate(50, 1, 1, 200);       
      followLinecount(100, 2);
      rotate(50, 0, 1, 200);
      while(!touchWall()){
        followLine(100, 80);
      }
      interactBall();
      rotate(50, 1, 1, 200);
    } else if (ballCount == 5){ 
      //Code for first robot’s 5st path
      followLinecount(100, 2);
      rotate(50, 0, 1, 200);
      while(!touchWall()){
        followLine(100, 80);
      }    
      interactBall();
      rotate(50, 1, 1, 200);       
      followLinecount(100, 4);
      rotate(50, 1, 1, 200);
      while(!touchWall()){
        followLine(100, 80);
      }
      interactBall();
      rotate(50, 1, 1, 200);
    } else { 
      //Stop the first robot
      setDrive(0,0);
      
    } 
  } else if (POS ==  2){
    // In this step the code would reflect the actions of robot at position 2 
    //increase the ball count to work towards a new path 
    ballCount++;
    
    if (ballCount == 1){ 
      //Code for second robot’s 1st path 
    
    } else if (ballCount == 2){ 
      //Code for second robot’s 2nd path 
    
    } else if (ballCount == 3){ 
      //Code for second robot’s 3rd path 
    
    } else if (ballCount == 4){ 
      //Code for second robot’s 4st path 
    
    } else if (ballCount == 5){ 
      //Code for second robot’s 5st path 
      
    } else { 
      //Stop the second robot 
      
    }
  } else {
    // In this step the code would reflect the actions of robot at position 2 
    //increase the ball count to work towards a new path 
    ballCount++;
    
    if (ballCount == 1){ 
      //Code for third robot’s 1st path 
    
    } else if (ballCount == 2){ 
      //Code for third robot’s 2nd path 
    
    } else if (ballCount == 3){ 
      //Code for third robot’s 3rd path 
    
    } else if (ballCount == 4){ 
      //Code for third robot’s 4st path 
    
    } else if (ballCount == 5){ 
      //Code for third robot’s 5st path 
      
    } else { 
      //Stop the third robot 
      
    } 
  
  } 
}
