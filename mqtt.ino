#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// thay đổi thông tin bên dưới cho phù hợp
const char* ssid =         "STEK"; 
const char* password =    "ptn209b3@";
const char* mqtt_server = "192.168.1.90";   /// lấy ip bằng lệnh ifconfig
const uint16_t mqtt_port = 1883;
const int ledPin1 = 16;
const int ledPin2 = 5;
const int buttonPin1 = 14;
const int buttonPin2 = 12;
const int buttonReset = 13; 

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

int led1State,led2State;
int button1State = HIGH;
int button2State = HIGH;
void publishstate(){
  if (led1State == 0){
    client.publish("State1","0") ;
  }
  else client.publish("State1","1");

  if (led2State == 0){
    client.publish("State2","0");
  }
  else client.publish("State2","1");
}



void Button1(){
  if(digitalRead(buttonPin1) == LOW){
    delay(10);
    if(digitalRead(buttonPin1) == LOW){
    if(button1State != LOW){  
    //Toggle LED state
    led1State = !led1State; 
    }
    button1State = LOW;
  }
  }
  else{
    button1State = HIGH;
  } 
}
void Button2(){
   if(digitalRead(buttonPin2) == LOW){
    delay(10);  
    if(digitalRead(buttonPin2) == LOW){
    if(button2State != LOW){  
    //Toggle LED state
    led2State = !led2State;          
    }
    button2State = LOW;
    }
    }
    else{
    button2State = HIGH;
    }  
}

void checkbuttonReset(){
  if(digitalRead(buttonReset) == LOW){
    delay(10);
    if(digitalRead(buttonReset) == LOW){
    if(buttonReset != LOW){
      ESP.restart();
    }
  }
  }
}

void updateledstate1(){
  Button1();
  digitalWrite(ledPin1,led1State); 
}
void updateledstate2(){
  Button2();
  digitalWrite(ledPin2,led2State);
}

void setup() {
  pinMode(4, OUTPUT);//led red 1
  pinMode(0, OUTPUT);//led blue 1
  pinMode(2, OUTPUT);//led red 2
  pinMode(15, OUTPUT);//led blue 2
  
  pinMode(ledPin1, OUTPUT);     // Khởi tạo LED
  pinMode(ledPin2, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonReset, INPUT_PULLUP);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // Kết nối wifi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
 
  // Bật LED1 nếu nhận được lệnh là 1
  
  if (!strcmp(topic,"LED1")){
    if ((char)payload[0] == '1') {
      Serial.println("ON");
      led1State = 1;    
      digitalWrite(ledPin1, led1State); // LED có sẵn trên NodeMCU bật ở mức 0(LOW)
  } 
  // Tắt LED1 nếu nhận được lệnh là 0
    if ((char)payload[0] == '0') {
      Serial.println("OFF");
      led1State = 0;
      digitalWrite(ledPin1, led1State); // LED tắt ở mức 1(HIGH)
      }    
  }
  // Bật LED2 nếu nhận được lệnh là 1

  if (!strcmp(topic,"LED2")){
    if ((char)payload[0] == '1') {
      Serial.println("ON");
      led2State = 1;    
      digitalWrite(ledPin2, led2State); // LED có sẵn trên NodeMCU bật ở mức 0(LOW)
  } 
  // Tắt LED2 nếu nhận được lệnh là 0
    if ((char)payload[0] == '0') {
      Serial.println("OFF");
      led2State = 0;
      digitalWrite(ledPin2, led2State); // LED tắt ở mức 1(HIGH)
  }
  }
}

void reconnect() {
  // Đợi tới khi kết nối
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
// khi ket noi thanh cong thi subscribe trang thai  
      client.subscribe("LED1");
      client.subscribe("LED2");
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop(){
  updateledstate1();
  updateledstate2();
  checkbuttonReset();
  digitalWrite(4, led1State);
  digitalWrite(0, !led1State);
  digitalWrite(2,led2State);
  digitalWrite(15,!led2State);
  if (!client.connected()) {
    reconnect();
  }
  publishstate();
  client.loop(); 
}
