#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "你的路由器名称";
const char* sspw = "路由器WiFi密码";
const char* stuno = "学号";
const char* stupw = "校园网密码";

/*以 下 勿 动*/
const char* url = "http://172.16.154.130:69/cgi-bin/srun_portal";
char dec2hex(short int c)
{
    if (0 <= c && c <= 9)
    {
        return c + '0';
    }
    else if (10 <= c && c <= 15)
    {
        return c + 'A' - 10;
    }
    else
    {
        return -1;
    }
}
 
//编码一个url 
void urlencode(char url[])
{
    int i = 0;
    int len = strlen(url);
    int res_len = 0;
    char res[50];
    for (i = 0; i < len; ++i)
    {
        char c = url[i];
        if (    ('0' <= c && c <= '9') ||
                ('a' <= c && c <= 'z') ||
                ('A' <= c && c <= 'Z') ||
                c == '/' || c == '.')
        {
            res[res_len++] = c;
        }
        else
        {
            int j = (short int)c;
            if (j < 0)
                j += 256;
            int i1, i0;
            i1 = j / 16;
            i0 = j - i1 * 16;
            res[res_len++] = '%';
            res[res_len++] = dec2hex(i1);
            res[res_len++] = dec2hex(i0);
        }
    }
    res[res_len] = '\0';
    strcpy(url, res);
}
void setup() {
  // put your setup code here, to run once:
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
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
  breath_on();
  check_online();
  breath_off();
  delay(10000);
}

char* user_encrypt(const char* username) {
  char usrtemp[13] = {"\0"};
  strcpy(usrtemp, username);
  for (int i = 0; i < strlen(usrtemp); i++)
    usrtemp[i] += 4;
  char* usr = (char*)malloc(sizeof(char) * ((strlen(usrtemp) + 9)*3+1));//9 for {SRUN3}\r\n *3 for urlencode,1 for '\0'
  memset(usr, '\0', sizeof(char) * ((strlen(usrtemp) + 9)*3+1));
  strcpy(usr, "{SRUN3}\r\n");
  strcat(usr, usrtemp);
  //Serial.printf("Username encrypted: %s\n", usr);
  urlencode(usr);
  return usr;
}
char* pw_encrypt(const char* password) {
  char key[] = {"1234567890"};
  char* pw = (char*)malloc(sizeof(char) * (6 * strlen(password) + 1));//*2 for encrypt,*3 for urlencode,1 for '\0'
  memset(pw, '\0', sizeof(char) * (6 * strlen(password) + 1));
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
  urlencode(pw);
  return pw;
}
void check_online() {
  HTTPClient http;
  http.begin(url);
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
}

void breath_off() {
  for (int i = 1; i < 1024; i++) {
    analogWrite(BUILTIN_LED, i);
    delay(2);
  }
}
void breath_on() {
  for (int i = 1022; i >= 0; i--) {
    analogWrite(BUILTIN_LED, i);
    delay(2);
  }
}
