#include <FS.h>
bool clearData(const char* filename)
{
  SPIFFS.begin();
  File f = SPIFFS.open(String("/") + filename, "w");
  if(!f)
  {
    f.close();
    return false;
  }
  else
  {
    f.close();
    return true;
  }
}
bool saveData(const char* filename, const char* content, uint16_t len)
{
  SPIFFS.begin();
  File f = SPIFFS.open(String("/") + filename, "a");
  if(!f)
  {
    f.close();
    return false;
  }
  else
  {
    f.write(content,len);
    f.close();
    return true;
  }
}
String readData(const char* filename)
{
  SPIFFS.begin();
  File f = SPIFFS.open(String("/") + filename, "r");
  String ret = f.readString();
  f.close();
  return ret;
}