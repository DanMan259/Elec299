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
#define forwardSpeed  165
#define rotateSpeed 150

int ballCount = 0;
int cycleCount = 0;
int POS = -1;

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
  while (!(POS == 0 || POS == 1 || POS == 2)){ 
    POS = int(char(IRR.receive(100)));
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

void moveDrive(int rightPow, int leftPow, int timeDelay){
  setDrive(rightPow, leftPow);
  delay(timeDelay);
  setDrive(0, 0);
}

void followLine(int speed){
    int LF_MIN = -30;
    int left = analogRead(LLINE);
    int right = analogRead(RLINE);
    if (left > LIGHTTHRESHOLD && right < LIGHTTHRESHOLD)
        setDrive(speed, LF_MIN);   
    else if (left < LIGHTTHRESHOLD && right > LIGHTTHRESHOLD)
        setDrive(LF_MIN, speed);
    else
        setDrive(speed*0.92, speed);
}

void followLinecount(int speed, int numInter){ 
  //intialize the intersection count to zero
  int interCount, count, temp, runningSpeed;
  interCount = count = temp = 0;
  runningSpeed = speed;

  //While the interCount is less than the number of intersections that want to be passed  
   
  while(interCount < numInter){
    count = 0;
    Serial.println("Start");
    
    while (count < 4) {
      followLine(runningSpeed);
      Serial.println(String(count)+" black cycle");
      //wait until an intersection is detected based off the IR line detector 
      //Intersection can be found if all three bottom sensors (Left, Right, and Center) sense black
      if ((analogRead(LLINE)>LIGHTTHRESHOLD) && (analogRead(RLINE)>LIGHTTHRESHOLD))count++;
      else count = 0;
      delay(1);
    }
    runningSpeed = speed;
    temp = interCount;
    while (interCount < temp+1){
      //When an intersection is detected, loop until it leaves it to to then stop the robot then updated InterCount
      count = 0;
      while (count < 5) {
        followLine(runningSpeed);
        Serial.println(String(count)+" white cycle");
        if ((analogRead(LLINE) < LIGHTTHRESHOLD) || (analogRead(RLINE) < LIGHTTHRESHOLD))count++;
        else count = 0;
        delay(1);
      }
      interCount++;
      Serial.println("UPDATED COUNT");   
    }
  }
  //Stop when done intersections
  //setDrive(0,0);
  moveDrive(92, 100, 300);
} 

void rotate(int speed, bool left, int maxLinesPassed, int delayStart){
  if (left)
      setDrive(speed, -speed);
  else
      setDrive(-speed, speed);
  for (int i = 0; i < maxLinesPassed; i++) {
      delay(delayStart);
      int count = 0;
      while (count < 40) {
          if (analogRead(CLINE)>LIGHTTHRESHOLD) count++;
          else count = 0;
          delay(1);
      }
  }

  if (left)
      moveDrive (-speed * 15, speed * 15, 50);
  else
      moveDrive(speed * 15, -speed * 15, 50);
} 

bool touchWall(){ 
  //This function returns whether either of the bumpers have been pressed 
  return (rightBumper.isPressed() || leftBumper.isPressed());
}

void interactBall(){ 
  int gripValue = 0;
  int gripped = 0;
  setDrive(0,0);
  //This code can both pick up or drop off the ball 
  //First position the gripper’s tilt servo at 115-120 degrees and open the gripper 
  tiltServo.write(120);
  //delay(3000);
  gripServo.write(gripValue);
  delay(1000);
  
  //Position the tilt servo at 70 degrees then start closing the gripper until closed or ball gripped 
  tiltServo.write(70);
  delay(1000);
  //Checked whether ball is gripped using the grip sensor 
  while (!gripped){
    //Keep increasing grip servo until ball is gripped or servo completely closed
    delay(10);
    gripServo.write((++gripValue)%180);
    gripped = digitalRead(GRIPSENSOR);
    if(gripValue == 180*2) gripped = 1;
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
  //iniPosition();
  POS = 0; 

  //Initialize gripper servo to upright, centre, and open grippers 
  panServo.write(panStart);
  tiltServo.write(tiltStart);
  gripServo.write(gripStart);
}

void loop() {
  //The steps inside each of the if statements would utilize the base functions provided at the top 
  
  if (POS ==  0){ 
    // In this step the code would reflect the actions of robot at position 1 
    //increase the ball count to work towards a new path 
    ballCount++;
    
    if (ballCount == 1){ 
      //Code for first robot’s 1st path 
      followLinecount(forwardSpeed, 1);
      rotate(rotateSpeed, 1, 1, 200);
      followLinecount(forwardSpeed, 1);
      rotate(rotateSpeed, 0, 1, 200);
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);       
      followLinecount(forwardSpeed, 4);
      rotate(rotateSpeed, 1, 1, 200);
      followLinecount(forwardSpeed, 1);
      rotate(rotateSpeed, 0, 1, 200);
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);  
    } 
    
    else if (ballCount == 2){ 
      //Code for first robot’s 2nd path
      followLinecount(forwardSpeed, 1);
      rotate(rotateSpeed, 1, 1, 200);
      followLinecount(forwardSpeed, 1);
      rotate(rotateSpeed, 0, 1, 200);
      followLinecount(forwardSpeed, 4);
      rotate(rotateSpeed, 1, 1, 200);
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);       
      followLinecount(forwardSpeed, 2);
      rotate(rotateSpeed, 0, 1, 200);
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);
    } 
    else if (ballCount == 3){ 
      //Code for first robot’s 3rd path
      followLinecount(forwardSpeed, 4);  //go forward 4 intersections PROBLEMS SOMETIMES HERE
      rotate(rotateSpeed, 1, 1, 200);
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);       
      followLinecount(forwardSpeed, 2);
      rotate(rotateSpeed, 0, 1, 200);
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);
    } else if (ballCount == 4){ 
      //Code for first robot’s 4st path
      followLinecount(forwardSpeed, 3);
      rotate(rotateSpeed, 1, 1, 200);
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);   
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);       
      followLinecount(forwardSpeed, 2);
      rotate(rotateSpeed, 0, 1, 200);
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);
    } 
    else if (ballCount == 5){ 
      //Code for first robot’s 5st path
      followLinecount(forwardSpeed, 2);
      rotate(rotateSpeed, 0, 1, 200);
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);    
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);       
      followLinecount(forwardSpeed, 2);
      rotate(rotateSpeed, 1, 1, 200);
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);
    } else { 
      //Stop the first robot
      setDrive(0,0);
      delay(10000000);
    } 
  } else if (POS ==  1){
    // In this step the code would reflect the actions of robot at position 2 
    //increase the ball count to work towards a new path 
    ballCount++;
    
    if (ballCount == 1){ 
    followLinecount(forwardSpeed, 1); //goes forward 1 intersections
    rotate(rotateSpeed, 1, 1, 200);   //rotates left and then interacts with wall/ball
    cycleCount = 0;
    while(cycleCount < 40){
      followLine(forwardSpeed);
      if (touchWall()) cycleCount++;
      else cycleCount = 0;
      delay(1);
    }
    moveDrive(-92,-100, 400);
    interactBall();
    rotate(rotateSpeed, 1, 1, 200);       //rotate 180
    followLinecount(forwardSpeed, 3);     //forward 3 intersections
    rotate(rotateSpeed, 0, 1, 200);       //turns right and delivers ball
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);
    
    } else if (ballCount == 2){ 
      followLinecount(forwardSpeed, 1);   //forward 1 intersection | CHANGED THIS FROM 1
      rotate(rotateSpeed, 0, 1, 200);     //rotate right until hit wall
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);   //rotate left 
      followLinecount(forwardSpeed, 3); //move forward 3 intersections
    rotate(rotateSpeed, 1, 1, 200);      //turn left back to deliver object
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();   //deliver ball
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);

    } else if (ballCount == 3){ 
      followLinecount(forwardSpeed, 4);   //go forward for 4 intersections
      rotate(rotateSpeed, 1, 1, 200);     //turn left 90 degrees
      followLinecount(forwardSpeed, 1);   //go forward for 1 intersection
      rotate(rotateSpeed, 0, 1, 200);     //turn right 90 degrees until hits wall
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);     //turn left 180 degrees
      followLinecount(forwardSpeed, 2);   //go forward 2 intersections
      rotate(rotateSpeed, 1, 1, 200);     //turn left 90 degrees
      followLinecount(forwardSpeed, 1);   //go forward 1 intersection
      rotate(rotateSpeed, 0, 1, 200);     //turn right until it hits wall
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);

    } else if (ballCount == 4){ 
      cycleCount = 0;                   //goes straight until it hits wall
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);   
      interactBall();
      rotate(rotateSpeed, 1, 1, 200); 
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);
    
    } else if (ballCount == 5){          //POSITION 1 BALL 5
      followLinecount(forwardSpeed, 4);  // go forward for 4 intersections
      rotate(rotateSpeed, 0, 1, 200);     //turn right
      followLinecount(forwardSpeed, 1);   //go forward 1 intersection
      rotate(rotateSpeed, 1, 1, 200);     //turn left until it hits wall
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);    
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);  //rotate from wall 
      followLinecount(forwardSpeed, 2); //forward 2 intersections
      rotate(rotateSpeed, 0, 1, 200);   //turn right
      followLinecount(forwardSpeed, 1); //go forward 1 intersection
      rotate(rotateSpeed, 1, 1, 200);   //turn left until it hits wall CHANGED THIS FROM RIGHT HAND TURN
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);      
    } else { 
      //Stop the second robot 
      setDrive(0,0);
      delay(10000000);
    }
  } else {
    // In this step the code would reflect the actions of robot at position 2 
    //increase the ball count to work towards a new path 
    ballCount++;
                                    //POSITION 2
    if (ballCount == 1){ 
      followLinecount(forwardSpeed, 4);   //forward 4 intersections
      rotate(rotateSpeed, 0, 1, 200);     //turn right
      followLinecount(forwardSpeed, 1);   //forward for 1 intersection until it hits wall
      rotate(rotateSpeed, 1, 1, 200);
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);    //turn 180
      followLinecount(forwardSpeed, 1);  //go forward 1 intersection
    rotate(rotateSpeed, 0, 1, 200);       //turn right
      followLinecount(forwardSpeed, 1);   //forward 1 intersection
      rotate(rotateSpeed, 1, 1, 200);     //turn left
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);  
    
    } else if (ballCount == 2){ 
      followLinecount(forwardSpeed, 3);  //go forward 3 intersections
      rotate(rotateSpeed, 0, 1, 200);     //turn right until it hits wall
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);  //rotate 180 degrees
      followLinecount(forwardSpeed, 2); //go forward 2 intersections
      rotate(rotateSpeed, 1, 1, 200);   //turn left
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200); //rotate 180 degrees
    
    } else if (ballCount == 3){ 
      followLinecount(forwardSpeed, 4); //go forward 4 intersections
      rotate(rotateSpeed, 0, 1, 200);    //turn right until it hits wall
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);  //rotate 180
      followLinecount(forwardSpeed, 2); //go forward 2 intersections
      rotate(rotateSpeed, 1, 1, 200);   //turn left until it hits wall
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);
    
    } else if (ballCount == 4){ 
      followLinecount(forwardSpeed, 5); //go forward 5 intersections
      rotate(rotateSpeed, 0, 1, 200);   //turn right until it hits wall
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);  //rotate 180 degres
      followLinecount(forwardSpeed, 2); //go forward 2 intersections
      rotate(rotateSpeed, 1, 1, 200);   //turn left unti lit hits wall
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);
    
    } else if (ballCount == 5){ 
      followLinecount(forwardSpeed, 2);   //go forward 2 intersections
      rotate(rotateSpeed, 1, 1, 200);      //turn left until it hits wall
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      moveDrive(-92,-100, 400);
      interactBall();
      rotate(rotateSpeed, 1, 1, 200);   //rotate 180 degrees 
      followLinecount(forwardSpeed, 2); //go forward 2 intersections CHANGED THIS FROM 4
      rotate(rotateSpeed, 0, 1, 200);   //turn right until it hits wall
      cycleCount = 0;
      while(cycleCount < 40){
        followLine(forwardSpeed);
        if (touchWall()) cycleCount++;
        else cycleCount = 0;
        delay(1);
      }
      interactBall();
      moveDrive(-92,-100, 400);
      rotate(rotateSpeed, 1, 1, 200);   
      
    } else { 
      //Stop the third robot 
      setDrive(0,0);
      delay(10000000);
    } 
  
  } 
}
