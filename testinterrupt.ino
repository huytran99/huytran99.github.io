const int         interruptPin = 0; // button flash
const int         led = 2;

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);   // LED pin to Output mode
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), ISR, FALLING); //Respond to falling edges on the pin
}
 
void loop() {
    digitalWrite(led,HIGH); //LED off 
    delay(1000); 
    digitalWrite(led,LOW); //LED on 
    delay(1000);
}
 
ICACHE_RAM_ATTR void ISR () {                  // Interrupt Service Routine, come here when an interrupt occurs
   Serial.println("Interrupt Detected"); 
}
