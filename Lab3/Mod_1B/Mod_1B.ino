#include <EEPROM.h>

byte left_speed, right_speed;
int lbump = 10; //10k pull up
int rbump = 9;

int delta = 5;

int E1 = 6;//left motor
int M1 = 7;
int E2 = 5;//right motor
int M2 = 4;

void setup() {  
  // setup EEPROM's (RUN ONCE)
  EEPROM.write(0,110);
  EEPROM.write(1,110);
  delay(100);
  
  Serial.begin(9600);
  pinMode(E1, OUTPUT);
  pinMode(E2, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(lbump, INPUT);
  pinMode(rbump, INPUT);
  left_speed = EEPROM.read(0);
  right_speed = EEPROM.read(1);
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
  analogWrite(E1, left_speed%255);
  analogWrite(E2, right_speed%255);
//  Serial.println("Motors: Forward");
}

void Stop(){
//  Write Motors to Stop
  Serial.println("Motors: Stopped");
  while (true){
    analogWrite(E1, 0);
    analogWrite(E2, 0);
  }
}

void Calibrate(){
  if ((digitalRead(lbump) == LOW) || (digitalRead(rbump) == LOW)){
    delay(100);
  }
  if (digitalRead(lbump) == LOW && digitalRead(rbump) == LOW){
    EEPROM.write(0, left_speed);
    EEPROM.write(1, right_speed);
    Serial.print("Left speed: ");
    Serial.println(left_speed);
    Serial.print("Right speed: ");
    Serial.println(right_speed);
    Stop();
//    delay(100);
  }else if(digitalRead(lbump) == LOW){
    right_speed += delta;
    Serial.print("Right speed: ");
    Serial.println(right_speed);
//    left_speed -= delta;
  }else if(digitalRead(rbump) == LOW){
    left_speed += delta;
    Serial.print("Left speed: ");
    Serial.println(left_speed);
//    right_speed -= delta;
  }
}



void loop() {
  // put your main code here, to run repeatedly:
  Forward();
  Calibrate();
  
}
