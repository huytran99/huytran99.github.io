#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "dataus.h"

ESP8266WebServer sv(80);
void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("HUY","123456789");
  WiFi.begin("Thanh","13122003");
  while(WiFi.waitForConnectResult()!= WL_CONNECTED)
    delay(100);
  Serial.begin(115200);
  Serial.println(WiFi.localIP());
  sv.on("/index",[]{
    sv.send(200,"text/html",readData("index.html"));
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
  
  sv.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  sv.handleClient();
}
