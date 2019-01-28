#include <Servo.h>

int GS = 13;
int c = 0;
int gripValue = 0;
Servo panServo, tiltServo, gripServo;

void setup() {
  Serial.begin(9600);
  panServo.attach(8);
  tiltServo.attach(9);
  gripServo.attach(10);
  panServo.write(105);
  tiltServo.write(120);
  gripServo.write(0);
  pinMode(GS, INPUT);
  pinMode(3, OUTPUT);
  delay(2000);
  
}

void loop() {

  gripServo.write(c);
  if (digitalRead(GS) != 1){
    c++;
    delay(50);
  }
  if (digitalRead(GS) == 1){
    Serial.println("Gripped");
    digitalWrite(3, HIGH);
  }
  
}
