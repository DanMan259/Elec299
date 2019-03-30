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
#define LIGHTTHRESHOLD 450
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

void rotate(int speed, bool left, int maxLinesPassed, int delayStart){
  if (left)
      setDrive(-speed, speed);
  else
      setDrive(speed, -speed);
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
      setDrive(speed * 15, -speed * 15);
  else
      setDrive(-speed * 15, speed * 15);
  delay(150);
  setDrive(0,0);

    
//  while(linesPasses < maxLinesPassed){
//    if (left){ 
//      //if left is true
//      //Set left motor to –1*speed and right motor to speed 
//      setDrive(speed, speed*-1);
//      //Delay to give the machine a headstart so it won't detect the same line
//      delay(delayStart);
//      if (analogRead(CLINE)>LIGHTTHRESHOLD){
//          Serial.println("over black sensor reads: " + analogRead(CLINE));
//        linesPasses++;
//        setDrive(0, 0);
//        delay(delayStart);
//        }
//    } else {
//        Serial.println(analogRead(CLINE));
//      //if left is false, which means right
//      //Set left motor to speed and right motor to –1* speed
//      setDrive(speed*-1, speed);
//      //Delay to give the machine a headstart so it won't detect the same line
//      delay(delayStart);
//      if (analogRead(CLINE)>LIGHTTHRESHOLD){
//        linesPasses++;
//        setDrive(0, 0);
//        delay(delayStart);
//      }
//    }
    
//    //Delay to give the machine a headstart so it won't detect the same line
//    delay(delayStart);
//    if (analogRead(CLINE)>LIGHTTHRESHOLD){
//      linesPasses++;
//      setDrive(0, 0);
//      delay(delayStart);
//    }
//  }

//  int count = 0;
//  int memory = 0;
//  int memory2 = 0;
//  int current = 0;  
//    while(numberOfLinesPassed>tempLines){
//        Serial.println("CLINE " + String(analogRead(CLINE)));
//        count++;
//        memory = analogRead(CLINE);
//        delay(160);
//        current = analogRead(CLINE);
//        delay(160);
//        memory2 = analogRead(CLINE);
//          
//        //Serial.println("Rotating");
//        if (memory < LIGHTTHRESHOLD && current > LIGHTTHRESHOLD && memory2 < LIGHTTHRESHOLD ){
//          tempLines++;
//  
//          Serial.println("tempLines " + String(tempLines));
//          Serial.println("CLINE " + String(analogRead(CLINE)));
//      }
//    }
//    setDrive(0, 0);
} 

void setup() {
  //Standard Initialization steps 
  Serial.begin(19200);

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
  rotate(150, 1, 1, 150);
  Serial.println("Right 180");
  delay(3000);
  rotate(150, 0, 2, 150);
  Serial.println("Left 270");
  delay(3000);
  rotate(150, 1, 3, 150);
  Serial.println("Left 90");
  delay(3000);
}
