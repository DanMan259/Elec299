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
