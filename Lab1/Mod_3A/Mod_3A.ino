#include <Servo.h>

Servo panServo, tiltServo, gripServo;

void setup() {
  Serial.begin(9600);
  panServo.attach(8);
  tiltServo.attach(9);
  gripServo.attach(10);
}

void loop() {
  int c = 0;
  while (c < 20){
    panServo.write(c+70);
    tiltServo.write(c+50);
    gripServo.write(c*4+100);
    delay(1000); 
    c++; 
  }
  
}
