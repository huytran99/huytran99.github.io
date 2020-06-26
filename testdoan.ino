#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include "html.h"
const char *ssid = "SMART SW";
const char *password = "123456789";
const char* mqtt_server = "192.168.1.90";   /// lấy ip bằng lệnh ifconfig
ESP8266WebServer sv(80); //Khoi tao server ở port 80
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


//-----------------------------------------------------------------------------
void handleWebsite(){
  sv.send(200,"text/html",PAGE_NetworkConfiguration);//gửi dưới dạng html
}
void ketnoithanhcong(){
  sv.send(200,"text/plain","NetworkConfiguration success");
}
//-----------------------------------------------------------------------------

void cai_dat(){
  String tenwf = sv.arg("tenWiFi");
  String mk = sv.arg("matkhau");
  Serial.print("ten wifi:");
  Serial.println(tenwf);
  Serial.print("mat khau:");
  Serial.println(mk);

 //....................................... 
  if (tenwf.length() > 0 && mk.length() >0){
    Serial.println("clear EEPROM");
    // write a 0 to 96 bytes of the EEPROM
    for (int i = 0; i < 96; ++i) {
    EEPROM.write(i, 0);
    }
    delay(200);
    Serial.println("Chép tên WiFi vào EEPROM:");
    for (int i = 0; i < tenwf.length();++i){
      EEPROM.write(i, tenwf[i]);
      Serial.print("viết tên: ");
      Serial.println(tenwf[i]); 
    }
    Serial.println("Chép mật khẩu WiFi vào EEPROM:");
    for (int i = 0; i < mk.length();++i){
      EEPROM.write(32 + i, mk[i]);
      Serial.print("viết mk: ");
      Serial.println(mk[i]); 
    }
    EEPROM.commit();
  }
  //.......................................................
    ESP.reset();
}
//.......................................................
bool testWiFi(void)
{
  int c = 0;
  Serial.println("Chờ kết nối");
  while (c < 20){
    if (WiFi.status() == WL_CONNECTED)
    {
      return true;
    }
    delay(500);
    Serial.print("*");
    c++;
  }
  Serial.println();
  Serial.println("Không thể kết nối vì quá thời gian chờ ");
  return false;  
}
//-----------------------------------------------------------------------------
void setup() {
  // put your setup code here, to run once:
  EEPROM.begin(512); //Initialasing EEPROM
  Serial.begin(115200);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);//Set up a soft access point

  pinMode(4, OUTPUT);//led red 1
  pinMode(0, OUTPUT);//led blue 1
  pinMode(2, OUTPUT);//led red 2
  pinMode(15, OUTPUT);//led blue 2
  pinMode(ledPin1, OUTPUT);     // Khởi tạo LED
  pinMode(ledPin2, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonReset, INPUT_PULLUP);

  
  //Đọc tên và mk wifi ở EEPROM
  String ten_wifi;
  String mk_wifi;
  Serial.println();
  Serial.println("Đọc tên wifi trên EEPROM");
  for (int i = 0; i < 32; ++i)
  {
    ten_wifi += char(EEPROM.read(i)); 
  }
  Serial.print("Tên wifi: ");
  Serial.println(ten_wifi);
  Serial.println();
  Serial.println("Đọc mk wifi trên EEPROM: ");
  for (int i = 32; i < 96; ++i)
  {
    mk_wifi += char(EEPROM.read(i)); 
  }
  Serial.print("Mật khẩu: ");
  Serial.println(mk_wifi);
  WiFi.begin(ten_wifi,mk_wifi);
  if (testWiFi())
  {
    Serial.println("Kết nối thành công!! ");
    Serial.print("Địa chỉ IP: ");
    Serial.println(WiFi.localIP());
 // sv.on("/",ketnoithanhcong);
    sv.begin();//bắt đầu khởi động server
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    return;
  }
  else{
    Serial.println("Cấu hình lại điểm kết nối");
    WiFi.softAP(ssid , password);
    Serial.print("Địa chỉ IP của ESP: ");
    Serial.println(WiFi.softAPIP());
    sv.on("/",handleWebsite);
    sv.on("/caidat",cai_dat);
    sv.begin();//bắt đầu khởi động server
  }
}
//-----------------------------------------------------------------------------
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

void loop() {
  // put your main code here, to run repeatedly:
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
  sv.handleClient();
}
