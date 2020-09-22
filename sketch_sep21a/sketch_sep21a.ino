#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "你的路由器名称";
const char* sspw = "路由器WiFi密码";
const char* stuno = "学号";
const char* stupw = "校园网密码";

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
  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("\nReConnecting to %s", ssid);
    WiFi.begin(ssid, sspw);
    while (WiFi.status() != WL_CONNECTED) {
      delay(200);
      Serial.print(".");
    }
  }
  check_online();
}

char* user_encrypt(const char* username) {
  char usrtemp[13] = {"\0"};
  strcpy(usrtemp, username);
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
void check_online() {
  HTTPClient http;
  WiFiClient client;
  http.begin(client, url);
  http.addHeader("Accept", "application/json");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  char payload[200];
  strcpy(payload, "&action=login&n=117&mbytes=0&minutes=0&ac_id=1&mac=02:00:00:00:00:00&type=3&username=");
  char* usr = user_encrypt(stuno);
  strcat(payload, usr);
  free(usr);
  strcat(payload, "&password=");
  char* pw = pw_encrypt(stupw);
  strcat(payload, pw);
  free(pw);
  strcat(payload, "&drop=0&pop=1");
  //Serial.printf(payload);
  int httpCode = http.POST(payload);
  Serial.printf("Send POST request to: %s\n", url);
  Serial.printf("Server response: %s\n", http.getString().c_str());
  http.end();
  client.stop();
  doc.clear();
  delay(30000);
}
