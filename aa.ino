#include <AES.h>
#include <AESLib.h>
#include <AES_config.h>
#include <xbase64.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include <time.h> 
#include <NTPClient.h>               
#include <TimeLib.h>
#include <ArduinoJson.h>
#include<Hash.h>
AES aes;


MDNSResponder mdns;
WiFiUDP ntpUDP;
 HTTPClient http;

NTPClient timeClient(ntpUDP, "time.nist.gov",0, 60000);
const char* ssid = "WiFi-BBEvol6082";
const char* password = "1BCE62731BC2F51BEECC434504";
IPAddress ip(198,168,1,158);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
ESP8266WebServer server(80);
/*void handleRoot() {
  server.send(200, "text/plain", "Root of WebServer!");
}*/
String webPage = "";
String stateString = "";
int gpio0_pin = 15;
int gpio2_pin = 2;
 int  state= 0;
char dateTime[]={"00,00,00,00,00,0000"};
char aa[]={"00,00,00,00,00,0000"};
String Password;
//String msg;
String UserName;
String dateTimeString;
String aaString;
   const char* UserNameAlby = "alberto";
   const char* PasswordAlby = "ghiaccio";
time_t dateTimeUtc1;
time_t aaUtc1;
  double diff;
  double diff1;
  int randomNumber=0;
  int Numero[4];
  int i =0;
  bool y = false;
   //tmElements_t ;
//int Year, Month, Day, Hour, Minute, Second ;

 
/* byte key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
// The unitialized Initialization vector (16-bit)
byte iv[N_BLOCK] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 

// NodeMCU side raw message
//String msg = "{\"x\":10, \"y\":20, \"z\":30 \"lamp_stat\":\"ON  This is a sentence. Testing testing testing\"}";

uint8_t generate_random_unit8()
{
  uint8_t really_random = *(volatile uint8_t *)0x3FF20E44;
  return really_random;
}

// Generate a random initialization vector
void generate_iv(byte *vector)
{
  for (int i = 0; i < N_BLOCK; i++)
  {
    vector[i] = (byte)generate_random_unit8();
  }
}
void encrypt()
{
  char b64data[200];
  byte cipher[1000];
  byte iv[N_BLOCK];

 // generate_iv(iv);

  base64_encode(b64data, (char *)iv, N_BLOCK);
  String IV_base64 = String(b64data);
  Serial.println(" IV b64: " + IV_base64);

  int b64len = base64_encode(b64data, (char *)msg.c_str(), msg.length());



  // Encrypt! With AES128, our key and IV, CBC and pkcs7 padding
  aes.do_aes_encrypt((byte *)b64data, b64len, cipher, key, 128, iv);

 

  base64_encode(b64data, (char *)cipher, aes.get_size());
  Serial.println("Encrypted data in base64: " + String(b64data));

  //decrypt(b64data, IV_base64, aes.get_size());
}
 
*/
/*void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authenticated()) {
    server.sendHeader("Location", "/");
    server.sendHeader("Cache-Control ", "no-cache");
    server.send(301);
    return;
  }
}*/
void setup() {
// webPage += "<h1>impostazioni</h1>  <input type="text" class="form-control" aria-label="Sizing example input" aria-describedby="inputGroup-sizing-default">";

  pinMode(gpio0_pin, OUTPUT);
  digitalWrite(gpio0_pin, LOW);
  pinMode(gpio2_pin, OUTPUT);
  digitalWrite(gpio2_pin, HIGH);
  delay(1000);
  Serial.begin(115200);
  if(!SPIFFS.begin()){
          Serial.println("An Error has occurred while mounting SPIFFS");
          return;
      }  
  WiFi.begin(ssid, password);
  WiFi.config(ip,gateway,subnet);
 
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connesso a ");
  Serial.println(ssid);
  Serial.print("Indirizzo IP: ");
  Serial.println(WiFi.localIP());
  digitalWrite(gpio2_pin, LOW);
  
   //  WiFiClient client = server.available();
//     header('Access-Control-Allow-Origin: http:/localhost:3000');
 /*server.enableCORS(true);
  server.sendHeader("Access-Control-Allow-Origin", "*");*/


  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }
 
  server.on("/RelaySwitch/SetState/1", []() {
    if(!is_authenticated())
    {
      server.send ( 200, "text/json", "{autenticazione:no}" ); 
       return;
    }
    
     state = 1;
              digitalWrite(gpio0_pin, HIGH);
               auto sJ = String( String(state));
    server.send(200, "application/json", sJ);
  });
  
  server.on("/RelaySwitch/SetState/0", []() {
      if(!is_authenticated())
    {
       server.send ( 200, "text/json", "{autenticazione:no}" ); 
       return;
    }
     state = 0;
              digitalWrite(gpio0_pin, LOW);
               auto sJ = String( String(state) );
    server.send(200, "application/json", sJ);
  });
 
  server.on("/RelaySwitch/GetState", []() {
       if(!is_authenticated())
    {
      server.send ( 200, "text/json", "{autenticazione:no}" ); 
       return;
    }
    auto sJ = String( String(state));
    server.send(200, "application/json", sJ);
  });

server.on("/RelaySwitch/GetStateDataInz", []() {
     if(!is_authenticated())
    {
      //server.send ( 200, "text/json", "{autenticazione:no}" ); 
       return;
    }
    auto sJ = String( String (dateTimeString));
    server.send(200, "application/json", sJ);
    Serial.println(dateTimeString);
  });

  server.on("/RelaySwitch/GetStateDataFin", []() {
     if(!is_authenticated())
    {
    //  server.send ( 200, "text/json", "{autenticazione:no}" ); 
       return;
    }
    auto sJ = String( String (aaString));
    server.send(200, "application/json", sJ);
     Serial.println(aaString);
  });

  
  server.on("/RelaySwitch/SetDataInz", HTTP_POST, [](){
       if(!is_authenticated())
    {
      server.send ( 200, "text/json", "{autenticazione:no}" ); 
       return;
    }
    auto json = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
    //serializeJson(doc, Serial);
    server.send ( 200, "text/json", "{success:true}" );
 const char* xxxx = doc["xxxx"];
long long int timeInz = doc["timeInz"];
   dateTimeString = xxxx;
   dateTimeUtc1= timeInz;
    
    for(int i =0; i <= dateTimeString.length(); i++)
   {
    dateTime[i] = dateTimeString[i];
    
   }
  
   Serial.println(dateTimeString);
      // Serial.println(dateTimeUtc1);
   });


server.on("/RelaySwitch/SetDataFin", HTTP_POST, [](){
     if(!is_authenticated())
    {
      server.send ( 200, "text/json", "{autenticazione:no}" ); 
       return;
    }
    auto json = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
   // serializeJson(doc, Serial);
    server.send ( 200, "text/json", "{success:true}" );
 const char* xxxx = doc["xxxx"];
 long long int timeFin = doc["timeFin"];
   aaString = xxxx;
   aaUtc1 = timeFin;
  
   for(int i =0; i <= aaString.length(); i++)
   {
    aa[i] = aaString[i];
   }
  
   Serial.println(aaString);
  //Serial.println(aaUtc1);
   });

  
  
  server.on("/Signin", HTTP_POST, [](){
       if(is_authenticated())
    {
      server.send ( 200, "text/json", "{autenticazione:non neccesaria}" ); 
       return;
    }
    auto json = server.arg("plain");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, json);
  //serializeJson(doc, Serial);
    const char* UserName1 = doc["UserName"];
    const char* Password1 = doc["Password"];
      Password = Password1;
      UserName = UserName1;       
     //encrypt();
       randomSeed(millis()); 
    randomNumber = random(0, 2147483647);
    handleLogin();
    server.send ( 200, "text/json", "{success:true}" );
   });
 
  

 #include "buildMap.h"
   server.collectHeaders( "Cookie");
  server.begin();
  Serial.println("HTTP server avviato");
 timeClient.begin();
  /* Serial.println(dateTimeString);
    Serial.println(aaString);*/
    
}


void  handleLogin() {
    // Serial.println("ooo");
    
    
  if (server.hasHeader("Cookie")){
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
 
    if (Password == PasswordAlby && UserName == UserNameAlby) {
server.sendHeader("Location", "/");
server.sendHeader("cache-control" ,"no-cache");
String token = String(randomNumber);
server.sendHeader("Set-Cookie", "ESPSESSIONID=" + token);
Numero[i]=token.toInt();
Serial.println("Log in Successful" );
for(int u =0; u <=3; u++)
{ 
  Serial.println(Numero[u]);
}

server.send(301);
i++;
  }
 }

bool is_authenticated() {
  Serial.println("Enter is_authenticated");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
   int cookie1 =cookie.indexOf("ESPSESSIONID=");
    if (cookie1 != -1) {
      for(int i =0;i <= 3; i++)
      {
    if(atoi(&cookie[cookie1+13]) == Numero[i]) {
     // Serial.println(atoi(&cookie[cookie1+13]));
      //   Serial.println(Numero[i]);
      Serial.println("Authentication Successful");
     return true;
    }
    }
    }
       Serial.println("Authentication Failed");
       return false;
  }
}

  
void loop() {
    server.handleClient();
    
//is_authenticated();
//creadate1(dateTime);
//setTime(makeTime(dateTimeUtc1));
//creadate2(aa);
//setTime(makeTime(aaUtc1));
 /* struct tm dateTimeUtc1;
   struct tm aaUtc1;
mktime(&dateTimeUtc1);
mktime(&aaUtc1);*/

   
  timeClient.update();
 time_t unix_t =timeClient.getEpochTime();

diff=difftime(unix_t, dateTimeUtc1); // dateTimeUtc
diff1=difftime(unix_t, aaUtc1); // aaUtc*/
/*Serial.println(diff);
Serial.println("inz");
Serial.println(diff1);
Serial.println("fin");*/
if(diff > 0 && diff1 < 0 )
{
  digitalWrite(gpio0_pin, HIGH); // || state
  state =1;
  y = true;
 }else{
  while(y == true){
    state=0;
    y= false;
  }
  digitalWrite(gpio0_pin, state);
  
  }
}

  

/*void creadate1(const char *str)
{
   sscanf(str, "%d:%d:%d %d/%d/%d", &Hour, &Minute, &Second, &Day, &Month, &Year);
  tmInz.Year = CalendarYrToTm(Year);
  tmInz.Month = Month;
  tmInz.Day = Day;
  tmInz.Hour = Hour;
  tmInz.Minute = Minute;
  tmInz.Second = Second;
}

void creadate2(const char *str)
{
   sscanf(str, "%d:%d:%d %d/%d/%d", &Hour, &Minute, &Second, &Day, &Month, &Year);
  tmFin.Year = CalendarYrToTm(Year);
  tmFin.Month = Month;
  tmFin.Day = Day;
  tmFin.Hour = Hour;
  tmFin.Minute = Minute;
  tmFin.Second = Second;
}*/

  
