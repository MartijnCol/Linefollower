int ledPin = 12;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); // Start de seriële communicatie met de computer
  Serial1.begin(9600); // Start de hardwarematige seriële communicatie met de HC-06
  Serial.println("Arduino is gereed voor Bluetooth-communicatie.");
}

void loop() {
  if (Serial1.available()) {
    char receivedChar = Serial1.read();
    Serial.print("Ontvangen: ");
    Serial.println(receivedChar);

    // Voer actie uit op basis van het ontvangen commando
    if (receivedChar == '1') {
      digitalWrite(ledPin, HIGH); // Schakel LED in
    } else if (receivedChar == '0') {
      digitalWrite(ledPin, LOW); // Schakel LED uit
    }
  }
  if (Serial.available()) {
    char sendChar = Serial.read();
    Serial1.print(sendChar);
    Serial.print("Verzonden: ");
    Serial.println(sendChar);
  }
}