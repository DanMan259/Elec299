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

  //Initialize gripper servo to upright, centre, and open grippers 
  panServo.write(panStart);
  tiltServo.write(tiltStart);
  gripServo.write(gripStart);
}

void loop() {
  delay(2000);
  interactBall();
}
