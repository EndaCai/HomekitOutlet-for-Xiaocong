/*
 * @Date: 2022-04-11 00:38:00
 * @LastEditors: Enda Cai
 * @E-mail: EndaCai@qq.com
 * @LastEditTime: 2022-04-22 00:29:45
 * @FilePath: /HomekitOutlet-for-Xiaocong/src/wifiConfig.cpp
 */
#include "main.h"
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h> //导入Flash库文件

const char* AP_NAME = "Smart Plug";//softAP wifi name

char sta_ssid[32] = {0};
char sta_password[64] = {0};

String ssid;
String psw;
struct config_type
{
  char stassid[32];//定义配网得到的WIFI名长度(最大32字节)
  char stapsw[64];//定义配网得到的WIFI密码长度(最大64字节)
};

struct config_type wifi_config_rom;

void saveConfig()//保存函数
{
 EEPROM.begin(1024);//向系统申请1024kb ROM
 //开始写入
 uint8_t *p = (uint8_t*)(&wifi_config_rom);
  for (int i = 0; i < sizeof(wifi_config_rom); i++)
  {
    EEPROM.write(i, *(p + i)); //在闪存内模拟写入
  }
  EEPROM.commit();//执行写入ROM
}

void loadConfig()//读取函数
{
  EEPROM.begin(1024);
  uint8_t *p = (uint8_t*)(&wifi_config_rom);
  for (int i = 0; i < sizeof(wifi_config_rom); i++)
  {
    *(p + i) = EEPROM.read(i);
  }
  EEPROM.commit();
  ssid = wifi_config_rom.stassid;
  psw = wifi_config_rom.stapsw;
}


//web code
const char* page_html = "\
<!DOCTYPE html>\r\n\
<html lang='en'>\r\n\
<head>\r\n\
  <meta charset='UTF-8'>\r\n\
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>\r\n\
  <title>Document</title>\r\n\
</head>\r\n\
<body>\r\n\
  <form name='input' action='/' method='POST'>\r\n\
        wifi名称: <br>\r\n\
        <input type='text' name='ssid'><br>\r\n\
        wifi密码:<br>\r\n\
        <input type='text' name='password'><br>\r\n\
        <input type='submit' value='保存'>\r\n\
    </form>\r\n\
</body>\r\n\
</html>\r\n\
";

const byte DNS_PORT = 53;//DNS port
IPAddress apIP(192, 168, 4, 1);//esp8266-AP-IP address
DNSServer dnsServer;
ESP8266WebServer server(80);

void wifiConfigConnectWifi(void);

void handleRoot() {//访问主页回调函数
  server.send(200, "text/html", page_html);
}

void handleRootPost() {//Post回调函数
  Serial.println("handleRootPost");
  if (server.hasArg("ssid")) {//判断是否有账号参数
    Serial.print("got ssid:");
    strcpy(wifi_config_rom.stassid, server.arg("ssid").c_str());//将账号参数拷贝到sta_ssid中
    Serial.println(wifi_config_rom.stassid);
  } else {//没有参数
    Serial.println("error, not found ssid");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found ssid");//返回错误页面
    return;
  }
  //密码与账号同理
  if (server.hasArg("password")) {
    Serial.print("got password:");
    strcpy(wifi_config_rom.stapsw, server.arg("password").c_str());
    Serial.println(wifi_config_rom.stapsw);
  } else {
    Serial.println("error, not found password");
    server.send(200, "text/html", "<meta charset='UTF-8'>error, not found password");
    return;
  }

  server.send(200, "text/html", "<meta charset='UTF-8'>保存成功");//返回保存成功页面
  delay(2000);
  //Save wifi config
  saveConfig();//调用保存函数
  //连接wifi
  wifiConfigConnectWifi();
}

void wifiConfigInitBasic(void){//初始化基础
  Serial.begin(115200);
  WiFi.hostname("Smart-Plug");//设置ESP8266设备名
}

void wifiConfigInitSoftAP(void){//初始化AP模式
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  if(WiFi.softAP(AP_NAME)){
    Serial.println("ESP8266 SoftAP is right");
  }
}

void initWebServer(void){//初始化WebServer
  //server.on("/",handleRoot);
  //上面那行必须以下面这种格式去写否则无法强制门户
  server.on("/", HTTP_GET, handleRoot);//设置主页回调函数
  server.onNotFound(handleRoot);//设置无法响应的http请求的回调函数
  server.on("/", HTTP_POST, handleRootPost);//设置Post请求回调函数
  server.begin();//启动WebServer
  Serial.println("WebServer started!");
}

void wifiConfigInitDNS(void){//初始化DNS服务器
  if(dnsServer.start(DNS_PORT, "*", apIP)){//判断将所有地址映射到esp8266的ip上是否成功
    Serial.println("start dnsserver success.");
  }
  else Serial.println("start dnsserver failed.");
}

void wifiConfigConnectWifi(void){
  if(!WiFi.mode(WIFI_STA)) {//切换为STA模式
    Serial.println("Conver to STA failed");
  }
  WiFi.setAutoConnect(true);//设置自动连接
  loadConfig();//读取ROM是否包含密码
  if(ssid!=0 && psw!=0){
    Serial.printf("ssid:%s ; psw:%s \r\n",ssid,psw);
    WiFi.begin(ssid, psw);//如果有密码则自动连接
    Serial.println("");
    Serial.print("Connecting to wifi");
    digitalWrite(PIN_LED_RED, LOW);
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      count++;
      if(count > 10){//如果10秒内没有连上，就开启Web配网 可适当调整这个时间
        wifiConfigInitSoftAP();
        initWebServer();
        wifiConfigInitDNS();
        Serial.println("Failed to connect config Wi-Fi, convert to SoftAP.");
        break;//跳出 防止无限初始化
      }
      Serial.print(".");
    }
  }
  else{
    Serial.println("No saved wifi config");
    wifiConfigInitSoftAP();
    initWebServer();
    wifiConfigInitDNS();
    digitalWrite(PIN_LED_GREEN, LOW); //Init 
    digitalWrite(PIN_LED_RED, LOW); //Init 
  }
  
  // WiFi.begin();//连接上一次连接成功的wifi

  Serial.println("");
  if(WiFi.status() == WL_CONNECTED){//如果连接上 就输出IP信息 防止未连接上break后会误输出
    Serial.println("WIFI Connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());//打印esp8266的IP地址
    server.stop();
    digitalWrite(PIN_LED_RED, HIGH);
  }
}