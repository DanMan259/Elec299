//Include all the correct libraries 
#include <Servo.h>
#include "QSerial.h"

// Define constants for pins and other variables/objects 
#define GRIPSERVO 10
#define PANSERVO 9
#define TILTSERVO 8
#define GRIPSENSOR 13
#define M1 7
#define E1 6
#define M2 4
#define E2 5
#define RBUMPER 3
#define LBUMPER 2
#define RLINE A1
#define CLINE A2
#define LLINE A0
#define IRRRX 1

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
          return digitalRead(_pin);
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
/* Pseudo Coded Section

void followLine(speed){ 
  //Set both left and right motor to the current speed 
} 

void followLinecount(speed, numInter){ 

  //intialize the intersection count to zero 
  int interCount= 0 

  //While the interCount is less than the number of intersections that want to be passed  

  while(interCount < numInter){ 
    followLine(speed); 
    //wait until an intersection is detected based off the IR line detector 
    //Intersection can be found if all three bottom sensors (Left, Right, and Center) sense black 
    //The code for the detection would be here 
    //update InterCount when an intersection is detected 
    interCount++; 
  } 
} 

void rotate(Speed, left, numberOf90Rotations, delayStart){ 
  If (left){ 
    //if left is true 
    //Set left motor to –1*speed and right motor to speed 
  } else { 
    //if left is false, which means right 
    //Set left motor to speed and right motor to –1* speed 
  } 
  
  for (loop until numberOf90Rotations){ 
    //Delay to give the machine a headstart so it won't detect the same line 
    delay(delayStart) 
    //The light sensors would then be used to detect whether the robot positioned at a line 
    //The code for the detection would be here 
    
  }
} 
*/

bool touchWall(){ 
  //This function returns whether either of the bumpers have been pressed 
  return (rightBumper.isPressed() || leftBumper.isPressed());
}

void interactBall(){ 
  int gripValue = 40;
  //This code can both pick up or drop off the ball 
  //First position the gripper’s tilt servo at 115-120 degrees and open the gripper 
  
  tiltServo.write(120) 
  gripServo.write(gripValue) 
  
  //Position the tilt servo at 160 degrees then start closing the gripper until closed or ball gripped 
  tiltServo.write(160) 
  
  //Checked whether ball is gripped using the grip sensor 
  while (digitalRead(GRIPSENSOR) != 1){ 
    //Keep increasing grip servo until ball is gripped or servo completely closed
    delay(50);
    gripServo.write(++gripValue);
  } 
  
  //Position the tilt servo completely up 
  tiltServo.write(160) ;
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
  panServo.write(90);
  tiltServo.write(160);
  gripServo.write(40);
}

void loop() {
  //The steps inside each of the if statements would utilize the base functions provided at the top 
  
  if (POS ==  1){ 
    // In this step the code would reflect the actions of robot at position 1 
    //increase the ball count to work towards a new path 
    ballCount++;
    
    if (ballCount == 1){ 
      //Code for first robot’s 1st path 
    
    } else if (ballCount == 2){ 
      //Code for first robot’s 2nd path 
    
    } else if (ballCount == 3){ 
      //Code for first robot’s 3rd path 
    
    } else if (ballCount == 4){ 
      //Code for first robot’s 4st path 
    
    } else if (ballCount == 5){ 
      //Code for first robot’s 5st path 
      
    } else { 
      //Stop the first robot 
      
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
