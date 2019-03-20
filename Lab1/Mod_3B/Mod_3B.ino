#include <Servo.h>

#define GRIPSERVO 10
#define PANSERVO 8
#define TILTSERVO 9
#define GRIPSENSOR 12

int c = 0;
int gripValue = 0;
Servo panServo, tiltServo, gripServo;

void setup() {
  Serial.begin(9600);
  panServo.attach(PANSERVO);
  tiltServo.attach(TILTSERVO);
  gripServo.attach(GRIPSERVO);
  panServo.write(105);
  tiltServo.write(120);
  gripServo.write(0);
  pinMode(GRIPSERVO, INPUT);
  pinMode(3, OUTPUT);
  delay(2000);
  
}

void loop() {

  gripServo.write(c);
  if (digitalRead(GRIPSERVO) != 1 && c != 180){
    c++;
    delay(50);
  }
  if (digitalRead(GRIPSERVO) == 1 || c == 180){
    Serial.println("Gripped");
    digitalWrite(3, HIGH);
  }
  
}
