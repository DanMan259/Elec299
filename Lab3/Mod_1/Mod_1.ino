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

Button rightBumper(11);
Button leftBumper(12);

void clearVar(){
    LCount=0;
    RCount=0;
}

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
    rightBumper.begin();
    leftBumper.begin();
    pinMode(M1, OUTPUT);
    pinMode(M2, OUTPUT);
    pinMode(E1, OUTPUT);
    pinMode(E2, OUTPUT);
    pinMode(EL, INPUT);
    pinMode(ER, INPUT); 
    Serial.begin(9600);
}

void loop() {
    if (rightBumper.isReleased() && leftBumper.isReleased()){
      while (LeftCountEncoder() <= rotationTicks*3){
        Serial.print("Back\n");
        digitalWrite(M1, HIGH);
        digitalWrite(M2, HIGH);
        analogWrite(E1, 200);
        analogWrite(E2, 200);
      }
      clearVar();
      while (LeftCountEncoder() <= rotationTicks*2){
        Serial.print("180 Spin\n");
        digitalWrite(M1, HIGH);
        digitalWrite(M2, LOW);
        analogWrite(E1, 175);
        analogWrite(E2, 175);
      }
      clearVar();
    }else if (leftBumper.isReleased()){
      while (LeftCountEncoder() <= rotationTicks*3){
        Serial.print("Back\n");
        digitalWrite(M1, HIGH);
        digitalWrite(M2, HIGH);
        analogWrite(E1, 200);
        analogWrite(E2, 200);
      }
      clearVar();
      while (LeftCountEncoder() <= rotationTicks){
        Serial.print("90 Spin Left\n");
        digitalWrite(M1, HIGH);
        digitalWrite(M2, LOW);
        analogWrite(E1, 175);
        analogWrite(E2, 175);
      }
      clearVar();
    }else if (rightBumper.isReleased()){
      while (LeftCountEncoder() <= rotationTicks*3){
        Serial.print("Back\n");
        digitalWrite(M1, HIGH);
        digitalWrite(M2, HIGH);
        analogWrite(E1, 200);
        analogWrite(E2, 200);
      }
      clearVar();
      while (LeftCountEncoder() <= rotationTicks){
        Serial.print("90 Spin Right\n");
        digitalWrite(M1, LOW);
        digitalWrite(M2, HIGH);
        analogWrite(E1, 175);
        analogWrite(E2, 175);
      }
      clearVar();
    }
}
