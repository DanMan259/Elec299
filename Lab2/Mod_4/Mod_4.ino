#include "QSerial.h"

QSerial IRR;

int output = 0;
int rx=11;
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
    Serial.println(char(IRR.receive(200)));
  }
}
