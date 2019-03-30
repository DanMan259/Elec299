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

bool touchWall(){ 
  //This function returns whether either of the bumpers have been pressed 
  return (rightBumper.isPressed() || leftBumper.isPressed());
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
    int right_pow = max((speed + (error * kP / 1000))*0.92, LF_MIN);

    //Write the value to the motor
    setDrive(right_pow,left_pow);
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

  //Initialize gripper servo to upright, centre, and open grippers 
  panServo.write(panStart);
  tiltServo.write(tiltStart);
  gripServo.write(gripStart);
}

void loop() {
  cycleCount = 0;
  while(cycleCount < 40){
    followLine(100, 100);
    if (touchWall()) cycleCount++;
    else cycleCount = 0;
    delay(1);
  }
  moveDrive(-92,-100, 500);
  //setDrive(200*0.92,200);
}
