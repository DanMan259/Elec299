#include <Servo.h>

#define LEDPIN 13

int state = 0;
int potPin = 2;
int val = 0;
int E1 = 6;
int M1 = 7;

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

Button myButton(3);

void setup() {
  Serial.begin(9600);
  myButton.begin();
  pinMode(M1, OUTPUT);
}

void loop() {
    if (myButton.isReleased()) {
        state++;
        Serial.println(F("Released"));
    }
    if (state%2 == 1){
        digitalWrite(M1, HIGH);
        analogWrite(E1, 100);
    }
}
