#define LEDPIN 13

int state = 0;
int potPin = 2;
int val = 0;

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
    myButton.begin();
    pinMode(LEDPIN, OUTPUT); 
    Serial.begin(115200);
}

void loop() {
    if (myButton.isReleased()) {
        state++;
        Serial.println(F("Released"));
    }            // wait for a second

    if (state%2 == 1){
        val = analogRead(potPin);
        Serial.println(val);
        digitalWrite(LEDPIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(val);               // wait for a second
        digitalWrite(LEDPIN, LOW);    // turn the LED off by making the voltage LOW
        delay(val);
    }
}
