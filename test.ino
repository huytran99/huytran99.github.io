#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include "html.h"
#include "data.h"
const char *ssid = "SMART SW";
const char *password = "123456789"; 
ESP8266WebServer sv(80);//Khoi tao server ở port 80
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
  bool testWiFi(void)
  {
    int c = 0;
    Serial.println("Chờ kết nối");
    while (c < 20){
      if (WiFi.status() == WL_CONNECTED)
      {
        return true;
      }
      digitalWrite(LED_BUILTIN,LOW);
      delay(250);
      digitalWrite(LED_BUILTIN,HIGH);
      delay(250);
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
  pinMode(16, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);//Set up a soft access point

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
    sv.on("/",ketnoithanhcong);
    //Bắt đầu truyền file html lên esp
    sv.on("/index",[]{
      sv.send(200,"text/html",readData("index.html"));
    });
    sv.on("/bat.html",[]{
      digitalWrite(16,HIGH);
      sv.send(200,"text/html","");
      });
    sv.on("/tat.html",[]{
      digitalWrite(16,LOW);
      sv.send(200,"text/html","");
      });
    sv.on("/transFile",HTTP_ANY,[]{
      sv.send(200,"text/html",
    "<meta charset='utf-8'>"
    "<form method='POST' action='/transFile' enctype='multipart/form-data'>"
    "<input type='file' name='Chọn file'>"
    "<input type='submit' value='Gửi file'>" 
    "</form>"  
    );
    },[]{
      HTTPUpload& file = sv.upload();
      if (file.status == UPLOAD_FILE_START)
      {
        clearData("index.html");
      }
      else if (file.status == UPLOAD_FILE_WRITE)
      {
        saveData("index.html",(const char*)file.buf,file.currentSize);
      }
    });
    //Kết thúc truyền file htm
    sv.begin();//bắt đầu khởi động server
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

void loop() {
  // put your main code here, to run repeatedly:
  sv.handleClient();
}
