/*
  DRV8833-Dual-Motor-Driver-Module
  made on 23 Nov 2020
  by Amir Mohammad Shojaee @ Electropeak
  Home

*/

#define AIN1 5
#define AIN2 6
#define BIN1 9
#define BIN2 10

void setup() {
  Serial.begin(9600);
  
  pinMode(AIN1,OUTPUT);
  pinMode(AIN2,OUTPUT);
  pinMode(BIN1,OUTPUT);
  pinMode(BIN2,OUTPUT);

  digitalWrite(AIN1,LOW); 
  digitalWrite(AIN2,LOW);
  digitalWrite(BIN1,LOW); 
  digitalWrite(BIN2,LOW);
  
}
 
void loop() {
for (int i = 0; i <= 255; i ++){
  analogWrite(AIN1,i); 
  analogWrite(AIN2,0);
  analogWrite(BIN1,0); 
  analogWrite(BIN2,i);
  delay(10);

}

for (int i = 255; i >= 0; i--){
  analogWrite(AIN1,i); 
  analogWrite(AIN2,0);
  analogWrite(BIN1,0); 
  analogWrite(BIN2,i);
  delay(10);
}

  analogWrite(AIN1,0); 
  analogWrite(AIN2,0);
  analogWrite(BIN1,0); 
  analogWrite(BIN2,0);
  delay(1000);

for (int i = 0; i <= 255; i ++){
  analogWrite(AIN1,0); 
  analogWrite(AIN2,i);
  analogWrite(BIN1,i); 
  analogWrite(BIN2,0);
  delay(10);
}

for (int i = 255; i >=0; i--){
  analogWrite(AIN1,0); 
  analogWrite(AIN2,i);
  analogWrite(BIN1,i); 
  analogWrite(BIN2,0);
  delay(10);
}
 
}
  