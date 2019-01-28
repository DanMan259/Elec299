#include <Servo.h>

int AVGTHRESH = 55;
int L = A2;
int R = A0;
int C = A1;
int LVal = 0;
int RVal = 0;
int CVal = 0;
int AVGVal = 0;
int E1 = 6;
int M1 = 7;
int E2 = 5;
int M2 = 4;


void setup() {
  Serial.begin(9600);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(L, INPUT);
  pinMode(R, INPUT);
  pinMode(C, INPUT);

}

void loop() {
  LVal=analogRead(L);
  RVal=analogRead(R);
  CVal=analogRead(C);
  AVGVal = (LVal+RVal+CVal)/3;
  Serial.println(AVGVal);
  if (AVGVal > AVGTHRESH){
        while (AVGVal > AVGTHRESH){
          Serial.println("BLACK");
          LVal=analogRead(L);
          RVal=analogRead(R);
          CVal=analogRead(C);
          AVGVal = (LVal+RVal+CVal)/3;
          Serial.println(AVGVal);
          digitalWrite(M1, HIGH);
          analogWrite(E1, 750);
          digitalWrite(M2, HIGH);
          analogWrite(E2, 750);
        } 
  }else{
        Serial.println("WHITE");
        digitalWrite(M1, HIGH);
        digitalWrite(M2 , LOW);
        analogWrite(E1, 750);
        analogWrite(E2, 750);
  }
}
