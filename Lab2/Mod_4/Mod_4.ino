#include "QSerial.h"

QSerial IRR;

int output = 0;
int rx=3;
int tx=2;
int tr = 0;

void setup() {
  Serial.begin(9600);
  if (tr){
    IRR.attach(-1,tx);
  }else{
    IRR.attach(rx,-1);
  }
}

void loop() {
  if (tr){
    IRR.transmit('2');  
  }else{
    output = IRR.receive(100);
    if (output != 0){
      Serial.println(char(output));
      Serial.println(output);
    }
  }
}
