#include <EEPROM.h>

int E1 = 6;
int M1 = 7;
int E2 = 5;
int M2 = 4;
int EL = 11;
int ER = 12;
int LCount = 0;
int RCount = 0;
int LState = 0;
int RState = 0;
int rotationTicks = 7;
int state;
int leftSpeed = 0;
int rightSpeed = 0;


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
};

Button rightButton(9);
Button leftButton(10);

int LeftCountEncoder(){
  if(digitalRead(EL) == 1){
    LState = 1;
    delay(10);
  }
  if (digitalRead(EL) != LState){
    LCount++;
    LState = 0;
  }
  return LCount;
}

int RightCountEncoder(){
  if(digitalRead(ER) == 1){
    RState = 1;
    delay(10);
  }
  if (digitalRead(ER) != RState){
    RCount++;
    RState = 0;
  }
  Serial.println(RCount);
  return RCount;
}

void AdjustSpeeds(){
  if ((rightButton.isReleased()) || (leftButton.isReleased()))    //both bumpers pressed
    {
      delay(500);
    }
  if (leftButton.isReleased()){                                   //only left bumper pressed
      leftSpeed = 160;
      rightSpeed = 240;
      while (LeftCountEncoder() <= rotationTicks*3){
        Serial.print("left curve\n");
        digitalWrite(M1, HIGH);           //right motor forward
        digitalWrite(M2, HIGH);           //left motoro forward
        analogWrite(E1, rightSpeed);      //right motor 240 speed
        analogWrite(E2, leftSpeed);       //left motor 160 speed
         }
    }
    else if (rightButton.isReleased()){                            //only right bumper pressed
      leftSpeed = 240;
      rightSpeed = 160;
      while (RightCountEncoder() <= rotationTicks*3){
        Serial.print("right curve\n");
        digitalWrite(M1, HIGH);           //right motor forward
        digitalWrite(M2, HIGH);           //left motoro forward
        analogWrite(E1, rightSpeed);      //right motor 160 speed
        analogWrite(E2, leftSpeed);       //left motor 240 speed
         }
    }
    else if ((leftButton.isReleased()) || (leftButton.isReleased())) //both bumpers pressed
      {
        EEPROM.write(0, leftSpeed);
        EEPROM.write(1, rightSpeed);
        leftSpeed = 0;
        rightSpeed = 0;
        delay(100);
      }
}

void Forward(){
    digitalWrite(M1, HIGH);
    digitalWrite(M2, HIGH);
    analogWrite(E1, 100);
    analogWrite(E2, 100);
}

void clearVar(){
    LCount=0;
    RCount=0;
    analogWrite(E1, 0);
    analogWrite(E2, 0);
    delay(1);
}

void setup() {
    rightButton.begin();
    leftButton.begin();
    pinMode(M1, OUTPUT);
    pinMode(M2, OUTPUT);
    pinMode(E1, OUTPUT);
    pinMode(E2, OUTPUT);
    pinMode(EL, INPUT);
    pinMode(ER, INPUT); 
    Serial.begin(9600);
}

void loop() {
    clearVar();
    if ((rightButton.isReleased()) && (leftButton.isReleased())){
      Serial.println("Both Clicked");
        while(true){
          Forward();
          AdjustSpeeds();
          if ((rightButton.isReleased()) && (leftButton.isReleased())){
  
              clearVar();
              Serial.print("Back\n");
              while (LeftCountEncoder() <= rotationTicks){
                digitalWrite(M1, LOW);
                digitalWrite(M2, LOW);
                analogWrite(E1, 100);
                analogWrite(E2, 100);
              }
              clearVar();
              Serial.print("180 Spin\n");
              while (LeftCountEncoder() <= rotationTicks*2){
                digitalWrite(M1, HIGH);
                digitalWrite(M2, LOW);
                analogWrite(E1, 100);
                analogWrite(E2, 100);
              }
              clearVar();
            
            }
            else if (leftButton.isReleased()){
            
              clearVar();
              Serial.print("Back\n");
              while (LeftCountEncoder() <= rotationTicks){
                digitalWrite(M1, LOW);
                digitalWrite(M2, LOW);
                analogWrite(E1, 100);
                analogWrite(E2, 100);
              }
              clearVar();
              Serial.print("90 Spin Left\n");
              while (LeftCountEncoder() <= rotationTicks){
                digitalWrite(M1, HIGH);
                digitalWrite(M2, LOW);
                analogWrite(E1, 100);
                analogWrite(E2, 100);
              }
              clearVar();          
            }
            else if (rightButton.isReleased()){
            
              clearVar();
              Serial.print("Back\n");
              while (LeftCountEncoder() <= rotationTicks){
                digitalWrite(M1, LOW);
                digitalWrite(M2, LOW);
                analogWrite(E1, 100);
                analogWrite(E2, 100);
              }
              clearVar();
              Serial.print("90 Spin Right\n");
              while (LeftCountEncoder() <= rotationTicks){
                digitalWrite(M1, LOW);
                digitalWrite(M2, HIGH);
                analogWrite(E1, 100);
                analogWrite(E2, 100);
              }
              clearVar();
            
          } 
        }
    }
}
