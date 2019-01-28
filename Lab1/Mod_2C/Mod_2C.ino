#include <Servo.h>

int count = 0;
int LCount = 0;
int RCount = 0;
int E1 = 6;
int M1 = 7;
int E2 = 5;
int M2 = 4;
int EL = 11;
int ER = 12;
int LState = 0;
int RState = 0;
int rTicks=10;


int LeftCountEncoder(){
  if(digitalRead(EL) == 1){
    LState = 1;
    delay(1);
  }
  if (digitalRead(EL) != LState){
    LCount++;
    LState = 0;
  }
  Serial.println(LCount);
  return LCount;
}

int RightCountEncoder(){
  if(digitalRead(ER) == 1){
    RState = 1;
    delay(1);
  }
  if (digitalRead(ER) != RState){
    RCount++;
    RState = 0;
  }
  Serial.println(RCount);
  return RCount;

}

void setup() {
  Serial.begin(9600);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(E1, OUTPUT);
  pinMode(E2, OUTPUT);
  pinMode(EL, INPUT);
  pinMode(ER, INPUT);
/*  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, 255);
  analogWrite(E2, 255);*/
}

void loop() {
  if (LeftCountEncoder() <= rTicks*3){
      Serial.print("Forward\n");
      digitalWrite(M1, HIGH);
      digitalWrite(M2, HIGH);
      analogWrite(E1, 200);
      analogWrite(E2, 200);
  }else if(LeftCountEncoder() > rTicks*3 && LeftCountEncoder() <= rTicks*5){
      if (LeftCountEncoder() == (rTicks*3)){
        delay(500);
      }
      Serial.print("Back\n");
      digitalWrite(M1, LOW);
      digitalWrite(M2, LOW);
      analogWrite(E1, 200);
      analogWrite(E2, 200);
  }else if(LeftCountEncoder() > rTicks*5 && LeftCountEncoder() <= 57){
      if (LeftCountEncoder() == (rTicks*5)){
        delay(500);
      }
      Serial.print("spin\n");
      digitalWrite(M1, HIGH);
      digitalWrite(M2, LOW);
      analogWrite(E1, 175);
      analogWrite(E2, 175);      
  }else{
      Serial.print("STOP\n");
      analogWrite(E1, 0);
      analogWrite(E2, 0);
  }
}
