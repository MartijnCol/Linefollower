const int Button = 2; 
const int ledPin = 12; 
bool Running; 

void setup() {
  analogReference(DEFAULT);
  pinMode(Button, INPUT);
  pinMode(ledPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(Button),Interrupt, RISING);
}

void loop() {
  if(Running == true){
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
  }
  else {
    digitalWrite(ledPin, HIGH);
  }  
}

void Interrupt() {
    Running =! Running; 
}