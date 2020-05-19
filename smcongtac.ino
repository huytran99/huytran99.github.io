#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "CiqO_vBnT_jojIJS-DlbuVzZa4ZJenyP";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Free";
char pass[] = "123456789";

//Set your LED and physical button pins here
const int ledPin1 = 16;
const int ledPin2 = 5;
const int buttonPin1 = 14;
const int buttonPin2 = 12;
BlynkTimer timer;
void checkPhysicalButton();
  int led1State = LOW;
  int button1State = HIGH;

  int led2State = LOW;
  int button2State = HIGH;

//Every time we connect to the cloud...
  BLYNK_CONNECTED(){
  //Request the latest state from the server
  Blynk.syncVirtual(V16);
  Blynk.syncVirtual(V5);

  //Altermatively, you could override server state using:
  //Blynk.virtualWrite(V12,led1State);
  //Blynk,virtualWrite(V13,led2State);
  //Blynk,virtualWrite(V14,led3State);
  //Blynk,virtualWrite(V15,led4State);
   
}
//When App button is pushed - switch the state


BLYNK_WRITE(V16){
  led1State = param.asInt();
  digitalWrite(ledPin1, led1State);
}
BLYNK_WRITE(V5){
  led2State = param.asInt();
  digitalWrite(ledPin2, led2State);
}
BLYNK_WRITE(V10){
  led1State = param.asInt();
  digitalWrite(ledPin1, led1State);
  led2State = param.asInt();
  digitalWrite(ledPin2, led2State);
}
void checkPhysicalButton(){
  if(digitalRead(buttonPin1) == LOW){
    //btn1State is used to avoid sequential toggles
    if(button1State != LOW){
      
      //Toggle LED state
      led1State = !led1State;
      digitalWrite(ledPin1, led1State);
      
      //Update Button Widget
      Blynk.virtualWrite(V16,led1State);
          
    }
    button1State = LOW;
  }
  else{
    button1State = HIGH;
  } 

  if(digitalRead(buttonPin2) == LOW){
    //btn2State is used to avoid sequential toggles
    if(button2State != LOW){
      
      //Toggle LED state
      led2State = !led2State;
      digitalWrite(ledPin2, led2State);
      
      //Update Button Widget
      Blynk.virtualWrite(V5,led2State);
          
    }
    button2State = LOW;
  }
  else{
    button2State = HIGH;
  }
} 

  
void setup() {
  // put your setup code here, to run once:
  // Debug console
  pinMode(4, OUTPUT);//led red 1
  pinMode(0, OUTPUT);//led blue 1
  pinMode(2, OUTPUT);//led red 2
  pinMode(15, OUTPUT);//led blue 2
 
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  //You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com",8442);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100),8442);

  pinMode(ledPin1, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);
  digitalWrite(ledPin1,led1State);
  
  timer.setInterval(50L, checkPhysicalButton);
  
  pinMode(ledPin2, OUTPUT);
  pinMode(buttonPin2, INPUT_PULLUP);
  digitalWrite(ledPin2,led2State);
  
  timer.setInterval(50L, checkPhysicalButton);
  //Setup a fubction to be called every 100 ms
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(4, led1State);
  digitalWrite(0, !led1State);
  digitalWrite(2,led2State);
  digitalWrite(15,!led2State);
  Blynk.run();
  timer.run();
}
