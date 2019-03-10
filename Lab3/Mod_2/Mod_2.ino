#include <EEPROM.h>
#include <math.h>

byte left_speed, right_speed;
int lbump = 10; //10k pull up (white and black wires)
int rbump = 9;
bool LEFT = false;
bool RIGHT = true;
int encoder = 10;
int IRrange = A1; 

int E1 = 6;//left motor
int M1 = 7;
int E2 = 5;//right motor
int M2 = 4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(E1, OUTPUT);
  pinMode(E2, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(lbump, INPUT);
  pinMode(rbump, INPUT);
  pinMode(encoder, INPUT);
  pinMode(IRrange, INPUT);
  left_speed = 110;//EEPROM.read(0);
  right_speed = 105;//EEPROM.read(1);
  Serial.print("Left speed: ");
  Serial.println(left_speed);
  Serial.print("Right speed: ");
  Serial.println(right_speed);
  WaitButton();
}

void WaitButton(){
  while (((digitalRead(lbump) == HIGH) || (digitalRead(rbump) == HIGH))){
    if (((digitalRead(lbump) == LOW) || (digitalRead(rbump) == LOW))){
      while (((digitalRead(lbump) == LOW) || (digitalRead(rbump) == LOW))){
        if (((digitalRead(lbump) == HIGH) || (digitalRead(rbump) == HIGH))){
          Serial.println("Starting...");
          break;
        }
      }
      break;
    }
  }
}

void Forward(){
//  Write Motors to Drive Forward
  digitalWrite(M1, HIGH);
  digitalWrite(M2, HIGH);
  analogWrite(E1, left_speed);
  analogWrite(E2, right_speed);
//  Serial.println("Motors: Forward");
}
void Backward(){
//  Write Motors to Drive Backwards
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  analogWrite(E1, left_speed);
  analogWrite(E2, right_speed);
//  Serial.println("Motors: Backward");
}

void Stop(){
//  Write Motors to Stop
  analogWrite(E1, 0);
  analogWrite(E2, 0);
//  Serial.println("Motors: Stopped");
}

void Pivot(bool dir, int angle){
  int count = 0;
  Stop();
  delay(100);
  if (dir == true){
      digitalWrite(M1, HIGH);
      digitalWrite(M2, LOW);
      analogWrite(E1, 100);
      analogWrite(E2, 100);
  }else{
      digitalWrite(M1, LOW);
      digitalWrite(M2, HIGH);
      analogWrite(E1, 100);
      analogWrite(E2, 100);
  }
  while (true){
    if (digitalRead(encoder) == HIGH){
      count++;
      Serial.println(count);
      delay(10); //Might help?
    }
    if (count == angle*1){
      Stop();
      break;
    }
  }
}

void adjustSpeed(float factor){
  left_speed = 105*factor;//EEPROM.read(0)*factor;
  right_speed = 110*factor;//EEPROM.read(1)*factor;
  Serial.print("Left speed: ");
  Serial.println(left_speed);
  Serial.print("Right speed: ");
  Serial.println(right_speed);
}

void ReactIRSensor(){
  int range = analogRead(IRrange); //mV
  Serial.print("voltage: ");
  Serial.println(range);
  if (range >= 500){//object infront
    Stop();
    adjustSpeed(1);
    Pivot(LEFT, 180);
  }else if(range >= 300){
    adjustSpeed(0.8);
  }else if(range >= 175){
    adjustSpeed(1);
  }else{
    adjustSpeed(1.1);
  }
}

void loop() {
  ReactIRSensor();
  Forward();
}
