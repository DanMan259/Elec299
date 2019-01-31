int data = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if(Serial.available() > 0){
      data = Serial.read();
      Serial.println(data,DEC);
  }
  //Serial.println("500");
  //Serial.println("send!");
  //delay(1000);
}
