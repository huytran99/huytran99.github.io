#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <PubSubClient.h>
#include "html.h"

const char *ssid = "HOST_STM32F4";
const char *password = "123456789";
const char* mqtt_server = "192.168.0.103";   /// lấy ip bằng lệnh ifconfig
ESP8266WebServer sv(80); //Khoi tao server ở port 80
const uint16_t mqtt_port = 1883;


String currentLine;
String event_DHT11,data_DHT11;

int flash; 


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];


void uart_read(){
  if (Serial.available()){
    currentLine = Serial.readStringUntil('\n');
  }  
  int Index = currentLine.indexOf(',');
  if (Index > 0){
    event_DHT11 = currentLine.substring(0, Index);
    data_DHT11 = currentLine.substring(Index + 1); 
  }
}


 
void publish_data_DHT11() {
   uart_read(); 
   if (event_DHT11 == "HUMI")
   {  
      Serial.print("Do am: ");
      Serial.println(data_DHT11);
      char charBuf[data_DHT11.length()+1];
      data_DHT11.toCharArray(charBuf,data_DHT11.length()+1);
      client.publish("DHT11_HUMI",charBuf) ;
   }
  if (event_DHT11 == "TEMP")
  {
      Serial.print("Nhiet do: ");
      Serial.println(data_DHT11);
      char charBuf[data_DHT11.length()+1];
      data_DHT11.toCharArray(charBuf,data_DHT11.length()+1);
      client.publish("DHT11_TEMP",charBuf) ;
  }
}

//-----------------------------------------------------------------------------
void handleWebsite() {
  sv.send(200, "text/html", PAGE_NetworkConfiguration); //gửi dưới dạng html
}

//-----------------------------------------------------------------------------

void cai_dat() {
  String tenwf = sv.arg("tenWiFi");
  String mk = sv.arg("matkhau");
  Serial.print("ten wifi:");
  Serial.println(tenwf);
  Serial.print("mat khau:");
  Serial.println(mk);

  //.......................................
  if (tenwf.length() > 0 && mk.length() > 0) {
    Serial.println("clear EEPROM");
    // write a 0 to 96 bytes of the EEPROM
    for (int i = 0; i < 96; ++i) {
      EEPROM.write(i, 0);
    }
    delay(200);
    Serial.println("Chép tên WiFi vào EEPROM:");
    for (int i = 0; i < tenwf.length(); ++i) {
      EEPROM.write(i, tenwf[i]);
      Serial.print("viết tên: ");
      Serial.println(tenwf[i]);
    }
    Serial.println("Chép mật khẩu WiFi vào EEPROM:");
    for (int i = 0; i < mk.length(); ++i) {
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
  while (c < 20) {
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

  
  pinMode(16, OUTPUT);

  
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
  WiFi.begin(ten_wifi, mk_wifi);
  if (testWiFi())
  {
    Serial.println("Kết nối thành công!! ");
    Serial.print("Địa chỉ IP: ");
    Serial.println(WiFi.localIP());
    sv.begin();//bắt đầu khởi động server
    client.setServer(mqtt_server, mqtt_port);
    flash = 1;
    return;
  }
  else {
    Serial.println("Cấu hình lại điểm kết nối");
    WiFi.softAP(ssid , password);
    Serial.print("Địa chỉ IP của ESP: ");
    Serial.println(WiFi.softAPIP());
    flash = 0;
    sv.on("/", handleWebsite);
    sv.on("/caidat", cai_dat);
    sv.begin();//bắt đầu khởi động server
  }
}
//-----------------------------------------------------------------------------

void reconnect() {
  // Đợi tới khi kết nối
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
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
  if (flash == 0){
     sv.handleClient(); 
     digitalWrite(16, HIGH);
     delay(200);
     digitalWrite(16,LOW);
     delay(200);  
  }
  else{
  if (!client.connected()) {
    reconnect();
  }
  publish_data_DHT11();
  client.loop();
  } 
}
