#include<ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
const char* ssid = "你的路由器名称";
const char* sspw = "路由器WiFi密码";
const char* stuno="学号";
const char* stupw="校园网密码";

/*以 下 勿 动*/
const char* url = "http://172.16.154.130:69/cgi-bin/srun_portal";
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.printf("\nConnecting to %s", ssid);
  WiFi.begin(ssid, sspw);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected");
  Serial.printf("IP adress: %s \n", WiFi.localIP().toString().c_str());
}

void loop() {
  // put your main code here, to run repeatedly:
  check_online();
}

char* user_encrypt(const char* username) {
  char usrtemp[13]={"\0"};
  strcpy(usrtemp,username);
  for (int i = 0; i < strlen(usrtemp); i++)
    usrtemp[i] += 4;
  char* usr = (char*)malloc(sizeof(char) * (strlen(usrtemp) + 1 + 9));
  memset(usr, '\0', sizeof(char) * (strlen(usrtemp) + 1 + 9));
  strcpy(usr, "{SRUN3}\r\n");
  strcat(usr, usrtemp);
  //Serial.printf("Username encrypted: %s\n", usr);
  return usr;
}
char* pw_encrypt(const char* password) {
  char key[] = {"1234567890"};
  char* pw = (char*)malloc(sizeof(char) * (2 * strlen(password) + 1));
  memset(pw, '\0', sizeof(char) * (2 * strlen(password) + 1));
  for (int i = 0; i < strlen(password); i++) {
    char ki = password[i] ^ key[10 - i % 10 - 1];
    char _l = (ki & 0x0f) + 0x36;
    char _h = (ki >> 4 & 0x0f) + 0x63;
    if (i % 2 == 0) {
      sprintf(pw, "%s%c%c", pw, _l, _h);
    }
    else {
      sprintf(pw, "%s%c%c", pw, _h, _l);
    }
  }
  //Serial.printf("password encrypted:%s\n", pw);
  return pw;
}
void check_online(){
  StaticJsonDocument<1024> doc;
  HTTPClient http;
  WiFiClient client;
  http.begin(client,url);
  http.addHeader("Accept", "application/json");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  char payload[200];
  strcpy(payload,"&action=login&n=117&mbytes=0&minutes=0&ac_id=1&mac=02:00:00:00:00:00&type=3&username=");
  char* usr=user_encrypt(stuno);
  strcat(payload,usr);
  free(usr);
  strcat(payload,"&password=");
  char* pw=pw_encrypt(stupw);
  strcat(payload,pw);
  free(pw);
  strcat(payload,"&drop=0&pop=1");
  //Serial.printf(payload);
  int httpCode = http.POST(payload);
  Serial.printf("Send POST request to: %s\n", url);
  Serial.printf("Server response: %s\n", http.getString().c_str());
  DeserializationError error = deserializeJson(doc, http.getString());
  if (error) {
    Serial.printf("deserializeJson() failed: ");
    Serial.println(error.c_str());
  } else {
    const char* error = doc["error"];
    Serial.println(error);
    delay(30000);
  }
  http.end();
  doc.clear();
}

String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    //Serial.println(encodedString);
    return encodedString;
    
}
